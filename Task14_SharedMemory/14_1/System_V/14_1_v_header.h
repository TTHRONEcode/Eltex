#ifndef HEADER_H
#define HEADER_H

#define TO_STRING(x) #x

const char *const k_key_pathname = "./";

#include <sys/sem.h>
struct sembuf lock[2] = { { 0, 0, 0 }, { 0, 2, 0 } };
struct sembuf unlock = { 0, -1, 0 };

#endif // HEADER_H