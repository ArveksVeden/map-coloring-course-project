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
