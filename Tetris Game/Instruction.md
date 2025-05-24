# Инструкция по созданию игры "Тетрис" на C++ с SDL3 в Visual Studio

## Важно!
Из-за того, что туториал по созданию логики игры "Тетрис" на C++ устарел (он 2008 года). Многие вещи пришлось поменять. Например, пришлось использовать SDL3 вместо обычного SDL или SDL2, потому что это самая доступная и стабильная версия на данный момент. Поэтому большую часть кода пришлось переписывать...

## Часть 1: Настройка Visual Studio и подключение SDL3
### Установка Visual Studio

1) Скачайте Visual Studio Community с официального сайта Microsoft

2) При установке выберите рабочую нагрузку "Разработка на C++"

### Скачивание SDL3

1) Перейдите на официальный сайт SDL (https://libsdl.org/)

2) На странице будет надпись "Get the current stable SDL version 3.2.14"
Вы перейдёте на GitHub.
Для Windows скачайте "SDL3-devel-3.2.14-VC.zip"

### Создание проекта в Visual Studio

1) Запустите Visual Studio

2) Выберите "Создание проекта"
Выберите "Пустой проект" с указанием языка C++ и Windows
Укажите имя проекта (например, "TetrisSDL3") и расположение

### Настройка каталогов SDL3

1) Распакуйте скачанный архив SDL3 в папку с созданным проектом

2) В обозревателе решений кликните "Проект" -> "Свойства"

3) В открывшемся окне кликните "Каталоги VC++" -> "Включаемые каталоги" и укажите путь до папки include в скачанном SDL3
Например: C:\Users\Роман\source\repos\SDL3Game\Tetriss\SDL3-3.2.14\include

4) В этом же окне выбираем "Компоновщик" -> "Общие" и ищем там "Дополнительные каталоги библиотек". Указываем путь до папки lib, в которой выбираем нужную битность (зависит от вашего компьютера)
Например: C:\Users\Роман\source\repos\SDL3Game\Tetriss\SDL3-3.2.14\lib\x64;%(AdditionalLibraryDirectories)

5) В этом же окне выбираем "Компоновщик" -> "Ввод" и ищем "Дополнительные зависимости". Здесь через Enter указываем файлы SDL3.lib и SDL3_test.lib

## Часть 2: Создание кода игры
### Добавление основного файла

1) В обозревателе решений кликните правой кнопкой по "Файлы ресурсов"

2) Выберите "Добавить" → "Новый элемент"

3) Создайте файл main.cpp

### Написание кода в main.cpp

1) Добавьте необходимые заголовочные файлы:

'''

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <ctime>
#include <map>
#include <tuple>
#include <algorithm>
#include <random>
'''

2) Добавьте константы для размеров окна, блоков и других параметров:
   
'''

constexpr int WINDOW_WIDTH = 422;          // Ширина окна
constexpr int WINDOW_HEIGHT = 750;         // Высота окна
constexpr float OUTER_BLOCK_WIDTH = 20;    // Ширина внешней части блока
constexpr float INNER_BLOCK_WIDTH = 18;     // Ширина внутренней части блока
constexpr float GRID_X = 110;               // X-координата начала игрового поля
constexpr float GRID_Y = 350;               // Y-координата начала игрового поля
constexpr int STEP_RATE_IN_MILLISECONDS = 300; // Интервал между шагами фигуры (мс)
constexpr int MAX_RND_NUM = 6;              // Максимальное число для выбора следующей фигуры
constexpr int STARTING_PIVOT[] = { 5, 0 };    // Начальная точка вращения
constexpr int SQUARE_IN_PIECE_ARRAY = 0;    // Индекс квадратной фигуры в массиве
'''

3) Добавьте переменные времени, которые помогут с работой игры
   
'''

Uint32 lastTime = SDL_GetTicks();          // Время последнего кадра
Uint32 accumulator = 0;                    // Аккумулятор времени
'''

4) Добавьте основные объекты, которые важны для запуска игры
   
'''

SDL_Window* window = nullptr;       //Запуск окна
SDL_Renderer* renderer = nullptr;   //Рендер окна
'''

5) Создайте переменные enum для хранения цветов и направлений движения фигур. Учтите, что каждая фигура имеет только один цвет (поэтому их всего 7)
   
'''

enum squareColor {
    RED,     // Красный
    BLUE,    // Синий
    GREEN,   // Зеленый
    YELLOW,  // Желтый
    PURPLE,  // Фиолетовый
    ORANGE,  // Оранжевый
    CYAN,    // Бирюзовый
    BLANK    // Пусто
};

enum directionInput {
    LEFT,   // Влево
    RIGHT   // Вправо
};
'''

6) Задайте цветам их числовые обозначения RGB:
   
'''

std::map<squareColor, std::tuple<int, int, int>> colorToRGB = {
    {RED, {255, 0, 0}},
    {GREEN, {0, 255, 0}},
    {BLUE, {0, 0, 255}},
    {YELLOW, {255, 255, 0}},
    {PURPLE, {128, 0, 128}},
    {ORANGE, {250, 156, 28}},
    {CYAN, {0, 255, 255}}
};
'''

7) Сделайте флаг и задайте состояние игрового поля:
   
'''

bool isActivePieceTheSquare = false;        // Флаг, является ли текущая фигура квадратом
squareColor board_state[10][20];            // Состояние игрового поля (цвета клеток)
'''

8) Задайте начальные координаты появления фигур (7 фигур, по 4 блока, координаты x и y):
   
'''

int piece_starting_coordinates[7][4][2] = {
    {{4, 0},{5, 0},{4, 1},{5, 1}}, // Квадрат
    {{4, 0},{5, 0},{6, 0},{7, 0}}, // Палка
    {{4, 1},{5, 1},{5, 0},{6, 0}}, // Z-образная
    {{4, 0},{5, 0},{5, 1},{6, 1}}, // S-образная
    {{4, 0},{5, 0},{6, 0},{4, 1}}, // L-образная
    {{4, 0},{5, 0},{6, 0},{6, 1}}, // J-образная
    {{4, 0},{5, 0},{6, 0},{5, 1}}  // T-образная
};
'''

9) Продумайте работу с активной фигурой:
    
'''

int active_coordinates[4][2];               // Координаты активной фигуры
int active_pivot_point[2];                  // Центр вращения активной фигуры
squareColor active_color;                   // Цвет активной фигуры
'''

10) Пропишите функции, которые позволят взаимодействовать с фигурой:
    
'''

// Проверка, не является ли координата частью активной фигуры
bool is_not_active_coord(int x, int y) {
    for (int i = 0; i < 4; i++) {
        if (active_coordinates[i][0] == x && active_coordinates[i][1] == y) {
            return false;
        }
    }
    return true;
}

// Проверка возможности движения вниз
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

// Перемещение фигуры вниз
void move_active_down() {
    // Очистка старых позиций
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = BLANK;
        active_coordinates[i][1]++;
    }
    // Установка новых позиций
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = active_color;
    }
    active_pivot_point[1] += 1; // Обновление Y-координаты центра вращения
}

// Отрисовка одного блока
void draw_tetris_square(SDL_Renderer* renderer, float x_cor, float y_cor, squareColor sColor) {
    int rgbFirst = std::get<0>(colorToRGB[sColor]);
    int rgbSecond = std::get<1>(colorToRGB[sColor]);
    int rgbThird = std::get<2>(colorToRGB[sColor]);

    // Внешний прямоугольник (рамка)
    SDL_FRect outline = { x_cor, y_cor, OUTER_BLOCK_WIDTH, OUTER_BLOCK_WIDTH };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Белый цвет
    SDL_RenderFillRect(renderer, &outline);

    // Внутренний прямоугольник (основной цвет)
    SDL_FRect rect = { x_cor + 1, y_cor + 1, INNER_BLOCK_WIDTH, INNER_BLOCK_WIDTH };
    SDL_SetRenderDrawColor(renderer, rgbFirst, rgbSecond, rgbThird, 255);
    SDL_RenderFillRect(renderer, &rect);
}

// Отрисовка всей фигуры
void draw_tetronimo(SDL_Renderer* renderer, int coords[4][2], squareColor color) {
    for (int i = 0; i < 4; i++) {
        draw_tetris_square(renderer, GRID_X + (coords[i][0] * OUTER_BLOCK_WIDTH),
            GRID_Y + (coords[i][1] * OUTER_BLOCK_WIDTH), color);
    }
}

// Отрисовка сетки игрового поля
void draw_tetris_grid(SDL_Renderer* renderer) {
    // Вертикальные линии
    for (int i = 0; i < 11; i++) {
        SDL_RenderLine(renderer, GRID_X + (OUTER_BLOCK_WIDTH * i), GRID_Y,
            GRID_X + (OUTER_BLOCK_WIDTH * i), WINDOW_HEIGHT);
    }
    // Горизонтальные линии
    for (int i = 0; i < 21; i++) {
        SDL_RenderLine(renderer, GRID_X, GRID_Y + (OUTER_BLOCK_WIDTH * i),
            310, GRID_Y + (OUTER_BLOCK_WIDTH * i)); // 310 = GRID_X + (20 блоков * ширина)
    }
}

// Отрисовка всего поля на основе board_state
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

// Создание новой фигуры
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

// Проверка возможности горизонтального движения
bool can_active_move_horizontal(directionInput direction) {
    int direction_adjustment = (direction == RIGHT ? 1 : -1); // +1 для вправо, -1 для влево
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

// Горизонтальное перемещение фигуры
void move_active_piece_horizontal(directionInput direction) {
    int move_direction = (direction == RIGHT ? 1 : -1);
    // Очистка старых позиций
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = BLANK;
        active_coordinates[i][0] += move_direction;
    }
    // Установка новых позиций
    for (int i = 0; i < 4; i++) {
        board_state[active_coordinates[i][0]][active_coordinates[i][1]] = active_color;
    }
    active_pivot_point[0] += move_direction; // Обновление X-координаты центра вращения
}

// Удаление заполненной строки
void delete_full_row(int row) {
    // Очистка строки
    for (int i = 0; i < 10; i++) {
        board_state[i][row] = BLANK;
    }
    // Сдвиг всех строк выше вниз
    for (int y = row; y > 0; y--) {
        for (int x = 0; x < 10; x++) {
            board_state[x][y] = board_state[x][y - 1];
        }
    }
    // Очистка верхней строки
    for (int i = 0; i < 10; i++) {
        board_state[i][0] = BLANK;
    }
}

// Проверка заполненных строк
void check_full_rows() {
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            if (board_state[x][y] == BLANK) {
                break;
            }
            else if (x == 9) { // Если дошли до конца строки и все заполнены
                delete_full_row(y);
            }
        }
    }
}

// Вращение активной фигуры
void rotate_active_piece() {
    if (isActivePieceTheSquare) return; // Квадрат не вращается

    int temp_coordinates[4][2];
    squareColor temp_board_state[10][20];

    // Копирование текущего состояния поля во временное
    std::copy(&board_state[0][0], &board_state[0][0] + 10 * 20, &temp_board_state[0][0]);

    // Удаление активной фигуры из временного состояния
    for (int i = 0; i < 4; i++) {
        temp_board_state[active_coordinates[i][0]][active_coordinates[i][1]] = BLANK;
    }

    // Вычисление новых координат после поворота
    for (int i = 0; i < 4; i++) {
        // Матрица поворота на 90 градусов
        temp_coordinates[i][0] = -1 * (active_coordinates[i][1] - active_pivot_point[1]);
        temp_coordinates[i][1] = active_coordinates[i][0] - active_pivot_point[0];
        // Возвращение к глобальным координатам
        temp_coordinates[i][0] += active_pivot_point[0];
        temp_coordinates[i][1] += active_pivot_point[1];
    }

    // Проверка на возможность поворота
    for (int i = 0; i < 4; i++) {
        if (temp_board_state[temp_coordinates[i][0]][temp_coordinates[i][1]] != BLANK ||
            temp_coordinates[i][0] < 0 || temp_coordinates[i][0] > 9 ||
            temp_coordinates[i][1] < 0 || temp_coordinates[i][1] > 19) {
            return; // Поворот невозможен
        }
    }

    // Применение поворота
    for (int i = 0; i < 4; i++) {
        temp_board_state[temp_coordinates[i][0]][temp_coordinates[i][1]] = active_color;
        active_coordinates[i][0] = temp_coordinates[i][0];
        active_coordinates[i][1] = temp_coordinates[i][1];
    }

    // Копирование временного состояния обратно
    std::copy(&temp_board_state[0][0], &temp_board_state[0][0] + 10 * 20, &board_state[0][0]);
}
'''

11) Создайте функцию инициализации SDL:
    
'''

bool initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Ошибка инициализации SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Тетрис на SDL3", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        SDL_Log("Ошибка создания окна: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Ошибка создания рендерера: %s", SDL_GetError());
        return false;
    }
    return true;
}
'''

12) Пропишите функцию завершения работы игры:
    
'''

void shutdown() {
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
        renderer = nullptr;
    }
    SDL_Quit();
}
'''

13) Пропишите функцию main:
    
'''

int main(int argc, char* args[]) {
    if (!initialize()) {
        return 1;
    }

    // Настройка генератора случайных чисел
    std::mt19937 gen(time(0));
    std::uniform_int_distribution<> distrib(0, MAX_RND_NUM);

    bool running = true;
    SDL_Event event;

    // Инициализация пустого игрового поля
    for (int i = 0; i < 10; i++) {
        std::fill_n(board_state[i], 20, BLANK);
    }

    // Создание первой фигуры
    spawn_new_piece(distrib(gen));

    // Главный игровой цикл
    while (running) {
        // Обработка событий
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false; // Выход при закрытии окна
                break;

            case SDL_EVENT_KEY_DOWN:
                // Обработка нажатий клавиш
                switch (event.key.scancode) {
                case SDL_SCANCODE_RIGHT: // Вправо
                    if (can_active_move_horizontal(RIGHT)) {
                        move_active_piece_horizontal(RIGHT);
                    }
                    break;
                case SDL_SCANCODE_UP: // Вращение
                    rotate_active_piece();
                    break;
                case SDL_SCANCODE_LEFT: // Влево
                    if (can_active_move_horizontal(LEFT)) {
                        move_active_piece_horizontal(LEFT);
                    }
                    break;
                case SDL_SCANCODE_DOWN: // Ускорение вниз
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

        // Управление временем и скоростью игры
        Uint32 currentTime = SDL_GetTicks(); // Текущее время в миллисекундах
        Uint32 deltaTime = currentTime - lastTime; // Время с последнего кадра
        lastTime = currentTime; // Обновление времени
        accumulator += deltaTime;

        // Автоматическое движение фигуры вниз
        if (accumulator >= STEP_RATE_IN_MILLISECONDS) {
            if (can_active_move_down()) {
                move_active_down();
            }
            else {
                // Проверка заполненных строк и создание новой фигуры
                check_full_rows();
                spawn_new_piece(distrib(gen));
            }
            accumulator -= STEP_RATE_IN_MILLISECONDS;
        }

        // Отрисовка
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Черный фон
        SDL_RenderClear(renderer);

        // Отрисовка игрового поля и сетки
        draw_from_board_state(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Белая сетка
        draw_tetris_grid(renderer);

        // Обновление экрана
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
    }

    // Завершение работы
    shutdown();
    return 0;
}
'''

### Часть 3: Компиляция и запуск

1) В верхней панели Visual Studio выберите "Debug" и "x64"

2) Нажмите "Локальный отладчик Windos"

Теперь у вас должна быть готова игра Тетрис с помощью C++ и SDL3.
