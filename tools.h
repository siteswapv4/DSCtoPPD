int invertString(char* string, char** outputString)
{
    int i = 0;
    char* tempString = (char*)malloc((strlen(string) + 1) * sizeof(char));
    tempString[0] = 0;

    for (i=(strlen(string) - 1); i>=0; i--)
    {
        tempString[strlen(tempString)] = string[i];
        tempString[strlen(tempString)] = 0;
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
    char* cpyFilePath = strdup(filePath);

    while ((i > 0) && (filePath[i] != '/') && (filePath[i] != '\\'))
    {
        filePath[i] = 0;

        i--;
    }

    if (i >= 0)
    {
        free(cpyFilePath);
        return strdup(filePath);
    }
    else
    {
        printf("Couldn't find folder in given string\n");
        return cpyFilePath;
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

    while ((i > 0) && (filePath[i] != '/') && (filePath[i] != '\\'))
    {
        filename[j] = filePath[i];
        filename[j+1] = 0;

        i--;
        j++;
    }

    invertString(filename, &filename);

    if (i >= 0)
    {
        if (extension)
        {
            return filename;
        }
        else
        {
            filename[strlen(filename) - 4] = 0;
            return filename;
        }
    }
    else
    {
        printf("Couldn't find folder in given string\n");
        free(filename);
        return strdup(filePath);
    }
}