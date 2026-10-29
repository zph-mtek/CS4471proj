#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

// Google AI Overview for SHA-256 Implementation

#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32-byte digest

typedef struct {
    uint8_t data[64];                   // 512-bit buffered input block
    uint32_t datalen;                   // Loop counter for buffered data
    uint64_t bitlen;                    // Total message length in bits
    uint32_t state[8];                  // Current hash state (A, B, C, D, E, F, G, H)
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len);
void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);

#endif