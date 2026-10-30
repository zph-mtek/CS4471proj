#include <stdio.h>
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

// Checking an IP against the Bloom Filter
int main(int argc, char *argv[])
{
    // Read filter filename and IP address
    if (argc != 3) {
        printf("Usage: %s filter.txt ip\n", argv[0]);
        return 1;
    }
    



    return 0;
}