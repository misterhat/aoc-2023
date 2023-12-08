#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PART_TWO false

#define MAX_HANDS_LENGTH 1024

#define HAND_LENGTH 5

#if (PART_TWO == true)
const char CARD_LABELS[] = {'A', 'K', 'Q', 'T', '9', '8', '7',
                            '6', '5', '4', '3', '2', 'J'};
#else
const char CARD_LABELS[] = {'A', 'K', 'Q', 'J', 'T', '9', '8',
                            '7', '6', '5', '4', '3', '2'};
#endif

#define CARDS_LENGTH (sizeof(CARD_LABELS) / sizeof(char))

typedef enum {
    /* where all cards' labels are distinct: 23456 */
    HIGH_CARD = 1,

    /* where two cards share one label, and the other three cards have a
       different label from the pair and each other: A23A4 */
    ONE_PAIR,

    /* where two cards share one label, two other cards share a second label,
       and the remaining card has a third label: 23432 */
    TWO_PAIR,

    /* where three cards have the same label, and the remaining two cards are
       each different from any other card in the hand: TTT98 */
    THREE_OF_A_KIND,

    /* where three cards have the same label, and the remaining two cards share
       a different label: 23332 */
    FULL_HOUSE,

    /* where four cards have the same label and one card has a different label:
       AA8AA */
    FOUR_OF_A_KIND,

    /* where all five cards have the same label: AAAAA */
    FIVE_OF_A_KIND
} HandType;

typedef struct {
    char hand[HAND_LENGTH];
    HandType type;
    uint32_t bid;
} Hand;

HandType hand_to_type(const char hand[HAND_LENGTH]) {
    uint32_t card_occurrences[CARDS_LENGTH] = {0};

    for (size_t i = 0; i < CARDS_LENGTH; i++) {
        for (size_t j = 0; j < HAND_LENGTH; j++) {
            if (CARD_LABELS[i] == hand[j]) {
                card_occurrences[i]++;
            }
        }
    }

    HandType type = 0;

    for (size_t i = 0; i < CARDS_LENGTH; i++) {
        // printf("%c=%d\n", CARD_LABELS[i], card_occurrences[i]);

        if (type < FIVE_OF_A_KIND && card_occurrences[i] == 5) {
            type = FIVE_OF_A_KIND;
        }

        if (type < FOUR_OF_A_KIND && card_occurrences[i] == 4) {
            type = FOUR_OF_A_KIND;
        }

        if (card_occurrences[i] == 3) {
            for (size_t j = 0; j < CARDS_LENGTH; j++) {
                if (type < FULL_HOUSE && card_occurrences[j] == 2) {
                    type = FULL_HOUSE;
                }
            }

            if (type < THREE_OF_A_KIND) {
                type = THREE_OF_A_KIND;
            }
        }

        if (card_occurrences[i] == 2) {
            for (size_t j = 0; j < CARDS_LENGTH; j++) {
                if (type < TWO_PAIR && j != i && card_occurrences[j] == 2) {
                    type = TWO_PAIR;
                }
            }

            if (type < ONE_PAIR) {
                type = ONE_PAIR;
            }
        }
    }

    if (type < HIGH_CARD) {
        return HIGH_CARD;
    }

    return type;
}

size_t index_of_label(char label) {
    for (size_t i = 0; i < CARDS_LENGTH; i++) {
        if (CARD_LABELS[i] == label) {
            return i;
        }
    }

    // should not happen
    return 0;
}

int hand_compare(const void *a, const void *b) {
    Hand *hand_a = (Hand *)a;
    Hand *hand_b = (Hand *)b;

    if (hand_b->type < hand_a->type) {
        return 1;
    } else if (hand_a->type < hand_b->type) {
        return -1;
    } else {
        for (size_t i = 0; i < HAND_LENGTH; i++) {
            size_t a_index = index_of_label(hand_a->hand[i]);
            size_t b_index = index_of_label(hand_b->hand[i]);

            if (b_index < a_index) {
                return -1;
            } else if (a_index < b_index) {
                return 1;
            }
        }
    }

    return 0;
}

/* find the best hand by brute replacing the J at joker_index */
void get_jokerfied_hand(char hand[HAND_LENGTH], int joker_index,
                        HandType *highest_type) {
    /* amount of jokers */
    size_t joker_length = 0;

    int hand_index = 0;

    for (size_t i = 0; i < HAND_LENGTH; i++) {
        if (hand[i] == 'J') {
            hand_index = i;
            joker_length++;
        }
    }

    if (joker_length == 0) {
        return;
    }

    for (int i = 0; i < CARDS_LENGTH; i++) {
        if (CARD_LABELS[i] == 'J') {
            continue;
        }

        char new_hand[HAND_LENGTH] = {0};
        memcpy(new_hand, hand, 5);

        new_hand[hand_index] = CARD_LABELS[i];

        if (joker_length == 1) {
            HandType type = hand_to_type(new_hand);

            if (type > *highest_type) {
                *highest_type = type;
            }

            if (type == FIVE_OF_A_KIND) {
                return;
            }
        }

        get_jokerfied_hand(new_hand, joker_index + 1, highest_type);
    }
}

int main(int argc, char **argv) {
    FILE *input_file = fopen("./day-7.txt", "r");
    fseek(input_file, 0L, SEEK_END);

    /* ignore last newline */
    size_t input_size = ftell(input_file) - 1;

    rewind(input_file);

    char *file_buffer = calloc(input_size, sizeof(char));
    fread(file_buffer, sizeof(char), input_size, input_file);

    fclose(input_file);

    char *line = strtok(file_buffer, "\n");

    Hand hands[MAX_HANDS_LENGTH] = {0};
    size_t hands_length = 0;

    while (line) {
        Hand *hand = &hands[hands_length];

        memcpy(hand->hand, line, 5);

        if (PART_TWO) {
            HandType highest_type = 0;
            get_jokerfied_hand(hand->hand, 0, &highest_type);

            if (highest_type > 0) {
                hand->type = highest_type;
            } else {
                hand->type = hand_to_type(hand->hand);
            }
        } else {
            hand->type = hand_to_type(hand->hand);
        }

        hand->bid = atoi(line + 6);

        hands_length++;

        line = strtok(NULL, "\n");
    }

    free(file_buffer);

    qsort(hands, hands_length, sizeof(Hand), hand_compare);

    uint32_t total_winnings = 0;

    for (int i = 0; i < hands_length; i++) {
        printf("%.5s %d\n", hands[i].hand, hands[i].type);
        total_winnings += (i + 1) * hands[i].bid;
    }

    /*HandType highest_type = 0;
    get_jokerfied_hand("T55J5", 0, &highest_type);
    printf("%d\n", highest_type);*/

    printf("%u\n", total_winnings);

    return 0;
}
