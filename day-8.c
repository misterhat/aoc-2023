#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1024

/* name = (left, right) */
typedef struct {
    char name[3];
    char left[3];
    char right[3];
} Node;

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-8.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    /* parse instructions on first line */
    char *line = strtok(file_buffer, "\n");

    char instructions[strlen(line) + 1];
    strcpy(instructions, line);

    size_t instructions_length = strlen(instructions);

    /* ignore empty line */
    line = strtok(NULL, "\n");

    /* parse each of the nodes into a struct */
    Node nodes[MAX_NODES] = {0};
    size_t nodes_length = 0;

    while (line) {
        Node *node = &nodes[nodes_length];

        strncpy(node->name, line, 3);
        strncpy(node->left, line + 7, 3);
        strncpy(node->right, line + 12, 3);

        nodes_length++;

        line = strtok(NULL, "\n");
    }

    free(file_buffer);

    uint32_t steps = 0;
    size_t instruction_index = 0;
    size_t node_index = 0;

    /* Starting at AAA, follow the left/right instructions */
    for (size_t i = 0; i < nodes_length; i++) {
        if (strncmp(nodes[i].name, "AAA", 3) == 0) {
            node_index = i;
            break;
        }
    }

    while (1) {
        /* re-set */
        if (instruction_index == instructions_length) {
            instruction_index = 0;
        }

        Node node = nodes[node_index];
        char instruction = instructions[instruction_index];

        /*printf("currently on %.3s with instruction %c\n", node.name,
               instruction);*/

        if (strncmp(node.name, "ZZZ", 3) == 0) {
            break;
        }

        char *compare_with = instruction == 'R' ? node.right : node.left;

        for (size_t i = 0; i < nodes_length; i++) {
            if (strncmp(nodes[i].name, compare_with, 3) == 0) {
                node_index = i;
                break;
            }
        }

        instruction_index++;
        steps++;
    }

    printf("%u\n", steps);
}
