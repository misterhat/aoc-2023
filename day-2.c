#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_TWO true

/* maximum possible cube counts corresponding with colours */
uint32_t colour_counts[] = {12, 13, 14};
char *colour_names[] = {"red", "green", "blue"};

#define TOTAL_COLOURS (sizeof(colour_names) / sizeof(char *))

/* determine if a line has cubes that are possible given our defined
 * constants */
bool is_roll_possible(char *bag_roll, uint32_t highest_colours[TOTAL_COLOURS]) {
    bool roll_possible = true;

    char *comma_save = bag_roll;

    /* "3 blue", "4 red", etc. */
    char *cube_section = strtok_r(comma_save, ",", &comma_save);

    while (cube_section != NULL) {
        /* skip space */
        cube_section++;

        int space_position = (int)(strchr(cube_section, ' ') - cube_section);

        char cube_count_str[space_position + 1];
        strncpy(cube_count_str, cube_section, space_position);
        cube_count_str[space_position] = '\0';

        uint32_t cube_count = atoi(cube_count_str);

        char *cube_colour = cube_section + space_position + 1;

        uint32_t colours_possible = 0;

        for (size_t i = 0; i < TOTAL_COLOURS; i++) {
            if (strcmp(cube_colour, colour_names[i]) == 0) {
                if (cube_count > colour_counts[i]) {
                    roll_possible = false;
                }

                if (cube_count > highest_colours[i]) {
                    highest_colours[i] = cube_count;
                }

                break;
            }
        }

        cube_section = strtok_r(comma_save, ",", &comma_save);
    }

    return roll_possible;
}

bool is_line_possible(char *line, uint32_t *power) {
    /* skip "Game x: " */
    while (line[0] != ':') {
        line++;
    }

    line++;

    /* the highest rolls for each colour on this line */
    uint32_t highest_colours[TOTAL_COLOURS] = {0};

    bool line_possible = true;

    char *semicolon_save = line;
    char *bag_roll = strtok_r(line, ";", &semicolon_save);

    while (bag_roll != NULL) {
        if (!is_roll_possible(bag_roll, highest_colours)) {
            line_possible = false;
        }

        bag_roll = strtok_r(semicolon_save, ";", &semicolon_save);
    }

    for (int i = 0; i < TOTAL_COLOURS; i++) {
        *power *= highest_colours[i];
    }

    return line_possible;
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-2.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    /* sum of game IDs that are possible */
    uint32_t game_id_sum = 0;

    uint32_t game_id = 1;

    /* sum of fewest amounts of cubes needed for each line, multipled
     * together */
    uint32_t cube_power_sum = 0;

    char *line = strtok(file_buffer, "\n");

    while (line != NULL) {
        uint32_t power = 1;

        if (is_line_possible(line, &power)) {
            // printf("game %d possible\n", game_id);
            game_id_sum += game_id;
        }

        cube_power_sum += power;
        game_id++;

        line = strtok(NULL, "\n");
    }

    printf("%d\n", PART_TWO ? cube_power_sum : game_id_sum);

    free(file_buffer);
}
