#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PART_TWO true

/* maximum amount of initial seeds in "seeds:" line */
#define MAX_SEED_LENGTH 32

/* maximum amount of x-to-y sections (7 by default) */
#define MAX_SECTIONS 32

/* seed coarseness */
#define SKIP_LENGTH 100000

/* turn a string of numbers into an int array ("1 23 4 56" -> [1, 23, 4, 56]) */
size_t parse_number_section(char *section, uint64_t *numbers) {
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

            numbers[length] = strtoul(digit_str, NULL, 0);
            length++;

            digits_length = 0;
        }
    }

    return length;
}

/* transform a seed line string of three numbers from a section */
uint64_t transform_seed_line(uint64_t seed, char *line) {
    uint64_t parsed_transform[3] = {0};
    parse_number_section(line, parsed_transform);

    uint64_t destination = parsed_transform[0];
    uint64_t source = parsed_transform[1];
    uint64_t range_length = parsed_transform[2];

    if (seed < source || seed >= source + range_length) {
        return seed;
    }

    return destination + (seed - source);
}

/* transform a seed at any given state to another utilizing section: a string
 * of destination, source, range separated by newlines */
uint64_t transform_seed_section(uint64_t seed, char *section) {
    char *section_copy = alloca(strlen(section) + 1);
    strcpy(section_copy, section);

    while (1) {
        char *next_line = strchr(section_copy, '\n');
        char *line = NULL;

        if (next_line == NULL) {
            line = section_copy;
        }

        if (!line) {
            section_copy[next_line - section_copy] = '\0';
            line = section_copy;
        }

        uint64_t transformed = transform_seed_line(seed, line);

        if (transformed != seed) {
            return transformed;
        }

        if (next_line == NULL) {
            break;
        }

        section_copy += strlen(section_copy) + 1;
    }

    return seed;
}

/* run the seed through all the section transformations and return location */
uint64_t transform_seed_sections(uint64_t seed, char *sections[],
                                 size_t sections_length) {
    for (int i = 0; i < sections_length; i++) {
        seed = transform_seed_section(seed, sections[i]);
    }

    return seed;
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-5.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    /* parse initial seeds in the first line */
    file_buffer[strchr(file_buffer, '\n') - file_buffer] = '\0';

    uint64_t seeds[MAX_SEED_LENGTH] = {0};

    size_t seeds_length =
        parse_number_section(strstr(file_buffer, ":") + 2, seeds);

    /* move pointer past our clobbered null pointer to reveal rest of the file
     */
    file_buffer += strlen(file_buffer) + 1;

    uint64_t lowest_location = 0;

    /* the lowest seed in the iteration that got us our lowest location */
    uint64_t lowest_seed_min = 0;

    /* the seed that got us our current lowest location */
    uint64_t lowest_seed = 0;

    char *sections[MAX_SECTIONS] = {0};
    size_t sections_length = 0;

    while (1) {
        char *next_section = strstr(file_buffer, "\n\n");
        char *section = NULL;

        if (next_section == NULL) {
            section = file_buffer;
        }

        if (!section) {
            file_buffer[next_section - file_buffer] = '\0';
            section = file_buffer;
        }

        sections[sections_length] = strstr(section, ":") + 2;
        sections_length++;

        /* the last section will be location */
        if (next_section == NULL) {
            break;
        }

        file_buffer += strlen(section) + 1;
    }

    if (PART_TWO) {
        for (size_t i = 0; i < seeds_length; i += 2) {
            uint64_t skip_length = SKIP_LENGTH > seeds[i + 1] ? 1 : SKIP_LENGTH;

            /* get a coarse answer for this seed */
            for (uint64_t j = 0; j < seeds[i + 1]; j += skip_length) {
                uint64_t seed = seeds[i] + j;

                uint64_t location =
                    transform_seed_sections(seed, sections, sections_length);

                if (lowest_location == 0 || location < lowest_location) {
                    lowest_seed_min = seeds[i];

                    lowest_seed = seed;
                    lowest_location = location;

                    /*printf("lowest seed boundaries min=%lu, start_seed=%lu, "
                           "location=%lu\n",
                           lowest_seed_min, seed, location);*/
                }
            }
        }

        uint64_t last_location = 0;

        /* decrement by 1 until it stops getting smaller */
        while (1) {
            last_location =
                transform_seed_sections(lowest_seed, sections, sections_length);

            if (last_location <= lowest_location) {
                lowest_location = last_location;

                if (lowest_seed > lowest_seed_min) {
                    lowest_seed--;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    } else {
        for (size_t i = 0; i < seeds_length; i++) {
            uint64_t location =
                transform_seed_sections(seeds[i], sections, sections_length);

            if (lowest_location == 0 || location < lowest_location) {
                lowest_location = location;
            }
        }
    }

    printf("%lu\n", lowest_location);

    return 0;
}
