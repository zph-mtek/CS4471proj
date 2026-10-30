#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

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

// Takes a key and returns the hashed value mod m
static int hashKey(int key, const char *input, int mod)
{
    char string[32];

    snprintf(string, sizeof(string), "%d%s", key, input);
    return hash20(string) % mod;
}

// Checking an IP against the Bloom Filter
int main(int argc, char *argv[])
{
    // Read filter filename and IP address
    if (argc != 3) {
        printf("Usage: %s filter.txt ip\n", argv[0]);
        return 1;
    }

    // Open the filter file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Could not open filter");
        return 1;
    }

    // Determine the size of the filter file
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Read the filter into memory
    char *filter = malloc((size_t)fileSize);
    if (filter == NULL || fread(filter, 1, (size_t)fileSize, file) != (size_t)fileSize) {
        fprintf(stderr, "Could not read filter\n");
        free(filter);
        fclose(file);
        return 1;
    }
    fclose(file);

    // The size of the filter is the number of bytes read
    int filterSize = (int)fileSize;

    // Check the IP against the Bloom Filter
    int possiblyMalicious = 1;
    for (int key = 1; key <= 8; key++) {
        int column = hashKey(key, argv[2], filterSize);
        char columnText[32];
        snprintf(columnText, sizeof(columnText), "%d", column);
        int expected = hashKey(0, columnText, 2);
        if (filter[column] != '0' + expected) {
            possiblyMalicious = 0;
            break;
        }
    }

    // Output the result
    if (possiblyMalicious) {
        printf("Block\n");
    } else {
        printf("Pass\n");
    }

    free(filter);
    return 0;
}