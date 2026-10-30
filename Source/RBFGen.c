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

// Helper, will convert the input inot a string and return the hashed value mod 2
int hashColumn(const int j)
{
    // SHA256 is 32 bytes allocated
    char column[31];
    char input[32];

    sprintf(column, "%d", j);
    sprintf(input, "0%s", column);

    return hash20(input) % 2;
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
        rbf[j] = 1 - hashColumn(j);
    }

    // Generate the first row of the RBF
    char ip[14];

    // Insert RBF Stuff

    // Generate IP strings



    return 0;
}