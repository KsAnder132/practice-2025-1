#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <ctime>
#include <map>
#include <tuple>
#include <algorithm>
#include <random>

// ��������� ����
constexpr int WINDOW_WIDTH = 422;          // ������ ����
constexpr int WINDOW_HEIGHT = 750;         // ������ ����
constexpr float OUTER_BLOCK_WIDTH = 20;    // ������ ������� ����� �����
constexpr float INNER_BLOCK_WIDTH = 18;     // ������ ���������� ����� �����
constexpr float GRID_X = 110;               // X-���������� ������ �������� ����
constexpr float GRID_Y = 350;               // Y-���������� ������ �������� ����
constexpr int STEP_RATE_IN_MILLISECONDS = 300; // �������� ����� ������ ������ (��)
constexpr int MAX_RND_NUM = 6;              // ������������ ����� ��� ������ ��������� ������
constexpr int STARTING_PIVOT[] = { 5, 0 };    // ��������� ����� ��������
constexpr int SQUARE_IN_PIECE_ARRAY = 0;    // ������ ���������� ������ � �������

// ���������� �������
Uint32 lastTime = SDL_GetTicks();          // ����� ���������� �����
Uint32 accumulator = 0;                    // ����������� �������

// ����������� �������
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// ����� �����
enum squareColor {
    RED,     // �������
    BLUE,    // �����
    GREEN,   // �������
    YELLOW,  // ������
    PURPLE,  // ����������
    ORANGE,  // ���������
    CYAN,    // ���������
    BLANK    // �����
};

// ����������� ��������
enum directionInput {
    LEFT,   // �����
    RIGHT   // ������
};

// ������������ ������ RGB-���������
std::map<squareColor, std::tuple<int, int, int>> colorToRGB = {
    {RED, {255, 0, 0}},
    {GREEN, {0, 255, 0}},
    {BLUE, {0, 0, 255}},
    {YELLOW, {255, 255, 0}},
    {PURPLE, {128, 0, 128}},
    {ORANGE, {250, 156, 28}},
    {CYAN, {0, 255, 255}}
};

bool isActivePieceTheSquare = false;        // ����, �������� �� ������� ������ ���������
squareColor board_state[10][20];            // ��������� �������� ���� (����� ������)

// ��������� ���������� ����� (7 �����, �� 4 �����, ���������� x � y)
int piece_starting_coordinates[7][4][2] = {
    {{4, 0},{5, 0},{4, 1},{5, 1}}, // �������
    {{4, 0},{5, 0},{6, 0},{7, 0}}, // �����
    {{4, 1},{5, 1},{5, 0},{6, 0}}, // Z-��������
    {{4, 0},{5, 0},{5, 1},{6, 1}}, // S-��������
    {{4, 0},{5, 0},{6, 0},{4, 1}}, // L-��������
    {{4, 0},{5, 0},{6, 0},{6, 1}}, // J-��������
    {{4, 0},{5, 0},{6, 0},{5, 1}}  // T-��������
};

int active_coordinates[4][2];               // ���������� �������� ������
int active_pivot_point[2];                  // ����� �������� �������� ������
squareColor active_color;                   // ���� �������� ������

// ��������, �� �������� �� ���������� ������ �������� ������
bool is_not_active_coord(int x, int y) {
    for (int i = 0; i < 4; i++) {
        if (active_coordinates[i][0] == x && active_coordinates[i][1] == y) {
            return false;
        }
    }
    return true;
}

// �������� ����������� �������� ����
bool can_active_move_down() {
    for (int i = 0; i < 4; i++) {
        int current_X = active_coordinates[i][0];
        int new_Y = active_coordinates[i][1] + 1;
        if ((board_state[current_X][new_Y] != BLANK && is_not_active_coord(current_X, new_Y)) || new_Y >= 20) {
            return false;
        }
    }
    return true;
}

// ����������� ������ ����
void move_active_down() {
    // ������� ������ �������
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = BLANK;
        active_coordinates[i][1]++;
    }
    // ��������� ����� �������
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = active_color;
    }
    active_pivot_point[1] += 1; // ���������� Y-���������� ������ ��������
}

// ��������� ������ �����
void draw_tetris_square(SDL_Renderer* renderer, float x_cor, float y_cor, squareColor sColor) {
    int rgbFirst = std::get<0>(colorToRGB[sColor]);
    int rgbSecond = std::get<1>(colorToRGB[sColor]);
    int rgbThird = std::get<2>(colorToRGB[sColor]);

    // ������� ������������� (�����)
    SDL_FRect outline = { x_cor, y_cor, OUTER_BLOCK_WIDTH, OUTER_BLOCK_WIDTH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // ����� ����
    SDL_RenderFillRect(renderer, &outline);

    // ���������� ������������� (�������� ����)
    SDL_FRect rect = { x_cor + 1, y_cor + 1, INNER_BLOCK_WIDTH, INNER_BLOCK_WIDTH };
    SDL_SetRenderDrawColor(renderer, rgbFirst, rgbSecond, rgbThird, 255);
    SDL_RenderFillRect(renderer, &rect);
}

// ��������� ���� ������
void draw_tetronimo(SDL_Renderer* renderer, int coords[4][2], squareColor color) {
    for (int i = 0; i < 4; i++) {
        draw_tetris_square(renderer, GRID_X + (coords[i][0] * OUTER_BLOCK_WIDTH),
            GRID_Y + (coords[i][1] * OUTER_BLOCK_WIDTH), color);
    }
}

// ��������� ����� �������� ����
void draw_tetris_grid(SDL_Renderer* renderer) {
    // ������������ �����
    for (int i = 0; i < 11; i++) {
        SDL_RenderLine(renderer, GRID_X + (OUTER_BLOCK_WIDTH * i), GRID_Y,
            GRID_X + (OUTER_BLOCK_WIDTH * i), WINDOW_HEIGHT);
    }
    // �������������� �����
    for (int i = 0; i < 21; i++) {
        SDL_RenderLine(renderer, GRID_X, GRID_Y + (OUTER_BLOCK_WIDTH * i),
            310, GRID_Y + (OUTER_BLOCK_WIDTH * i)); // 310 = GRID_X + (20 ������ * ������)
    }
}

// ��������� ����� ���� �� ������ board_state
void draw_from_board_state(SDL_Renderer* renderer) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            if (board_state[i][j] != BLANK) {
                draw_tetris_square(renderer, GRID_X + (i * OUTER_BLOCK_WIDTH),
                    GRID_Y + (j * OUTER_BLOCK_WIDTH), board_state[i][j]);
            }
        }
    }
}

// �������� ����� ������
void spawn_new_piece(int randomNumber) {
    active_color = squareColor(randomNumber);
    isActivePieceTheSquare = (randomNumber == SQUARE_IN_PIECE_ARRAY);

    for (int i = 0; i < 4; i++) {
        active_coordinates[i][0] = piece_starting_coordinates[randomNumber][i][0];
        active_coordinates[i][1] = piece_starting_coordinates[randomNumber][i][1];
    }
    active_pivot_point[0] = STARTING_PIVOT[0];
    active_pivot_point[1] = STARTING_PIVOT[1];
}

// �������� ����������� ��������������� ��������
bool can_active_move_horizontal(directionInput direction) {
    int direction_adjustment = (direction == RIGHT ? 1 : -1); // +1 ��� ������, -1 ��� �����
    for (int i = 0; i < 4; i++) {
        int current_Y = active_coordinates[i][1];
        int new_X = active_coordinates[i][0] + direction_adjustment;
        if ((board_state[new_X][current_Y] != BLANK && is_not_active_coord(new_X, current_Y)) ||
            new_X >= 11 || new_X <= -1) {
            return false;
        }
    }
    return true;
}

// �������������� ����������� ������
void move_active_piece_horizontal(directionInput direction) {
    int move_direction = (direction == RIGHT ? 1 : -1);
    // ������� ������ �������
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = BLANK;
        active_coordinates[i][0] += move_direction;
    }
    // ��������� ����� �������
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = active_color;
    }
    active_pivot_point[0] += move_direction; // ���������� X-���������� ������ ��������
}

// �������� ����������� ������
void delete_full_row(int row) {
    // ������� ������
    for (int i = 0; i < 10; i++) {
        board_state[i][row] = BLANK;
    }
    // ����� ���� ����� ���� ����
    for (int y = row; y > 0; y--) {
        for (int x = 0; x < 10; x++) {
            board_state[x][y] = board_state[x][y - 1];
        }
    }
    // ������� ������� ������
    for (int i = 0; i < 10; i++) {
        board_state[i][0] = BLANK;
    }
}

// �������� ����������� �����
void check_full_rows() {
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            if (board_state[x][y] == BLANK) {
                break;
            }
            else if (x == 9) { // ���� ����� �� ����� ������ � ��� ���������
                delete_full_row(y);
            }
        }
    }
}

// �������� �������� ������
void rotate_active_piece() {
    if (isActivePieceTheSquare) return; // ������� �� ���������

    int temp_coordinates[4][2];
    squareColor temp_board_state[10][20];

    // ����������� �������� ��������� ���� �� ���������
    std::copy(&board_state[0][0], &board_state[0][0] + 10 * 20, &temp_board_state[0][0]);

    // �������� �������� ������ �� ���������� ���������
    for (int i = 0; i < 4; i++) {
        temp_board_state[active_coordinates[i][0]][active_coordinates[i][1]] = BLANK;
    }

    // ���������� ����� ��������� ����� ��������
    for (int i = 0; i < 4; i++) {
        // ������� �������� �� 90 ��������
        temp_coordinates[i][0] = -1 * (active_coordinates[i][1] - active_pivot_point[1]);
        temp_coordinates[i][1] = active_coordinates[i][0] - active_pivot_point[0];
        // ����������� � ���������� �����������
        temp_coordinates[i][0] += active_pivot_point[0];
        temp_coordinates[i][1] += active_pivot_point[1];
    }

    // �������� �� ����������� ��������
    for (int i = 0; i < 4; i++) {
        if (temp_board_state[temp_coordinates[i][0]][temp_coordinates[i][1]] != BLANK ||
            temp_coordinates[i][0] < 0 || temp_coordinates[i][0] > 9 ||
            temp_coordinates[i][1] < 0 || temp_coordinates[i][1] > 19) {
            return; // ������� ����������
        }
    }

    // ���������� ��������
    for (int i = 0; i < 4; i++) {
        temp_board_state[temp_coordinates[i][0]][temp_coordinates[i][1]] = active_color;
        active_coordinates[i][0] = temp_coordinates[i][0];
        active_coordinates[i][1] = temp_coordinates[i][1];
    }

    // ����������� ���������� ��������� �������
    std::copy(&temp_board_state[0][0], &temp_board_state[0][0] + 10 * 20, &board_state[0][0]);
}

// ������������� SDL
bool initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("������ ������������� SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("������ �� SDL3", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        SDL_Log("������ �������� ����: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("������ �������� ���������: %s", SDL_GetError());
        return false;
    }
    return true;
}

// ���������� ������
void shutdown() {
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
        renderer = nullptr;
    }
    SDL_Quit();
}

// ������� �������
int main(int argc, char* args[]) {
    if (!initialize()) {
        return 1;
    }

    // ��������� ���������� ��������� �����
    std::mt19937 gen(time(0));
    std::uniform_int_distribution<> distrib(0, MAX_RND_NUM);

    bool running = true;
    SDL_Event event;

    // ������������� ������� �������� ����
    for (int i = 0; i < 10; i++) {
        std::fill_n(board_state[i], 20, BLANK);
    }

    // �������� ������ ������
    spawn_new_piece(distrib(gen));

    // ������� ������� ����
    while (running) {
        // ��������� �������
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false; // ����� ��� �������� ����
                break;

            case SDL_EVENT_KEY_DOWN:
                // ��������� ������� ������
                switch (event.key.scancode) {
                case SDL_SCANCODE_RIGHT: // ������
                    if (can_active_move_horizontal(RIGHT)) {
                        move_active_piece_horizontal(RIGHT);
                    }
                    break;
                case SDL_SCANCODE_UP: // ��������
                    rotate_active_piece();
                    break;
                case SDL_SCANCODE_LEFT: // �����
                    if (can_active_move_horizontal(LEFT)) {
                        move_active_piece_horizontal(LEFT);
                    }
                    break;
                case SDL_SCANCODE_DOWN: // ��������� ����
                    if (can_active_move_down()) {
                        move_active_down();
                    }
                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        // ���������� �������� � ��������� ����
        Uint32 currentTime = SDL_GetTicks(); // ������� ����� � �������������
        Uint32 deltaTime = currentTime - lastTime; // ����� � ���������� �����
        lastTime = currentTime; // ���������� �������
        accumulator += deltaTime;

        // �������������� �������� ������ ����
        if (accumulator >= STEP_RATE_IN_MILLISECONDS) {
            if (can_active_move_down()) {
                move_active_down();
            }
            else {
                // �������� ����������� ����� � �������� ����� ������
                check_full_rows();
                spawn_new_piece(distrib(gen));
            }
            accumulator -= STEP_RATE_IN_MILLISECONDS;
        }

        // ���������
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // ������ ���
        SDL_RenderClear(renderer);

        // ��������� �������� ���� � �����
        draw_from_board_state(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // ����� �����
        draw_tetris_grid(renderer);

        // ���������� ������
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
    }

    // ���������� ������
    shutdown();
    return 0;
}