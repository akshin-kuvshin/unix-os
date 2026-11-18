// author: Danila "akshin_" Axyonov

#ifndef FILEINFO_H
#define FILEINFO_H



#include <stddef.h>

#define SHA256_LEN (size_t)32

typedef struct {
    char* path;
    unsigned char hash[SHA256_LEN];
} FileInfo_t;

typedef struct {
    FileInfo_t* data;
    size_t size;
    size_t capacity;
} FileInfoList_t;

void FileInfoList_init(FileInfoList_t* const fileInfoList);
void FileInfoList_append(FileInfoList_t* const fileInfoList, FileInfo_t fileInfo);
const FileInfo_t* FileInfoList_find_by_hash(FileInfoList_t* const fileInfoList, const unsigned char* const hash);
void FileInfoList_free(FileInfoList_t* const fileInfoList);



#endif
