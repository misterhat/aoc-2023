#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_TWO true

#define MAX_SECTION_LENGTH 128
#define MAX_CARDS 255

/* turn a string of numbers into an int array ("1 23 4 56" -> [1, 23, 4, 56]) */
void parse_number_section(char *section, uint32_t *numbers, size_t *length) {
    *length = 0;

    int digits_length = 0;
    size_t section_length = strlen(section);

    for (int i = 0; i < section_length; i++) {
        bool parse_digits = false;

        if (isdigit(section[i])) {
            digits_length++;
        } else {
            parse_digits = true;
        }

        if (i == section_length - 1) {
            parse_digits = true;
            i++;
        }

        if (parse_digits && digits_length > 0) {
            char digit_str[digits_length + 1];
            memcpy(digit_str, section + i - digits_length, digits_length);
            digit_str[digits_length] = '\0';

            // printf("digit |%s| %d\n", digit_str, digits_length);

            numbers[*length] = atoi(digit_str);
            (*length)++;

            digits_length = 0;
        }
    }
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-4.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    uint32_t our_numbers[MAX_SECTION_LENGTH];
    uint32_t winning_numbers[MAX_SECTION_LENGTH];

    size_t our_numbers_length = 0;
    size_t winning_numbers_length = 0;

    uint32_t points_sum = 0;

    size_t card_id = 0;
    uint32_t card_wins[MAX_CARDS] = {0};

    char *line = strtok(file_buffer, "\n");

    while (line) {
        line = strstr(line, ":") + 1;

        char *section = line;
        char *save_section = section;

        /* parse our numbers */
        section = strtok_r(save_section, "|", &save_section) + 1;
        section[strlen(section) - 1] = '\0'; /* space before | */
        parse_number_section(section, our_numbers, &our_numbers_length);

        /* parse winning numbers */
        section = strtok_r(save_section, "|", &save_section) + 1;
        parse_number_section(section, winning_numbers, &winning_numbers_length);

        uint32_t points = 0;
        uint32_t matching_numbers = 0;

        for (int i = 0; i < our_numbers_length; i++) {
            for (int j = 0; j < winning_numbers_length; j++) {
                if (our_numbers[i] == winning_numbers[j]) {
                    points = points == 0 ? 1 : points * 2;
                    matching_numbers++;

                    break;
                }
            }
        }

        // printf("card %ld matching %d\n", card_id, matching_numbers);

        uint32_t original_wins = card_wins[card_id];

        for (int i = 0; i < matching_numbers; i++) {
            card_wins[card_id + 1 + i] += original_wins + 1;
        }

        /*printf("[");

        for (int i = 0; i < 6; i++) {
            printf("%d, ", card_wins[i]);
        }

        printf("]\n");*/

        points_sum += points;
        card_id++;

        line = strtok(NULL, "\n");
    }

    if (PART_TWO) {
        uint32_t card_sum = 0;

        for (int i = 0; i < card_id; i++) {
            card_sum += card_wins[i] + 1;
        }

        printf("%d\n", card_sum);
    } else {
        printf("%d\n", points_sum);
    }

    return 0;
}
