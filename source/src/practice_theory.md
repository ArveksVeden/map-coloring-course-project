## Постепенная теория [`main.c`]
текущий код `main.c`, который считывает карту и выводит основную информацию о ней:
```C
#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#define MAX_ROWS 100
#define MAX_COLS 100
#define MAX_REGIONS 1000

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    int id;             // номер региона
    Cell cells[1000];   // координаты всех клеток
    int cell_count;
    int color;          // цвет (по умолчанию -1)
} Region;

extern int map[MAX_ROWS][MAX_COLS];
extern int rows, cols;
extern Region regions[MAX_REGIONS];
extern int region_count;

void load_map(const char* filename);
void detect_regions();

#endif
```

**import библиотек**
```
#include <stdio.h>
#include "map_loader.h"
#include <windows.h>
```

, где:
- `#include <stdio.h>` - стандартная библиотека для ввода-вывода (printf, scanf и т.д.).
- `"map_loader.h"` - заголовочный файл, где объявлены функции и структуры для работы с картой.
- `<windows.h>` - заголовок Windows API. Нам он нужен только для установки кодировки консоли, чтобы поддерживать вывод Юникода/русских символов.

**точка входа и установка кодировки**
```C
int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
```
, где:
- `SetConsoleOutputCP(65001);` — устанавливает кодовую страницу консоли на UTF-8 для вывода.
- `SetConsoleCP(65001);` — устанавливает кодовую страницу консоли на UTF-8 для ввода (через scanf, getchar и т.д.).

**чтение карты и разбиение её на регионы**
```C
load_map("map.txt");          // Чтение карты
detect_regions();             // Объединение клеток в регионы
```

**Цикл по всем регионам**
```C
for (int i = 0; i < region_count; ++i) {
    Region* r = &regions[i];
    printf("Регион %d (значение: %d) — клеток: %d\n", i, r->id, r->cell_count);
    for (int j = 0; j < r->cell_count; ++j) {
        printf("  (%d, %d)\n", r->cells[j].x, r->cells[j].y);
    }
    printf("\n");
}
```
, где:
- `Region* r = &regions[i];` - указатель на текущий регион
- `r->id` - обращение к элементу структуры (Region), так как r - указатель на структуру
- `i` - номер региона в массиве (просто порядковый номер)
- `r->id` - значения клеток на карте (если регион состоин из '1' => id = 1)
---
## Постепенная теория [`map_loader.c`]
текущий код `map_loader.c`:
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map_loader.h"

int map[MAX_ROWS][MAX_COLS];
int visited[MAX_ROWS][MAX_COLS] = {0};
int rows = 0, cols = 0;
Region regions[MAX_REGIONS];
int region_count = 0;

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

void load_map(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Error opening map file");
        exit(1);
    }

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        int col = 0;
        char* token = strtok(line, " \n");
        while (token) {
            map[rows][col++] = atoi(token);
            token = strtok(NULL, " \n");
        }
        if (cols == 0) cols = col;
        rows++;
    }

    fclose(f);
}

void dfs(int x, int y, int region_id, int target_value) {
    if (x < 0 || y < 0 || x >= rows || y >= cols) return;
    if (visited[x][y]) return;
    if (map[x][y] != target_value) return;

    visited[x][y] = 1;
    Region* r = &regions[region_id];
    r->cells[r->cell_count++] = (Cell){x, y};

    for (int d = 0; d < 4; ++d) {
        dfs(x + dx[d], y + dy[d], region_id, target_value);
    }
}

void detect_regions() {
    region_count = 0;
    memset(visited, 0, sizeof(visited));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!visited[i][j] && map[i][j] != 0) {
                Region* r = &regions[region_count];
                r->id = map[i][j];
                r->cell_count = 0;
                r->color = -1;
                dfs(i, j, region_count, map[i][j]);
                region_count++;
            }
        }
    }
}
```
**, где:**

**Подключение библиотек**
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map_loader.h" - объявление структур, констант и функций
```

**Основные и глобальные переменные**
```C
int map[MAX_ROWS][MAX_COLS]; // Сама карта - двумерный массив
int visited[MAX_ROWS][MAX_COLS] = {0}; // Отмечает, были ли клетки уже посещены - при поиске регионов
int rows = 0, cols = 0;
Region regions[MAX_REGIONS]; // Массив всех найденных регионов
int region_count = 0;
```

**Вспомогательные массивы для движения по карте**
```C
int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};
```
- Для обхода по четырём направлениям: вверх, вправо, вниз, влево (используются в поиске в глубину — dfs).
- Например, `dx[0] = -1, dy[0] = 0` — движение вверх.

**Функции**
**1. Загрузка карты**
```C
void load_map(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Error opening map file");
        exit(1);
    }

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        int col = 0; // сколько стобцов в строке
        char* token = strtok(line, " \n"); // делим на "слова" - числа
        while (token) {
            map[rows][col++] = atoi(token); // atoi - перевод строки в число
            token = strtok(NULL, " \n");
        }
        if (cols == 0) cols = col; // если первая строка, то сохраняем кол-во столбцов
        rows++;
    }

    fclose(f);
}
```

**2. Поиск в глубину (DFS) - объединение клеток в регион**
```C
void dfs(int x, int y, int region_id, int target_value) {
    if (x < 0 || y < 0 || x >= rows || y >= cols) return;
    if (visited[x][y]) return;
    if (map[x][y] != target_value) return;

    visited[x][y] = 1;
    Region* r = &regions[region_id];
    r->cells[r->cell_count++] = (Cell){x, y};

    for (int d = 0; d < 4; ++d) {
        dfs(x + dx[d], y + dy[d], region_id, target_value);
    }
}
```
- Рекурсивная функция, находит все клетки, связанные с начальной, с тем же значением (`target_value`), и помечает их.
- Проверяет, что не вышли за границы, клетка ещё не посещена, значение подходит.
- Помечает клетку как посещённую (`visited[x][y] = 1`).
- Добавляет её координаты в регион (`cells` у текущего региона).
- Рекурсивно вызывает себя для всех соседей (вверх, вправо, вниз, влево).

**3. Обнаружение всех регионов**
```C
void detect_regions() {
    region_count = 0;
    memset(visited, 0, sizeof(visited));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!visited[i][j] && map[i][j] != 0) {
                Region* r = &regions[region_count];
                r->id = map[i][j];
                r->cell_count = 0;
                r->color = -1;
                dfs(i, j, region_count, map[i][j]);
                region_count++;
            }
        }
    }
}
```
- Обнуляет число регионов (region_count = 0).
- Сбрасывает все пометки посещения (memset).
- Проходит по всем клеткам карты.

  - Если клетка не посещена и не равна 0 (ноль — пустота/фон), начинает поиск нового региона:
    - Заводит новый регион:
        - Сохраняет значение клетки в id.
        - Сбрасывает количество клеток и цвет.
    - Запускает dfs с этой клетки, чтобы "собрать" все связанные клетки.
    - После завершения увеличивает счётчик регионов.


**Итог:**
- Сначала читается карта из файла и строится массив.
- Потом для каждой непосещённой клетки (которая не 0) запускается обход, который объединяет все связанные клетки в один регион.
- Все регионы сохраняются с их координатами, значениями и дополнительными параметрами (например, цвет — для будущей раскраски).


---
## Постепенная теория [`map_loader.h`]
текущий код `map_loader.h`:
```C
#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#define MAX_ROWS 100
#define MAX_COLS 100
#define MAX_REGIONS 1000

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    int id;             // номер региона
    Cell cells[1000];   // координаты всех клеток
    int cell_count;
    int color;          // цвет (по умолчанию -1)
} Region;

extern int map[MAX_ROWS][MAX_COLS];
extern int rows, cols;
extern Region regions[MAX_REGIONS];
extern int region_count;

void load_map(const char* filename);
void detect_regions();

#endif
```

**Защита от двойного включения**
```C
#ifndef MAP_LOADER_H
#define MAP_LOADER_H
...
#endif
```
Эта конструкция защищает от многократного включения заголовочного файла.
Если где-то несколько раз включат этот файл — компилятор всё равно подключит его только один раз.
Это называется "include guard" и всегда пишется в заголовках.

**Константы (макросы)**
```C
#define MAX_ROWS 100 // максимальное число строк в карте
#define MAX_COLS 100 // максимальное число столбцов
#define MAX_REGIONS 1000 // максимальное кол-во регионов может быть найдено
```

**Структура Cell**
```C
typedef struct {
    int x;
    int y;
} Cell;
```
- `Cell` — простая структура для хранения координат одной клетки:
  - `x` — номер строки (координата по вертикали)
  - `y` — номер столбца (координата по горизонтали)
- Используется для хранения информации о том, где именно находится каждая клетка региона.

**Структура Region**
```C
typedef struct {
    int id;             // номер региона (значение клетки)
    Cell cells[1000];   // массив всех клеток этого региона
    int cell_count;     // сколько реально клеток в этом регионе
    int color;          // цвет для раскраски (изначально -1)
} Region;
```

**Внешние переменные**
```C
extern int map[MAX_ROWS][MAX_COLS];
extern int rows, cols;
extern Region regions[MAX_REGIONS];
extern int region_count;
```
- Ключевое слово extern сообщает компилятору:
  "Эти переменные объявлены где-то в другом файле (в нашем случае — в map_loader.c), но будут использоваться и в других файлах, которые подключат этот заголовок."
- Это позволяет обращаться к этим переменным из других частей программы, не дублируя их определение.

**Прототипы функций**
```C
void load_map(const char* filename);
void detect_regions();
```
- Объявления функций (прототипы) — говорят компилятору, что такие функции есть, и их можно использовать.
- Реализация этих функций находится в map_loader.c.