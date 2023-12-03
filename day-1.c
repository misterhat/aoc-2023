#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_TWO true

const char *number_names[] = {"one", "two",   "three", "four", "five",
                              "six", "seven", "eight", "nine"};

/* grab the first and last digit (either a digit or a number name) and turn
 * into an integer */
uint32_t get_line_int(const char *line) {
    int first_digit_index = -1;
    int last_digit_index = -1;

    int first_name_index = -1;
    int last_name_index = -1;

    /* initially set with the number name ints, but overriden if digit
     * precedes or follows them respectively */
    uint32_t first_int = 0;
    uint32_t last_int = 0;

    for (int i = 0; i < strlen(line); i++) {
        if (isdigit(line[i])) {
            if (first_digit_index == -1) {
                first_digit_index = i;
            }

            last_digit_index = i;

            /* if it's a digit then it can't be a name too */
            continue;
        }

        if (!PART_TWO) {
            continue;
        }

        for (int j = 0; j < 9; j++) {
            if (strncmp(line + i, number_names[j], strlen(number_names[j])) !=
                0) {
                continue;
            }

            if (first_name_index == -1) {
                first_name_index = i;
                first_int = j + 1;
            }

            last_name_index = i;
            last_int = j + 1;
        }
    }

    if (first_digit_index > -1 &&
        (first_name_index == -1 || first_digit_index < first_name_index)) {
        first_int = line[first_digit_index] - '0';
    }

    if (last_digit_index > -1 &&
        (last_name_index == -1 || last_digit_index > last_name_index)) {
        last_int = line[last_digit_index] - '0';
    }

    // printf("%s %d\n", line, first_int * 10 + last_int);

    return first_int * 10 + last_int;
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-1.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    char *line = strtok(file_buffer, "\n");

    uint32_t total = 0;

    while (line != NULL) {
        total += get_line_int(line);
        line = strtok(NULL, "\n");
    }

    printf("%d\n", total);

    free(file_buffer);

    return 0;
}
