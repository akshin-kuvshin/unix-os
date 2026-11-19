// author: Danila "akshin_" Axyonov
// Linux specific!

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <iso646.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#define LOCK_MODE (mode_t)0666
#define FILE_MODE (mode_t)0644

#define UUID_LEN (size_t)36
#define PATH_MAX (size_t)4096
#define CONTENT_MAX (UUID_LEN + 1 + 10 + 1) // uuid + '\n' + file_n (possibly > 999) + '\0'

typedef unsigned long long int ulli;

const struct timespec second = { .tv_sec = 1, .tv_nsec = 0 };

void fatal_check(bool good, const char* const context);

int main(void) {
    char uuid[UUID_LEN + 1];
    FILE* uuid_file = fopen("/proc/sys/kernel/random/uuid", "r");
    fatal_check(uuid_file != NULL, "fopen");
    fatal_check(fgets(uuid, UUID_LEN + 1, uuid_file) != NULL, "fgets");
    fatal_check(strlen(uuid) == UUID_LEN, "invalid UUID");
    fatal_check(fclose(uuid_file) == 0, "fclose");

    const char* const shared_dirpath = "/shared";
    const char* const lock_filepath = "/shared/.lock";
    int lock_fd = open(lock_filepath, O_CREAT | O_RDWR, LOCK_MODE);
    fatal_check(lock_fd != -1, "lock_fd open");

    char filepath[PATH_MAX];
    ulli files_created = 0;
    while (true) {
        while (flock(lock_fd, LOCK_EX) == -1)
            fatal_check(errno == EINTR, "flock(LOCK_EX)");

        ulli file_n = 1;
        int file_fd;
        do {
            snprintf(filepath, PATH_MAX, "%s/%03llu", shared_dirpath, file_n);
            file_fd = open(filepath, O_CREAT | O_EXCL | O_WRONLY, FILE_MODE);
            if (file_fd == -1) {
                fatal_check(errno == EEXIST or errno == EINTR, "file_fd open");
                file_n += errno == EEXIST;
            }
        } while (file_fd == -1);

        fatal_check(flock(lock_fd, LOCK_UN) == 0, "flock(LOCK_UN)");



        char content[CONTENT_MAX];
        snprintf(content, CONTENT_MAX, "%s\n%llu", uuid, ++files_created);
        const char* content_ = content;
        size_t rem_len = strlen(content_);
        while (rem_len > 0) {
            ssize_t written = write(file_fd, content_, rem_len);
            if (written == -1) {
                fatal_check(errno == EINTR, "write");
                continue;
            }
            fatal_check(written > 0, "written == 0");
            content_ += written;
            rem_len -= written;
        }

        fatal_check(close(file_fd) == 0, "close(file_fd)");
        printf("%s created %03llu (%llu in total)\n", uuid, file_n, files_created);
        fflush(stdout);



        struct timespec req_time = second,
                        rem_time;
        while (nanosleep(&req_time, &rem_time) == -1) {
            fatal_check(errno == EINTR, "nanosleep");
            req_time = rem_time;
        }

        fatal_check(unlink(filepath) == 0, "unlink");
        printf("%s deleted %03llu (%llu in total)\n", uuid, file_n, files_created);
        fflush(stdout);
    }

    // unreachable, btw
    fatal_check(close(lock_fd) == 0, "close(lock_fd)");
    return EXIT_SUCCESS;
}

void fatal_check(bool good, const char* const context) {
    if (not good) {
        perror(context);
        exit(EXIT_FAILURE);
    }
}
