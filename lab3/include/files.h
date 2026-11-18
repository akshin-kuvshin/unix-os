// author: Danila "akshin_" Axyonov
// Linux specific!

#ifndef FILES_H
#define FILES_H



#include <stdbool.h>

#define BUFFER_SIZE (size_t)4096
#define SHA256_LEN (size_t)32
#define TEMP_DIRNAME_TEMPLATE ".tmp-XXXXXX"
#define TEMP_FILENAME "link"

void calculate_sha256(const char* const filepath, unsigned char* const hash);
bool files_equal(const char* const filepath1, const char* const filepath2);
void replace_with_hardlink(const char* const replacing_filepath, const char* const remaining_filepath);



#endif
