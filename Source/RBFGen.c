#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

// Creating the Bloom Filter


// Helper, run SHA256 and return its lowest 20 bits
int hash20(const char *text)
{
    SHA256_CTX ctx;
    uint8_t hash[32];

    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t *)text, strlen(text));
    sha256_final(&ctx, hash);

    // Return exactly the lowest 20 bits of the hash as an integer
    return ((int)(hash[29] & 15) << 16)
         | ((int)hash[30] << 8)
         | hash[31];
}


// Helper, takes a key and an input and will return the hashed value mod m
int hashKey(int key, char* input, int mod)
{
    char string[32];

    sprintf(string, "%d%s", key, input);

    return hash20(string) % mod;
}


int main(int argc, char *argv[])
{
    // Read filter size, and output file
    if (argc != 3) {
        printf("Usage: %s size output.txt\n", argv[0]);
        return 1;
    }

    int m = atoi(argv[1]);

    if (m <= 0) {
        printf("Filter size must be positive\n");
        return 1;
    }

    int *rbf = calloc(m, sizeof(int));

    //initializing the rbf
    for(int j = 0; j < m; j++) {
        char input[32];
        sprintf(input, "%d", j);
        rbf[j] = 1 - hashKey(0, input, 2);
    }

    //Generates corresponding rbf with embedded IP addresses
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 256; j++) {
            //creating ip for the hash
            char ip[14];
            sprintf(ip, "192.168.%d.%d", i, j);
            for(int l = 1; l < 9; l++) {
                //flipping bit of the column found with hash key
                int column = hashKey(l, ip, m);
                char col[32];
                sprintf(col, "%d", column);
                rbf[column] = hashKey(0, col, 2);
            }
        }
    }

    char buffer[100];

    sprintf(buffer, "../Results/%s", argv[2]);

    //opening output file
    FILE *output = fopen(buffer, "w");
    
    if (output == NULL) {
        printf("Could not open file");
        return 1;
    }

    //write to the output file
    for(int i = 0; i < m; i++) {
        fprintf(output, "%d", rbf[i]);
    }

    fclose(output);

    return 0;
}