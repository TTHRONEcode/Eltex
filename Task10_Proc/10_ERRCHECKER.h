#ifndef ERRCHECKER_INCLUDE
#define ERRCHECKER_INCLUDE

#include <sys/types.h>

#define D_ERRCHECK_fork ERRCHECK_fork(__FILE_NAME__, __func__, __LINE__)
pid_t ERRCHECK_fork(const char *, const char *, int);

#define D_ERRCHECK_wait ERRCHECK_wait(__FILE_NAME__, __func__, __LINE__)
int ERRCHECK_wait(const char *, const char *, int);

#endif