#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    
    FILE *seed = fopen(argv[0], "rb");
    
    if (seed == NULL) {
        perror("Could not open file");
        return 1;
    }

    printf("File opened successfully.\n");

    fclose(seed);

    return 0;
}