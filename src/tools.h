#pragma once
#include <SDL3/SDL.h>


/* Returns directory path of given file path */
char* DTP_GetDirectory(const char* path);

/* Returns filename (without extension if there's one) of given file path */
char* DTP_GetFilename(const char* path);
