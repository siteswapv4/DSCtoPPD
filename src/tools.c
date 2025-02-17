#include "tools.h"


char* DTP_GetDirectory(const char* path)
{
	char* copy = SDL_strdup(path);
	int i = SDL_strlen(path) - 1;

    while ((i >= 0) && (path[i] != '/') && (path[i] != '\\'))
    {
        i--;
    }

	copy[i + 1] = '\0';

    return copy;
}


char* DTP_GetFilename(const char* path)
{
	char* copy = NULL;
	int i = SDL_strlen(path) - 1;
	
	while ((i >= 0) && (path[i] != '/') && (path[i] != '\\'))
	{
		i--;
	}
	
	i++;
	copy = SDL_strdup(&path[i]);
	
	for (; copy[i] != '\0'; i++)
	{
		if (copy[i] == '.')
		{
			copy[i] = '\0';
			return copy;
		}
	}
	
	return copy;
}
