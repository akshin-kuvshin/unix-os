// author: Danila "akshin_" Axyonov
// Linux specific!

#define _GNU_SOURCE

#include "./include/files.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iso646.h>
#include <string.h>
#include "./include/fatal_check.h"

#include <openssl/evp.h>

#include <unistd.h>
#include <libgen.h>

void calculate_sha256(const char* const filepath, unsigned char* const hash)  {
    FILE* const file = fopen(filepath, "rb");
    fatal_check(file != NULL, "fopen");

    EVP_MD_CTX* const context = EVP_MD_CTX_new();
    openssl_fatal_check(context != NULL);
    openssl_fatal_check(EVP_DigestInit_ex(context, EVP_sha256(), NULL) == 1);

    unsigned char buffer[BUFFER_SIZE];
    while (not feof(file)) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
        fatal_check(ferror(file) == 0, "fread");

        if (bytes_read > 0)
            openssl_fatal_check(EVP_DigestUpdate(context, buffer, bytes_read) == 1);
    }

    unsigned int hash_len;
    openssl_fatal_check(EVP_DigestFinal_ex(context, hash, &hash_len) == 1);
    fatal_check(hash_len == SHA256_LEN, "hash_len");

    EVP_MD_CTX_free(context);
    fatal_check(fclose(file) == 0, "fclose");
}

bool files_equal(const char* const filepath1, const char* const filepath2) {
    bool equal = true;

    FILE* const file1 = fopen(filepath1, "rb");
    fatal_check(file1 != NULL, "fopen1");
    FILE* const file2 = fopen(filepath2, "rb");
    fatal_check(file2 != NULL, "fopen2");

    unsigned char buffer1[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];
    while (equal and (not feof(file1) or not feof(file2))) {
        size_t bytes_read_1 = fread(buffer1, 1, BUFFER_SIZE, file1);
        size_t bytes_read_2 = fread(buffer2, 1, BUFFER_SIZE, file2);
        fatal_check(ferror(file1) == 0, "fread1");
        fatal_check(ferror(file2) == 0, "fread2");

        if (bytes_read_1 != bytes_read_2) {
            equal = false;
            break;
        }
        if (bytes_read_1 > 0)
            for (size_t i = 0; i < bytes_read_1; ++i)
                if (buffer1[i] != buffer2[i]) {
                    equal = false;
                    break;
                }
    }

    fatal_check(fclose(file1) == 0, "fclose1");
    fatal_check(fclose(file2) == 0, "fclose2");

    return equal;
}

void replace_with_hardlink(const char* const replacing_filepath, const char* const remaining_filepath) {
    char* const replacing_filepath_copy = strdup(replacing_filepath);
    fatal_check(replacing_filepath_copy != NULL, "strdup malloc");

    const char* const parent_dirpath = dirname(replacing_filepath_copy); // директория, в которой лежит replacing_filepath
    size_t parent_dirpath_len = strlen(parent_dirpath);

    size_t template_len = parent_dirpath_len + 1 + strlen(TEMP_DIRNAME_TEMPLATE);
    char* const template = malloc(template_len + 1);
    fatal_check(template != NULL, "template malloc");
    strcpy(template, parent_dirpath);
    template[parent_dirpath_len] = '/';
    strcpy(template + parent_dirpath_len + 1, TEMP_DIRNAME_TEMPLATE);

    const char* const temp_dirpath = mkdtemp(template);
    fatal_check(temp_dirpath != NULL, "mkdtemp");
    size_t temp_dirpath_len = strlen(temp_dirpath);

    size_t temp_filepath_len = temp_dirpath_len + 1 + strlen(TEMP_FILENAME);
    char* const temp_filepath = malloc(temp_filepath_len + 1);
    fatal_check(temp_filepath != NULL, "temp_filepath malloc");
    strcpy(temp_filepath, temp_dirpath);
    temp_filepath[temp_dirpath_len] = '/';
    strcpy(temp_filepath + temp_dirpath_len + 1, TEMP_FILENAME);

    fatal_check(link(remaining_filepath, temp_filepath) == 0, "link");
    fatal_check(rename(temp_filepath, replacing_filepath) == 0, "rename");

    fatal_check(rmdir(temp_dirpath) == 0, "rmdir");

    free(temp_filepath);
    free(template);
    free(replacing_filepath_copy);
}
