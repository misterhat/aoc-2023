#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_TWO true

#define MAX_NODES 1024

/* name = (left, right) */
typedef struct {
    char name[3];
    char left[3];
    char right[3];
} Node;

uint64_t gcd(uint64_t a, uint64_t b) {
    if (b == 0) {
        return a;
    }

    return gcd(b, a % b);
}

size_t find_next_index(Node nodes[MAX_NODES], size_t nodes_length, Node node,
                       char instruction) {
    char *compare_with = instruction == 'R' ? node.right : node.left;

    for (size_t i = 0; i < nodes_length; i++) {
        if (strncmp(nodes[i].name, compare_with, 3) == 0) {
            return i;
        }
    }

    // shouldn't happen
    return 0;
}

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

    uint64_t steps = 0;

    if (PART_TWO) {
        size_t starting_node_indices[MAX_NODES / 2] = {0};
        size_t starting_nodes_length = 0;

        for (size_t i = 0; i < nodes_length; i++) {
            if (nodes[i].name[2] == 'A') {
                starting_node_indices[starting_nodes_length] = i;
                starting_nodes_length++;
            }
        }

        size_t instruction_index = 0;

        size_t finished_node_steps[MAX_NODES / 2] = {0};

        while (1) {
            steps++;

            /* re-set */
            if (instruction_index == instructions_length) {
                instruction_index = 0;
            }

            char instruction = instructions[instruction_index];

            // printf("step %c %d\n", instruction, steps);

            for (size_t i = 0; i < starting_nodes_length; i++) {
                Node node = nodes[starting_node_indices[i]];

                if (node.name[2] == 'Z') {
                    continue;
                }

                size_t next_index =
                    find_next_index(nodes, nodes_length, node, instruction);

                Node next_node = nodes[next_index];

                starting_node_indices[i] = next_index;

                // printf("%.3s\n", next_node.name);

                if (next_node.name[2] == 'Z') {
                    if (finished_node_steps[i] == 0) {
                        finished_node_steps[i] = steps;
                    }
                }
            }

            bool done = true;

            for (int i = 0; i < starting_nodes_length; i++) {
                if (finished_node_steps[i] == 0) {
                    done = false;
                }
            }

            if (done) {
                break;
            }

            instruction_index++;

            // printf("\n");
        }

        /*for (int i = 0; i < starting_nodes_length; i++) {
            printf("steps: %lu\n", finished_node_steps[i]);
        }*/

        uint64_t lcm = finished_node_steps[0];

        for (size_t i = 1; i < starting_nodes_length; i++) {
            uint64_t finished_steps = (uint64_t)finished_node_steps[i];
            lcm = (((finished_steps * lcm)) / (gcd(finished_steps, lcm)));
        }

        steps = lcm;
    } else {
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
    }

    printf("%lu\n", steps);
}
