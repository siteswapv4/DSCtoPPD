#ifdef _WIN32
#   include <stdarg.h>

    int vasprintf(char **sptr, char *fmt, va_list argv)
    {
        int wanted = vsnprintf(*sptr = NULL, 0, fmt, argv);
        if((wanted > 0) && ((*sptr = malloc(1 + wanted)) != NULL))
            return vsprintf(*sptr, fmt, argv);

        return wanted;
    }

    int asprintf(char **sptr, char *fmt, ...)
    {
        int retval;
        va_list argv;
        va_start(argv, fmt);
        retval = vasprintf(sptr, fmt, argv);
        va_end(argv);
        return retval;
    }
#endif


int invertString(char* string, char** outputString)
{
    int i = 0;
    char* tempString = (char*)malloc((strlen(string) + 1) * sizeof(char));
    tempString[0] = 0;

    for (i=(strlen(string) - 1); i>=0; i--)
    {
        int len = strlen(tempString);
        tempString[len] = string[i];
        tempString[len + 1] = 0;
        
    }

    free(*outputString);
    *outputString = tempString;

    return 0;
}

/*
Takes path to a file and returns path to the folder containing the file
*/
char* extractFolderPath(char* filePath)
{
    int i = strlen(filePath) - 1;

    while ((i >= 0) && (filePath[i] != '/') && (filePath[i] != '\\'))
    {
        filePath[i] = 0;

        i--;
    }

    if (i >= 0)
    {
        return strdup(filePath);
    }
    else
    {
        return strdup("./");
    }
}

/*
Takes a path to a file and returns the file name
*/
char* extractFileName(char* filePath, int extension)
{
    int i = strlen(filePath) - 1, j = 0;
    char* filename = (char*)malloc((strlen(filePath) + 1) * sizeof(char));
    filename[0] = 0;

    while ((i >= 0) && (filePath[i] != '/') && (filePath[i] != '\\'))
    {
        filename[j] = filePath[i];
        filename[j+1] = 0;

        i--;
        j++;
    }

    invertString(filename, &filename);

    if (i > 0)
    {
        filename[strlen(filename) - 4] = 0;

        return filename;
    }
    else
    {
        free(filename);
        char* res = strdup(filePath);
        res[strlen(res) - 4] = 0;

        return res;
    }
}