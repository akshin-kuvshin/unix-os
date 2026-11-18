// author: Danila "akshin_" Axyonov

#include "./include/FileInfo.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "./include/fatal_check.h"

void FileInfoList_init(FileInfoList_t* const fileInfoList) {
    fileInfoList->data = NULL;
    fileInfoList->size = fileInfoList->capacity = 0;
}

void FileInfoList_append(FileInfoList_t* const fileInfoList, FileInfo_t fileInfo) {
    if (fileInfoList->size == fileInfoList->capacity) {
        size_t new_capacity = fileInfoList->capacity > 0
                            ? (fileInfoList->capacity << 1)
                            : 1;
        FileInfo_t* new_data = realloc(
            fileInfoList->data,
            new_capacity * sizeof(FileInfo_t)
        );
        fatal_check(new_data != NULL, "FileInfoList_append malloc");

        fileInfoList->data = new_data;
        fileInfoList->capacity = new_capacity;
    }

    fileInfoList->data[fileInfoList->size++] = fileInfo;
}

FileInfoList_t* FileInfoList_find_all_by_hash(FileInfoList_t* const fileInfoList, const unsigned char* const hash) {
    FileInfoList_t* found = malloc(sizeof(FileInfoList_t));
    fatal_check(found != NULL, "found malloc");
    FileInfoList_init(found);

    for (size_t i = 0; i < fileInfoList->size; ++i)
        if (memcmp(fileInfoList->data[i].hash, hash, SHA256_LEN) == 0) {
            FileInfo_t fileInfo = fileInfoList->data[i];
            fileInfo.path_owner = false;
            FileInfoList_append(found, fileInfo);
        }

    return found;
}

void FileInfoList_free(FileInfoList_t* const fileInfoList) {
    for (size_t i = 0; i < fileInfoList->size; ++i)
        if (fileInfoList->data[i].path_owner)
            free(fileInfoList->data[i].path);
    free(fileInfoList->data);
}
