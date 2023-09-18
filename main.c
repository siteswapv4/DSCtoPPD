#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#include "tools.h"
#include "functions.c"

#ifdef _WIN32
# define mkdir(X, Y) mkdir(X)
#endif

typedef struct
{
    char* folderPath;
    char* projectPath;
    char* filePath;
    char* filename;
    char* layerPath;
    char* bpmPath;
    char* iniPath;
    char* evdPath;
    char* ppdPath;
    char* csinputPath;
    char* divascriptPath;
    char* targetDivascriptPath;
    char* targetCsinputPath;
    char* targetLayerPath;
    char* scriptFolderPath;
    char* layerFolderPath;
    char* soundFolderPath;
    char* ressourceFolderPath;
    char* soundsetPath;
    char* targetSoundsetPath;
    char* soundPath;
    char* targetSoundPath;
    char* ppdprojPath;
    char* outputFolderPath;
    char* difficultyChar;
}Tpath;

/*
Fills path strings by extracting the current folder location
*/
int initPath(int argc, char** argv, int choice, int difficulty, Tpath* path)
{
    path->difficultyChar = path->outputFolderPath = path->ppdprojPath = path->targetSoundsetPath = path->soundsetPath = path->soundPath = path->targetDivascriptPath = path->targetCsinputPath = path-> targetLayerPath = path->scriptFolderPath = path-> layerFolderPath = path-> soundFolderPath = path->ressourceFolderPath = path->folderPath = path->filePath = path->filename = path->ppdPath = path->layerPath = path->projectPath = path->bpmPath = path->iniPath = path->csinputPath = path->divascriptPath = path->evdPath = NULL;

    //add path
    if (argc > 0)
    {
        path->folderPath = extractFolderPath(argv[0]);


        if (argc > 1)
        {
            switch(difficulty)
            {
                case 1:
                    path->difficultyChar = strdup("Easy");
                    break;
                case 2:
                    path->difficultyChar = strdup("Normal");
                    break;
                case 3:
                    path->difficultyChar = strdup("Hard");
                    break;
                case 4:
                    path->difficultyChar = strdup("Extreme");
                    break;
                case 5:
                    path->difficultyChar = strdup("Base");
                    break;
            }
            
            path->outputFolderPath = (char*)malloc((strlen(path->folderPath) + 100) * sizeof(char)); path->outputFolderPath[0] = 0;
            strcat(path->outputFolderPath, path->folderPath);
            switch(choice)
            {
                case 1:
                    strcat(path->outputFolderPath, "outputProjects");
                    break;
                case 2:
                    strcat(path->outputFolderPath, "outputLayers");
                    break;
                case 3:
                    strcat(path->outputFolderPath, "outputScores");
                    break;
            }

            path->filePath = strdup(argv[1]);

            path->filename = extractFileName(path->filePath, 0);

            path->projectPath = (char*)malloc((strlen(path->outputFolderPath) + strlen(path->filename) + 100) * sizeof(char)); path->projectPath[0] = 0;
            strcat(path->projectPath, path->outputFolderPath); strcat(path->projectPath, "/"); strcat(path->projectPath, path->filename);

            path->ppdprojPath = (char*)malloc((strlen(path->projectPath) + strlen(path->filename) + 100) * sizeof(char)); path->ppdprojPath[0] = 0;
            strcat(path->ppdprojPath, path->projectPath); strcat(path->ppdprojPath, ".ppdproj");

            path->layerPath = (char*)malloc((strlen(path->projectPath) + strlen(path->filename) + 10) * sizeof(char)); path->layerPath[0] = 0;
            strcat(path->layerPath, path->projectPath); strcat(path->layerPath, "/"); strcat(path->layerPath, path->filename); strcat(path->layerPath, ".ppd");

            path->bpmPath = (char*)malloc((strlen(path->projectPath) + 20) * sizeof(char)); path->bpmPath[0] = 0;
            strcat(path->bpmPath, path->projectPath); strcat(path->bpmPath, "/"); strcat(path->bpmPath, "BPM.fsml");

            path->iniPath = (char*)malloc((strlen(path->projectPath) + 20) * sizeof(char)); path->iniPath[0] = 0;
            strcat(path->iniPath, path->projectPath); strcat(path->iniPath, "/"); strcat(path->iniPath, "data.ini");

            path->evdPath = (char*)malloc((strlen(path->projectPath) + 20) * sizeof(char)); path->evdPath[0] = 0;
            strcat(path->evdPath, path->projectPath); strcat(path->evdPath, "/"); strcat(path->evdPath, "evd.txt");

            path->ppdPath = (char*)malloc((strlen(path->projectPath) + 20) * sizeof(char)); path->ppdPath[0] = 0;
            strcat(path->ppdPath, path->projectPath); strcat(path->ppdPath, "/"); strcat(path->ppdPath, path->difficultyChar); strcat(path->ppdPath, ".ppd");
            
            path->csinputPath = (char*)malloc((strlen(path->folderPath) + 50) * sizeof(char)); path->csinputPath[0] = 0;
            strcat(path->csinputPath, path->folderPath); strcat(path->csinputPath, "Data/CSInput.fsml");

            path->divascriptPath = (char*)malloc((strlen(path->folderPath) + 50) * sizeof(char)); path->divascriptPath[0] = 0;
            strcat(path->divascriptPath, path->folderPath); strcat(path->divascriptPath, "Data/DivaScript.fsml");

            path->soundFolderPath = (char*)malloc((strlen(path->projectPath) + 50) * sizeof(char)); path->soundFolderPath[0] = 0;
            strcat(path->soundFolderPath, path->projectPath); strcat(path->soundFolderPath, "/Sound");

            path->soundPath = (char*)malloc((strlen(path->folderPath) + 50) * sizeof(char)); path->soundPath[0] = 0;
            strcat(path->soundPath, path->folderPath); strcat(path->soundPath, "Data/sound.wav");    

            path->targetSoundPath = (char*)malloc((strlen(path->projectPath) + 50) * sizeof(char)); path->targetSoundPath[0] = 0;
            strcat(path->targetSoundPath, path->projectPath); strcat(path->targetSoundPath, "/Sound/sound.wav");

            path->soundsetPath = (char*)malloc((strlen(path->folderPath) + 50) * sizeof(char)); path->soundsetPath[0] = 0;
            strcat(path->soundsetPath, path->folderPath); strcat(path->soundsetPath, "Data/soundset.txt");   

            path->targetSoundsetPath = (char*)malloc((strlen(path->projectPath) + 50) * sizeof(char)); path->targetSoundsetPath[0] = 0;
            strcat(path->targetSoundsetPath, path->projectPath); strcat(path->targetSoundsetPath, "/soundset.txt");

            path->scriptFolderPath = (char*)malloc((strlen(path->projectPath) + strlen(path->difficultyChar) + 50) * sizeof(char)); path->scriptFolderPath[0] = 0;
            strcat(path->scriptFolderPath, path->projectPath); strcat(path->scriptFolderPath, "/"); strcat(path->scriptFolderPath, path->difficultyChar); strcat(path->scriptFolderPath, "_Scripts");
        
            path->layerFolderPath = (char*)malloc((strlen(path->projectPath) + strlen(path->difficultyChar) + 50) * sizeof(char)); path->layerFolderPath[0] = 0;
            strcat(path->layerFolderPath, path->projectPath); strcat(path->layerFolderPath, "/"); strcat(path->layerFolderPath, path->difficultyChar); strcat(path->layerFolderPath, "_Layers");
        
            path->targetLayerPath = (char*)malloc((strlen(path->layerFolderPath) + 50) * sizeof(char)); path->targetLayerPath[0] = 0;
            strcat(path->targetLayerPath, path->layerFolderPath); strcat(path->targetLayerPath, "/layer0.ppd");

            path->targetCsinputPath = (char*)malloc((strlen(path->scriptFolderPath) + 50) * sizeof(char)); path->targetCsinputPath[0] = 0;
            strcat(path->targetCsinputPath, path->scriptFolderPath); strcat(path->targetCsinputPath, "/CSInput.fsml");

            path->targetDivascriptPath = (char*)malloc((strlen(path->scriptFolderPath) + 50) * sizeof(char)); path->targetDivascriptPath[0] = 0;
            strcat(path->targetDivascriptPath, path->scriptFolderPath); strcat(path->targetDivascriptPath, "/DivaScript.fsml");
        }
        else
        {
            printf("Missing file path\n");

            return 1;
        }
    }
    else
    {
        printf("Missing executable path, can't create folders\n");

        return 1;
    }

    return 0;
}

/*
Free all strings that had memory allocations in Tpath structure
*/
int freePath(Tpath* path)
{
    if (path->folderPath != NULL)
    {
        free(path->folderPath);
    }
    if (path->projectPath != NULL)
    {
        free(path->projectPath);
    }
    if (path->filePath != NULL)
    {
        free(path->filePath);
    }
    if (path->filename != NULL)
    {
        free(path->filename);
    }
    if (path->layerPath != NULL)
    {
        free(path->layerPath);
    }
    if (path->bpmPath != NULL)
    {
        free(path->bpmPath);
    }
    if (path->iniPath != NULL)
    {
        free(path->iniPath);
    }
    if (path->csinputPath != NULL)
    {
        free(path->csinputPath);
    }
    if (path->divascriptPath != NULL)
    {
        free(path->divascriptPath);
    }
    if (path->evdPath != NULL)
    {
        free(path->evdPath);
    }
    if (path->ppdPath != NULL)
    {
        free(path->ppdPath);
    }

    return 0;
}

int main(int argc, char** argv)
{
    int operation = 0, difficulty = 0;
    Tpath path;
    Tchart chart;

    //Ask for layer, project or score
    while ((operation <= 0) || (operation >= 4))
    {
        printf("Input number;\n1 : PPD project\n2 : PPD layer file (no BPM changes will be ported)\n3 : PPD score (playable chart)\n");
        if (scanf("%d",&operation) != 1) {
            printf("Wrong input");
            return 1;
        }
        fflush(stdin);
    }

    //Ask for difficulty
    while ((difficulty <= 0) || (difficulty >= 6))
    {
        printf("Input number;\n1 : Easy\n2 : Normal\n3 : Hard\n4 : Extreme\n5 : Base\n");
        if (scanf("%d",&difficulty) != 1) {
            printf("Wrong input");
            return 1;
        }
        fflush(stdin);
    }

    if (initPath(argc, argv, operation, difficulty, &path))
    {
        freePath(&path);
        return 1;
    }

    initChart(&chart);

    mkdir(path.outputFolderPath, 0777);
    mkdir(path.projectPath, 0777);
    writeLayer(path.filePath, path.layerPath, &chart);
    writeBPM(path.bpmPath, &chart);
    writeEvd(path.evdPath, &chart);
    writePPD(path.ppdPath, path.layerPath, path.divascriptPath, path.csinputPath, path.bpmPath, path.evdPath);

    if (operation != 2)
    {
        writeIni(path.iniPath, &chart);
        mkdir(path.soundFolderPath, 0777);
        copyTextFile(path.soundPath, path.targetSoundPath, "wb");
        copyTextFile(path.soundsetPath, path.targetSoundsetPath, "wb");

        if (operation == 1)
        {
            mkdir(path.scriptFolderPath, 0777);
            mkdir(path.layerFolderPath, 0777);
            copyTextFile(path.csinputPath, path.targetCsinputPath, "wb");
            copyTextFile(path.divascriptPath, path.targetDivascriptPath, "wb");
            writeLayer(path.filePath, path.targetLayerPath, &chart);
            writePpdproj(path.ppdprojPath, path.difficultyChar, &chart);
        }
    }

    freePath(&path);

    return 0;
}