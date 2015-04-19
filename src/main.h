#include <stdio.h>     // fprintf(), fputs()
#include <stdbool.h>   // bool
#include <stdint.h>    // explicitly-sized integral types
#include <stdlib.h>    // EXIT_SUCCESS
#include <curl/curl.h> // curl_easy_*()
#include <sys/stat.h>  // fstat()
#include <string.h>    // strncat()
#include <fcntl.h>     // open()
#include <unistd.h>    // close()

#define BUFFER_SIZE 256
#define FILE_MAX 62914560 // 60 MiB

// vim: set ts=4 sw=4 et:
