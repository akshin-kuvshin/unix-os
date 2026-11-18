// author: Danila "akshin_" Axyonov
// Linux specific!

// Compiler: gcc
// Output: main.out
// Flags: -lcrypto

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iso646.h>
#include <string.h>
#include <errno.h>
#include "./include/fatal_check.h"

#include <dirent.h>
#include <sys/stat.h>

#include "./include/FileInfo.h"
#include "./include/files.h"

void dfs(const char* const dirpath, size_t dirpath_len, FileInfoList_t* const unique_files);

int main(int argc, const char* const argv[]) {
    if (argc > 2) {
        fprintf(stderr, "[ERROR]: only 1 argument available - root directory;\n");
        fprintf(stderr, "       | given: %d arguments.\n", argc - 1);
        return EXIT_FAILURE;
    }

    char* const root_dirpath = realpath((argc == 2 ? argv[1] : "."), NULL); // malloc inside
    fatal_check(root_dirpath, "realpath");
    size_t root_dirpath_len = strlen(root_dirpath);

    struct stat root_dir_st;
    fatal_check(lstat(root_dirpath, &root_dir_st) == 0, "lstat(root_dir)");
    if (not S_ISDIR(root_dir_st.st_mode)) {
        fprintf(stderr, "[ERROR]: %s is not a directory.\n", root_dirpath);
        free(root_dirpath);
        return EXIT_FAILURE;
    }
    printf("[ROOT DIRECTORY]: %s\n", root_dirpath);

    FileInfoList_t* const unique_files = malloc(sizeof(FileInfoList_t));
    fatal_check(unique_files, "unique_files malloc");
    FileInfoList_init(unique_files);

    dfs(root_dirpath, root_dirpath_len, unique_files);

    FileInfoList_free(unique_files);
    free(root_dirpath);
    return EXIT_SUCCESS;
}

void dfs(const char* const dirpath, size_t dirpath_len, FileInfoList_t* const unique_files) {
    DIR* const dir = opendir(dirpath);
    fatal_check(dir != NULL, "opendir");

    struct dirent* entry;
    while (not (errno = 0) and (entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 or strcmp(entry->d_name, "..") == 0)
            continue;

        size_t entry_path_len = dirpath_len + 1 + strlen(entry->d_name);
        char* const entry_path = malloc(entry_path_len + 1);
        fatal_check(entry_path, "entry_path malloc");
        snprintf(entry_path, entry_path_len + 1, "%s/%s", dirpath, entry->d_name);
        bool free_entry_path = true;

        // printf("%s\n", entry_path);

        struct stat entry_st;
        fatal_check(lstat(entry_path, &entry_st) == 0, "lstat(entry)");

        if (S_ISREG(entry_st.st_mode)) {
            FileInfo_t file;
            file.path = entry_path;
            file.path_owner = true;
            calculate_sha256(entry_path, file.hash);

            FileInfoList_t* duplicates = FileInfoList_find_all_by_hash(unique_files, file.hash); // malloc inside
            bool was_equal = false;
            for (size_t i = 0; i < duplicates->size; ++i)
                if (files_equal(file.path, duplicates->data[i].path)) {
                    replace_with_hardlink(file.path, duplicates->data[i].path);
                    printf("replaced with hardlink: %s\n", entry_path);
                    was_equal = true;
                    break;
                }
            if (not was_equal) {
                FileInfoList_append(unique_files, file);
                free_entry_path = false;
            }
            FileInfoList_free(duplicates);
        } else if (S_ISDIR(entry_st.st_mode))
            dfs(entry_path, entry_path_len, unique_files);
        else
            printf("skipped: %s\n", entry_path);

        if (free_entry_path)
            free(entry_path);
    }
    fatal_check(errno == 0, "readdir");

    fatal_check(closedir(dir) == 0, "closedir");
}
