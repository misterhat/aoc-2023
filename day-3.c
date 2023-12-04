#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_TWO true

typedef struct Gear {
    size_t index;
    uint32_t parts[2];
} Gear;

typedef struct Grid {
    uint32_t columns;
    uint32_t rows;
    char *grid;

    size_t gears_length;
    Gear *gears;
} Grid;

void grid_init(Grid *grid) {
    grid->grid = calloc(grid->columns * grid->rows, sizeof(char));
    grid->gears = calloc((grid->columns * grid->rows) / 2, sizeof(Gear));
}

void grid_destroy(Grid *grid) {
    free(grid->grid);
    free(grid->gears);
}

size_t grid_get_index(Grid *grid, uint32_t row, uint32_t column) {
    return row * grid->columns + column;
}

bool is_symbol(char c) { return c != '.' && !isdigit(c); }

/* check if digits are adjacent to any non-period symbols in grid */
char grid_symbol_adjacent(Grid *grid, size_t digits_length, uint32_t digits_row,
                          uint32_t digits_column, int *symbol_index) {
    /* check to the left of the digits */
    if (digits_column > 0) {
        *symbol_index = grid_get_index(grid, digits_row, digits_column - 1);

        char c = grid->grid[*symbol_index];

        if (is_symbol(c)) {
            return c;
        }
    }

    /* check to the right of the digits */
    if (digits_column + digits_length < grid->columns) {
        *symbol_index =
            grid_get_index(grid, digits_row, digits_column + digits_length);

        char c = grid->grid[*symbol_index];

        if (is_symbol(c)) {
            return c;
        }
    }

    int start_column = digits_column - 1;

    if (start_column < 0) {
        start_column = 0;
    }

    int end_column = digits_column + digits_length + 1;

    if (end_column > grid->columns) {
        end_column = grid->columns;
    }

    /* check the row above digits */
    if (digits_row > 0) {
        for (int column = start_column; column < end_column; column++) {
            *symbol_index = grid_get_index(grid, digits_row - 1, column);

            char c = grid->grid[*symbol_index];

            if (is_symbol(c)) {
                return c;
            }
        }
    }

    /* check the row below digits */
    if (digits_row < grid->rows - 1) {
        for (int column = start_column; column < end_column; column++) {
            *symbol_index = grid_get_index(grid, digits_row + 1, column);

            char c = grid->grid[*symbol_index];

            if (is_symbol(c)) {
                return c;
            }
        }
    }

    return '\0';
}

void grid_gear_add(Grid *grid, size_t index, uint32_t part) {
    for (int i = 0; i < grid->gears_length; i++) {
        Gear *gear = grid->gears + i;

        if (grid->gears[i].index == index) {
            gear->parts[1] = part;
            return;
        }
    }

    Gear *gear = (grid->gears + grid->gears_length++);
    gear->index = index;
    gear->parts[0] = part;
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-3.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    Grid grid = {0};

    char *line = strtok(file_buffer, "\n");

    grid.columns = strlen(line);

    while (line) {
        grid.rows++;

        line = strtok(NULL, "\n");
    }

    grid_init(&grid);

    for (int row = 0; row < grid.rows; row++) {
        memcpy(grid.grid + (row * grid.columns),
               file_buffer + (row * (grid.columns + 1)), grid.columns);
    }

    uint32_t sum = 0;

    for (int row = 0; row < grid.rows; row++) {
        int digits_length = 0;

        for (int column = 0; column < grid.columns; column++) {
            size_t grid_index = row * grid.columns + column;

            bool parse_digits = false;

            if (isdigit(grid.grid[grid_index])) {
                digits_length++;

                /* handle end of line digits */
                if (column == grid.columns - 1) {
                    grid_index++;
                    column++;

                    parse_digits = true;
                }
            } else {
                if (digits_length > 0) {
                    parse_digits = true;
                }
            }

            if (parse_digits) {
                char digit_str[digits_length + 1];

                memcpy(digit_str, (grid.grid + grid_index) - digits_length,
                       digits_length);

                digit_str[digits_length] = '\0';

                int symbol_index = -1;

                char c =
                    grid_symbol_adjacent(&grid, digits_length, row,
                                         column - digits_length, &symbol_index);

                if (c != '\0') {
                    uint32_t part = atoi(digit_str);

                    if (c == '*') {
                        grid_gear_add(&grid, symbol_index, part);
                    }

                    if (!PART_TWO) {
                        sum += part;
                    }
                }

                digits_length = 0;
            }
        }
    }

    if (PART_TWO) {
        for (int i = 0; i < grid.gears_length; i++) {
            Gear gear = grid.gears[i];

            if (gear.parts[1] != 0) {
                sum += gear.parts[0] * gear.parts[1];
            }
        }
    }

    printf("%d\n", sum);

    grid_destroy(&grid);

    return 0;
}
