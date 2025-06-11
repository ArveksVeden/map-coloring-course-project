#include <stdio.h>
#include "map_loader.h"
#include <windows.h>

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    load_map("map.txt");          // Чтение карты
    detect_regions();             // Объединение клеток в регионы

    printf("Карта загружена: %d строк, %d столбцов\n", rows, cols);
    printf("Найдено регионов: %d\n\n", region_count);

    for (int i = 0; i < region_count; ++i) {
        Region* r = &regions[i];
        printf("Регион %d (значение: %d) — клеток: %d\n", i, r->id, r->cell_count);
        for (int j = 0; j < r->cell_count; ++j) {
            printf("  (%d, %d)\n", r->cells[j].x, r->cells[j].y);
        }
        printf("\n");
    }

    return 0;
}
