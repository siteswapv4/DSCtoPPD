#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include "tools.h"
#include "functions.h"
#include "data.h"


enum TpathIndexes
{
    folderPath,
    projectPath,
    filePath,
    filename,
    layerPath,
    iniPath,
    evdPath,
    ppdPath,
    targetDivascriptPath,
    targetCsinputPath,
    targetBpmPath,
    targetLayerPath,
    scriptFolderPath,
    layerFolderPath,
    soundFolderPath,
    ressourceFolderPath,
    targetSoundsetPath,
    targetScdPath,
    targetSoundPath,
    ppdprojPath,
    outputFolderPath,
    difficultyChar,
    TPATH_LENGTH,
};


typedef char* Tpath[TPATH_LENGTH];


int DTP_InitPath(int argc, char** argv, int choice, int difficulty, Tpath path, char** error)
{
    if (argc > 0)
    {
        path[folderPath] = DTP_GetDirectory(argv[0]);

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
                    outputFolder = "";
                    break;
                case 2:
                    outputFolder = "";
                    break;
                case 3:
                    outputFolder = "";
                    break;
            }
            
            SDL_asprintf(&path[outputFolderPath], "%s%s", path[folderPath], outputFolder);

            path[filePath] = strdup(argv[1]);

            path[filename] = DTP_GetFilename(path[filePath]);

            SDL_asprintf(&path[projectPath],            "%s/%s",                    path[outputFolderPath], path[filename]);
            SDL_asprintf(&path[layerPath],              "%s/%s.ppd",                path[projectPath], path[filename]);
            SDL_asprintf(&path[ppdprojPath],            "%s.ppdproj",               path[projectPath]);
            SDL_asprintf(&path[iniPath],                "%s/data.ini",              path[projectPath]);
            SDL_asprintf(&path[evdPath],                "%s/evd.txt",               path[projectPath]);
            SDL_asprintf(&path[ppdPath],                "%s/%s.ppd",                path[projectPath], path[difficultyChar]);
            SDL_asprintf(&path[targetScdPath],          "%s/%s.scd",                path[projectPath], path[difficultyChar]);
    
            SDL_asprintf(&path[soundFolderPath],        "%s/Sound",                 path[projectPath]);
            SDL_asprintf(&path[targetSoundPath],        "%s/Sound/sound.wav",       path[projectPath]);
            SDL_asprintf(&path[targetSoundsetPath],     "%s/soundset.txt",          path[projectPath]);

            SDL_asprintf(&path[scriptFolderPath],       "%s/%s_Scripts",            path[projectPath],  path[difficultyChar]);
            SDL_asprintf(&path[targetCsinputPath],      "%s/CSInput.fsml",          path[scriptFolderPath]);
            SDL_asprintf(&path[targetDivascriptPath],   "%s/DivaScript.fsml",       path[scriptFolderPath]);
            SDL_asprintf(&path[targetBpmPath],          "%s/BPM.fsml",              path[scriptFolderPath]);

            SDL_asprintf(&path[layerFolderPath],        "%s/%s_Layers",             path[projectPath],  path[difficultyChar]);
            SDL_asprintf(&path[targetLayerPath],        "%s/layer0.ppd",            path[layerFolderPath]);
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


int DTP_FreePath(Tpath path)
{
    for (int i=0; i<TPATH_LENGTH; i++)
    {
        if (path[i] != NULL)
        {
            SDL_free(path[i]);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int i;
    char *errmsg;
    int operation = 0, difficulty = 0;
    Tpath path = {0};
    DTP_Chart chart = {0};

    chart.parameters = newArrayList();
    chart.flyingTimes = newArrayList();

    //Ask for layer, project or score
    while ((operation <= 0) || (operation >= 4))
    {
        printf("Input number;\n1 : PPD project\n2 : PPD layer file (no BPM changes will be ported)\n3 : PPD score (playable chart)\n");
        if (scanf("%d",&operation) != 1) {
            errmsg = "Invalid input. Aborting...\n";
            goto error;
        }
    }

    //Ask for difficulty
    while ((difficulty <= 0) || (difficulty >= 6))
    {
        printf("Input number;\n1 : Easy\n2 : Normal\n3 : Hard\n4 : Extreme\n5 : Base\n");
        if (scanf("%d",&difficulty) != 1) {
            errmsg = "Invalid input. Aborting...\n";
            goto error;
        }
    }

    if (DTP_InitPath(argc, argv, operation, difficulty, path, &errmsg))
    {
        DTP_FreePath(path);
        goto error;
    }

    SDL_CreateDirectory(path[outputFolderPath]);
    SDL_CreateDirectory(path[projectPath]);
    DTP_WriteLayer(path[filePath], path[layerPath], &chart);
    DTP_WriteEvd(path[evdPath], &chart);
    DTP_WritePPD(path[ppdPath], path[layerPath], path[evdPath]);

    if (operation != 2)
    {
        DTP_WriteIni(path[iniPath], &chart);
        SDL_CreateDirectory(path[soundFolderPath]);
        DTP_WriteData(sound_wav, sound_wav_len, path[targetSoundPath]);
        DTP_WriteData(soundset_txt, soundset_txt_len, path[targetSoundsetPath]);

        if (operation == 1)
        {
            SDL_CreateDirectory(path[scriptFolderPath]);
            SDL_CreateDirectory(path[layerFolderPath]);
            DTP_WriteData(CSInput_fsml, CSInput_fsml_len, path[targetCsinputPath]);
			DTP_WriteData(DivaScript_fsml, DivaScript_fsml_len, path[targetDivascriptPath]);
            DTP_WriteData(BPM_fsml, BPM_fsml_len, path[targetBpmPath]);
            DTP_WriteData(scd_scd, scd_scd_len, path[targetScdPath]);

			freeArrayList(chart.flyingTimes, SDL_free);
			freeArrayList(chart.parameters, SDL_free);
			
			SDL_memset(&chart, 0, sizeof(DTP_Chart));
			
			chart.flyingTimes = newArrayList();
			chart.parameters = newArrayList();

            DTP_WriteLayer(path[filePath], path[targetLayerPath], &chart);
            DTP_WritePpdproj(path[ppdprojPath], path[difficultyChar], &chart);
        }
    }

    DTP_FreePath(path);
    goto end;

error:
    fprintf(stderr, "%s", errmsg);
    fgetc(stdin);
    
    return 1;

end:
    printf("\nPress Enter to leave\n");
    fgetc(stdin);

    return 0;
}
