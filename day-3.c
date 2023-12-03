#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Grid {
    uint32_t columns;
    uint32_t rows;
    char *grid;
} Grid;

void grid_init(Grid *grid) {
    grid->grid = calloc(grid->columns * grid->rows, sizeof(char));
}

void grid_destroy(Grid *grid) { free(grid->grid); }

size_t grid_get_index(Grid *grid, uint32_t row, uint32_t column) {
    return row * grid->columns + column;
}

char grid_get_char(Grid *grid, uint32_t row, uint32_t column) {
    return grid->grid[grid_get_index(grid, row, column)];
}

bool is_symbol(char c) { return c != '.' && !isdigit(c); }

/* check if digits are adjacent to any non-period symbols in grid */
bool is_symbol_adjacent(Grid *grid, size_t digits_length, uint32_t digits_row,
                        uint32_t digits_column) {
    /* check to the left of the digits */
    if (digits_column > 0 &&
        is_symbol(grid_get_char(grid, digits_row, digits_column - 1))) {
        return true;
    }

    /* check to the right of the digits */
    if (digits_column + digits_length < grid->columns &&
        is_symbol(
            grid_get_char(grid, digits_row, digits_column + digits_length))) {
        return true;
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
            if (is_symbol(grid_get_char(grid, digits_row - 1, column))) {
                return true;
            }
        }
    }

    /* check the row below digits */
    if (digits_row < grid->rows - 1) {
        for (int column = start_column; column < end_column; column++) {
            if (is_symbol(grid_get_char(grid, digits_row + 1, column))) {
                return true;
            }
        }
    }

    return false;
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

                // printf("%s\n", digit_str);

                if (is_symbol_adjacent(&grid, digits_length, row,
                                       column - digits_length)) {
                    sum += atoi(digit_str);
                }

                digits_length = 0;
            }
        }
    }

    // printf("%d\n", is_symbol_adjacent(&grid, 2, 0, 2));
    printf("%d\n", sum);

    grid_destroy(&grid);

    return 0;
}
