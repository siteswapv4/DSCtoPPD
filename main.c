#define _GNU_SOURCE
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
#include <locale.h>
#include <wchar.h>
#include <windows.h>
#endif


enum TpathIndexes
{
    folderPath,
    projectPath,
    filePath,
    filename,
    layerPath,
    bpmPath,
    iniPath,
    evdPath,
    ppdPath,
    scdPath,
    csinputPath,
    divascriptPath,
    targetDivascriptPath,
    targetCsinputPath,
    targetBpmPath,
    targetLayerPath,
    scriptFolderPath,
    layerFolderPath,
    soundFolderPath,
    ressourceFolderPath,
    soundsetPath,
    targetSoundsetPath,
    targetScdPath,
    soundPath,
    targetSoundPath,
    ppdprojPath,
    outputFolderPath,
    difficultyChar,
    TPATH_LENGTH,
};

typedef char* Tpath[TPATH_LENGTH];

void makeDirectory(char* path)
{
    #ifdef _WIN32
    wchar_t* wspath;
    long len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wspath = calloc(len + 1, sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, path, -1, &wspath[0], len);

    _wmkdir(wspath);

    free(wspath);
    #else
    mkdir(path, 0777);
    #endif
}

/*
Fills path strings by extracting the current folder location
*/
int initPath(int argc, char** argv, int choice, int difficulty, Tpath path, char** error)
{
    //add path
    if (argc > 0)
    {
        path[folderPath] = extractFolderPath(argv[0]);


        if (argc > 1)
        {
            switch(difficulty)
            {
                case 1:
                    path[difficultyChar] = strdup("Easy");
                    break;
                case 2:
                    path[difficultyChar] = strdup("Normal");
                    break;
                case 3:
                    path[difficultyChar] = strdup("Hard");
                    break;
                case 4:
                    path[difficultyChar] = strdup("Extreme");
                    break;
                case 5:
                    path[difficultyChar] = strdup("Base");
                    break;
            }
            
            char *outputFolder;
            switch(choice)
            {
                case 1:
                    outputFolder = "outputProjects";
                    break;
                case 2:
                    outputFolder = "outputLayers";
                    break;
                case 3:
                    outputFolder = "outputScores";
                    break;
            }
            asprintf(&path[outputFolderPath], "%s%s", path[folderPath], outputFolder);

            path[filePath] = strdup(argv[1]);

            path[filename] = extractFileName(path[filePath], 0);

            asprintf(&path[projectPath],            "%s/%s",                    path[outputFolderPath], path[filename]);
            asprintf(&path[layerPath],              "%s/%s.ppd",                path[projectPath], path[filename]);
            asprintf(&path[ppdprojPath],            "%s.ppdproj",               path[projectPath]);
            asprintf(&path[iniPath],                "%s/data.ini",              path[projectPath]);
            asprintf(&path[evdPath],                "%s/evd.txt",               path[projectPath]);
            asprintf(&path[ppdPath],                "%s/%s.ppd",                path[projectPath], path[difficultyChar]);
            asprintf(&path[targetScdPath],          "%s/%s.scd",                path[projectPath], path[difficultyChar]);
            asprintf(&path[scdPath],                "%sData/scd.scd",           path[folderPath]);
    
            asprintf(&path[soundPath],              "%sData/Sound.wav",         path[folderPath]);
            asprintf(&path[soundsetPath],           "%sData/soundset.txt",      path[folderPath]);
            asprintf(&path[soundFolderPath],        "%s/Sound",                 path[projectPath]);
            asprintf(&path[targetSoundPath],        "%s/Sound/sound.wav",       path[projectPath]);
            asprintf(&path[targetSoundsetPath],     "%s/soundset.txt",          path[projectPath]);

            asprintf(&path[csinputPath],            "%sData/CSInput.fsml",      path[folderPath]);
            asprintf(&path[divascriptPath],         "%sData/DivaScript.fsml",   path[folderPath]);
            asprintf(&path[bpmPath],                "%s/BPM.fsml",              path[projectPath]);
            asprintf(&path[scriptFolderPath],       "%s/%s_Scripts",            path[projectPath],  path[difficultyChar]);
            asprintf(&path[targetCsinputPath],      "%s/CSInput.fsml",          path[scriptFolderPath]);
            asprintf(&path[targetDivascriptPath],   "%s/DivaScript.fsml",       path[scriptFolderPath]);
            asprintf(&path[targetBpmPath],          "%s/BPM.fsml",              path[scriptFolderPath]);

            asprintf(&path[layerFolderPath],        "%s/%s_Layers",             path[projectPath],  path[difficultyChar]);
            asprintf(&path[targetLayerPath],        "%s/layer0.ppd",            path[layerFolderPath]);
        }
        else
        {
            *error = "Missing file path\n";

            return 1;
        }
    }
    else
    {
        *error = "Missing executable path, can't create folders\n";

        return 1;
    }

    return 0;
}

/*
Free all strings that had memory allocations in Tpath structure
*/
int freePath(Tpath path)
{
    for (int i=0; i<TPATH_LENGTH; i++)
    {
        if (path[i] != NULL)
        {
            free(path[i]);
        }
    }

    return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    int i;
    char *errmsg;
    int operation = 0, difficulty = 0;
    Tpath path = {0};
    Tchart chart = {0};
    char** arguments;

    arguments = malloc(argc * sizeof(char*));
    #ifdef _WIN32
    for (i=0; i<argc; i++)
    {
        long len = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);
        arguments[i] = calloc(len + 1, sizeof(char));
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, &arguments[i][0], len, NULL, NULL);
    }
    #else
    for (i=0; i<argc; i++)
    {
        arguments[i] = strdup(argv[i]);
    }
    #endif

    initChart(&chart);

    //Ask for layer, project or score
    while ((operation <= 0) || (operation >= 4))
    {
        printf("Input number;\n1 : PPD project\n2 : PPD layer file (no BPM changes will be ported)\n3 : PPD score (playable chart)\n");
        if (scanf("%d",&operation) != 1) {
            errmsg = "Invalid input. Aborting...\n";
            goto error;
        }
        fflush(stdin);
    }

    //Ask for difficulty
    while ((difficulty <= 0) || (difficulty >= 6))
    {
        printf("Input number;\n1 : Easy\n2 : Normal\n3 : Hard\n4 : Extreme\n5 : Base\n");
        if (scanf("%d",&difficulty) != 1) {
            errmsg = "Invalid input. Aborting...\n";
            goto error;
        }
        fflush(stdin);
    }

    printf("Input offset in milliseconds : ");
    if (scanf("%f",&chart.customOffset) != 1) {
        errmsg = "Invalid input. Aborting...\n";
        goto error;
    }
    fflush(stdin);

    chart.customOffset /= 1000;

    if (initPath(argc, arguments, operation, difficulty, path, &errmsg))
    {
        freePath(path);
        goto error;
    }

    makeDirectory(path[outputFolderPath]);
    makeDirectory(path[projectPath]);
    writeLayer(path[filePath], path[layerPath], &chart);
    writeBPM(path[bpmPath], &chart);
    writeEvd(path[evdPath], &chart);
    writePPD(path[ppdPath], path[layerPath], path[divascriptPath], path[csinputPath], path[bpmPath], path[evdPath]);

    if (operation != 2)
    {
        writeIni(path[iniPath], &chart);
        makeDirectory(path[soundFolderPath]);
        copyTextFile(path[soundPath], path[targetSoundPath], "wb");
        copyTextFile(path[soundsetPath], path[targetSoundsetPath], "wb");

        if (operation == 1)
        {
            makeDirectory(path[scriptFolderPath]);
            makeDirectory(path[layerFolderPath]);
            copyTextFile(path[csinputPath], path[targetCsinputPath], "wb");
            copyTextFile(path[divascriptPath], path[targetDivascriptPath], "wb");
            copyTextFile(path[scdPath], path[targetScdPath], "wb");

            chart.flyingTime.len = 0;
            chart.flyingTimeTiming.len = 0;

            writeLayer(path[filePath], path[targetLayerPath], &chart);
            writePpdproj(path[ppdprojPath], path[difficultyChar], &chart);
            writeBPM(path[targetBpmPath], &chart);
        }
    }

    freePath(path);
    goto end;

error:
    fprintf(stderr, "%s", errmsg);
    fflush(stdin); getchar();
    return 1;

end:
    printf("\nPress Enter to leave\n");
    fflush(stdin); getchar();

    return 0;
}