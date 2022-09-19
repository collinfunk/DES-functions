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
    // 1. 56 bit key goes through the inverse of PC2 to produce a 56 bit key.
    uint8_t key[56];
    for (int i = 0; i < 56; i++) {
        // PC2 inverse
        key[i] = original_key[PC2[i] - 1];
    }

    // 2. 56 bit key is split into two 28 bit halves.
    uint8_t left[28];
    uint8_t right[28];
    for (int i = 0; i < 28; i++) {
        left[i] = key[i];
        // right half = top indeces of key
        right[i] = key[i + 28];
    }

    // 3. Each 28 bit half is rotated right by a different amount.
    // 4. The two 28 bit halves are concatenated to form a 56 bit key.
    uint8_t rotated_left[28];
    uint8_t rotated_right[28];
    for (int i = 0; i < 16; i++) {
        // rotate left
        for (int j = 0; j < 28; j++) {
            // if key rotation is 1, rotate by 1
            // if key rotation is 2, rotate by 2
            rotated_left[j] = left[(j + KEY_ROTATIONS[i]) % 28];
            rotated_right[j] = right[(j + KEY_ROTATIONS[i]) % 28];
        }

        // 5. The 56 bit key goes through the inverse of PC1 to produce a 56 bit key.
        uint8_t subkey[56];
        for (int j = 0; j < 28; j++) {
            subkey[j] = rotated_left[j];
            subkey[j + 28] = rotated_right[j];
        }

        uint8_t subkey_48[48];
        for (int j = 0; j < 48; j++) {
            subkey_48[j] = subkey[PC1[j] - 1];
        }

        // copy subkey_48 into sub_keys
        for (int j = 0; j < 48; j++) {
            sub_keys[i][j] = subkey_48[j];
        }
    }
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
    return 0;
}


