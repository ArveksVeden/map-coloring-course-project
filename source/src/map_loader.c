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
