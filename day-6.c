#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* maximum amount of time/distance pairs */
#define MAX_RACES 32

/* turn a string of numbers into an int array ("1 23 4 56" -> [1, 23, 4, 56]) */
size_t parse_number_section(char *section, uint32_t *numbers) {
    size_t length = 0;

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

            numbers[length] = atoi(digit_str);
            length++;

            digits_length = 0;
        }
    }

    return length;
}

uint32_t race_boat(uint32_t time, uint32_t record_distance) {
    /* amount of times a button-holding time beats record_distance */
    uint32_t holding_wins = 0;

    for (int i = 1; i < time - 1; i++) {
        uint32_t distance = i * (time - i);

        holding_wins += distance > record_distance;
        // printf("%u %u\n", i, distance);
    }

    return holding_wins;
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-6.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    /* parse times in milliseconds each race took */
    uint32_t times[MAX_RACES] = {0};
    char *line = strtok(file_buffer, "\n");
    size_t races_length = parse_number_section(line, times);

    /* parse distance each race travelled */
    uint32_t distances[MAX_RACES] = {0};

    line = strtok(NULL, "\n");

    /* should be same length as time */
    parse_number_section(line, distances);

    uint32_t product = 1;

    for (size_t i = 0; i < races_length; i++) {
        product *= race_boat(times[i], distances[i]);
    }

    printf("%u\n", product);
}
