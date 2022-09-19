#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

#include "des.h"

uint8_t key[56] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55
};

uint8_t subkeys[16][48];

uint8_t KEY_ROTATIONS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 1
};

void get_sub_keys(uint8_t* original_key, uint8_t sub_keys[16][48]) {
    // Assume that we have a 56 bit key already and don't need to compress it from 64 bits

    //split the key into two halves
    uint8_t left[28];
    uint8_t right[28];
    for (int i = 0; i < 28; i++) {
        left[i] = original_key[i];
        right[i] = original_key[i + 28];
    }

    //rotate the halves the right based off of the KEY_ROTATIONS array
    for (int i = 0; i < 16; i++) {
        // temp arrays
        uint8_t left_temp = left[0];
        uint8_t right_temp = right[0];

        // roate halves
        for (int j = 0; j < 27; j++) {
            left[j] = left[j + 1];
            right[j] = right[j + 1];
        }

        // set arrays
        left[27] = left_temp;
        right[27] = right_temp;

        // if the rotation is 2, rotate again
        if (KEY_ROTATIONS[i] == 2) {
            left_temp = left[0];
            right_temp = right[0];
            for (int j = 0; j < 27; j++) {
                left[j] = left[j + 1];
                right[j] = right[j + 1];
            }

            // set arrays
            left[27] = left_temp;
            right[27] = right_temp;
        }

        //combine the halves
        uint8_t combined[56];
        for (int j = 0; j < 28; j++) {
            combined[j] = left[j];
            combined[j + 28] = right[j];
        }

        //compress the combined halves
        for (int j = 0; j < 48; j++) {
            sub_keys[i][j] = combined[PC2[j]];
        }
    }
    
    // finished
}

int main (int argc, char** argv) {
    get_sub_keys(key, subkeys);
    
    for (int i = 0; i < 16; i++) {
        printf("K%d: ", i + 1);
        for (int j = 0; j < 48; j++) {
            // if not last index
            if (j != 47) {
                printf("k%d, ", subkeys[i][j]);
            } 
            else {
                printf("k%d", subkeys[i][j]);
            }
        }
        printf("\n");
    }

    // count the amount of times each bit is used
    uint8_t bit_count[56];
    for (int i = 0; i < 56; i++) {
        bit_count[i] = 0;
    }

    // for subkey
    for (int i = 0; i < 16; i++) {
        // for bit
        for (int j = 0; j < 48; j++) {
            // incremet bitcounter by one for that bit
            bit_count[subkeys[i][j]]++;
        }
    }

    for (int i = 0; i < 56; i++) {
        if (i == 55) {
            printf("k%d: %d", i, bit_count[i]);
        }
        else if (i % 7 == 0 && i != 0) {
            printf("k%d: %d\n", i, bit_count[i]);
        }
        else {
            printf("k%d: %d, ", i, bit_count[i]);
        }
        
    }

    printf("\n");
    return 0;
}


