#ifndef HEADER_H
#define HEADER_H

#define STRING_SIZE_MAX 255

#define SHM_FD_ELEMENTS_COUNT_ 5
enum shm_fd_names
{
  SHM_FD_CLIENTS_AMOUNT,
  SHM_FD_MESSAGES_AMOUNT,
  SHM_FD_CLIENTS_ID,
  SHM_FD_MESSAGE_STRING,
  SHM_FD_MESSAGE_CLIENT_ID
};

enum semaphore_names
{
  SEMAPHORE_MAIN,
  SEMAPHORE_EXIT
};

static const char *const k_shm_names[5]
    = { "/CLIENTS_AMOUNT", "/MESSAGES_AMOUNT", "/CLIENTS_ID",
        "/MESSAGE_STRING", "/MESSAGE_CLIENT_ID" };

static const char *const k_sem_names[2]
    = { "/SEMAPHORE_MAIN", "/SEMAPHORE_EXIT" };

#endif // HEADER_H