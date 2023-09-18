#define LIST_SIZE 10

#ifdef _WIN32
#include <Windows.h>
#endif

typedef struct
{
    int len;
    int size;
    int* value;
}TintArray;

typedef struct
{
    int len;
    int size;
    float* value;
}TfloatArray;

typedef struct
{
    float time;
    float xpos;
    float ypos;
    float angle;
    int type;
    float releaseTime;
    int ID;
}Tnote;

typedef struct
{
    TintArray distance;
    TintArray amplitude;
    TintArray frequency;
    TintArray flyingTime;
    TfloatArray flyingTimeTiming;
    float offset;
    float customOffset;
    float endTime;
    int ID;
    bool isHold;
    bool isSlideL;
    bool isSlideR;
}Tchart;

/*
Check if char is free, if not frees it
*/
void freeChar(char* input, char** output)
{
    if (input != NULL)
    {
        free(input);
        input = NULL;
    }

    *output = input;
}

/*
Init the Tchart struct to default values and allocates default sizes to list
*/
void initChart(Tchart* chart)
{
    //init distance
    chart->distance.len = 0;
    chart->distance.size = LIST_SIZE;
    chart->distance.value = (int*)malloc(chart->distance.size * sizeof(int*));

    //init amplitude
    chart->amplitude.len = 0;
    chart->amplitude.size = LIST_SIZE;
    chart->amplitude.value = (int*)malloc(chart->amplitude.size * sizeof(int*));

    //init frequency
    chart->frequency.len = 0;
    chart->frequency.size = LIST_SIZE;
    chart->frequency.value = (int*)malloc(chart->frequency.size * sizeof(int*));

    //init flyingTime
    chart->flyingTime.len = 0;
    chart->flyingTime.size = LIST_SIZE;
    chart->flyingTime.value = (int*)malloc(chart->flyingTime.size * sizeof(int*));

    chart->flyingTimeTiming.len = 0;
    chart->flyingTimeTiming.size = LIST_SIZE;
    chart->flyingTimeTiming.value = (float*)malloc(chart->flyingTimeTiming.size * sizeof(float*));

    //init numbers
    chart->offset = 0;
    chart->customOffset = 0;
    chart->endTime = 0;
    chart->ID = 0;

    //Init bool
    chart->isHold = false;
    chart->isSlideL = false;
    chart->isSlideR = false;
}

/*
Adds int to array, checking its size and realocating space if needed
*/
void addIntToArray(int value, TintArray* array)
{
    int i = 0;

    if (array->len >= array->size)
    {
        TintArray tempArray;
        tempArray.len = array->len;
        tempArray.size = array->size;
        tempArray.value = (int*)malloc(tempArray.size * sizeof(int*));

        for (i=0; i<array->len; i++)
        {
            tempArray.value[i] = array->value[i];
        }

        array->size += LIST_SIZE;
        array->value = (int*)realloc(array->value, array->size * sizeof(int*));

        for (i=0; i<array->len; i++)
        {
            array->value[i] = tempArray.value[i];
        }

        free(tempArray.value);
    }

    array->value[array->len] = value;
    array->len++;
}

/*
Adds float to array, checking its size and realocating space if needed
*/
void addFloatToArray(float value, TfloatArray* array)
{
    int i = 0;

    if (array->len >= array->size)
    {
        TfloatArray tempArray;
        tempArray.len = array->len;
        tempArray.size = array->size;
        tempArray.value = (float*)malloc(tempArray.size * sizeof(float*));

        for (i=0; i<array->len; i++)
        {
            tempArray.value[i] = array->value[i];
        }

        array->size += LIST_SIZE;
        array->value = (float*)realloc(array->value, array->size * sizeof(float*));

        for (i=0; i<array->len; i++)
        {
            array->value[i] = tempArray.value[i];
        }

        free(tempArray.value);
    }

    array->value[array->len] = value;
    array->len++;
}

/*
look for opcode in list and return len if found
*/
int findOpcode(int opcodes[2][2], int hex)
{
    int i;
    for (i=0; i<107; i++)
    {
        if (hex == opcodes[i][0])
        {
            //printf("hex : %d, opcode : %d", hex, opcodes[i][0]);
            return opcodes[i][1];
        }
    }
    return -1;
}

/*
Returns number of digits in int n
*/
int countDigits(int n)
{
    int count = 0;

    if (n < 0)
    {
        count = 1;
    }
    else if (n == 0)
    {
        return 1;
    }

    while(n != 0)
    {
        n /= 10;
        count++;
    }

    return count;
}


/*
Take dsc file, read all target data and fill Tnote struct
*/
void convertTarget(FILE* dsc, Tnote* note, Tchart* chart, signed int* hex, float currentTime, Tchart** outputChart)
{
    float tempFloat = 0;
    int tempInt = 0;

    //FILL NOTES
    note->time = 0;
    note->xpos = 0;
    note->ypos = 0;
    note->angle = 0;
    note->type = 0;
    note->releaseTime = 0;
    note->ID = 0;

    //TIME
    note->time = currentTime;
    if (chart->flyingTime.len > 0)
    {
        note->time += chart->flyingTime.value[chart->flyingTime.len - 1];
    }
    note->time /= 1000;
    note->time += chart->offset;

    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    //TYPE
    switch (*hex)
    {
        //convert types
        case 0:
            note->type = 3;
            break;
        case 1:
            note->type = 2;
            break;
        case 2:
            note->type = 1;
            break;
        case 3:
            note->type = 0;
            break;
        case 4:
            note->type = 23;
            chart->isHold = true;
            break;
        case 5:
            note->type = 22;
            chart->isHold = true;
            break;
        case 6:
            note->type = 21;
            chart->isHold = true;
            break;
        case 7:
            note->type = 20;
            chart->isHold = true;
            break;
        case 12:
            if (chart->isSlideL)
            {
                note->type = 3;
            }
            else
            {
                note->type = 9;
                chart->isSlideL = true;
            }
            break;
        case 13:
            if (chart->isSlideR)
            {
                note->type = 2;
            }
            else
            {
                note->type = 8;
                chart->isSlideR = true;
            }
            break;
        case 15:
            note->type = 29;
            chart->isHold = true;
            break;
        case 16:
            note->type = 28;
            chart->isHold = true;
            break;
        default:
            break;
    }
    note->type += 40;

    //XPOS
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    tempFloat = (float)*hex;
    note->xpos = tempFloat * ((float)800/(float)480000);

    //YPOS
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    tempFloat = (float)*hex;
    note->ypos = tempFloat * ((float)450/(float)270000);

    tempFloat = 0;

    //ANGLE
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    tempFloat = (float)*hex;
    note->angle = tempFloat / (float)1000;

    if (note->angle < 0)
    {
        note->angle += 360;
    }

    note->angle = 360 - note->angle;
    note->angle += 90;

    if (note->angle > 360)
    {
        note->angle -= 360;
    }

    note->angle *= 3.13159/(float)180;

    //DISTANCE
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    addIntToArray(*hex, &(chart->distance));

    //AMPLITUDE
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    addIntToArray(*hex, &(chart->amplitude));

    //FREQUENCY
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
    tempInt = *hex;
    if (tempInt%2 == 0)
    {
        tempInt *= -1;
    }
    addIntToArray(tempInt, &(chart->frequency));

    //HOLD
    if (chart->isHold)
    {
        note->releaseTime = note->time + 0.01;
    }

    //ID
    chart->ID++;
    note->ID = chart->ID;

    chart->isHold = false;
    *outputChart = chart;
    *hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
}

/*
write Tnote struct in PPD layer file
*/
void writeTarget(FILE* ppd, Tnote note)
{
    fwrite(&(note.time), sizeof(note.time), 1, ppd);
    fwrite(&(note.xpos), sizeof(note.xpos), 1, ppd);
    fwrite(&(note.ypos), sizeof(note.ypos), 1, ppd);
    fwrite(&(note.angle), sizeof(note.angle), 1, ppd);
    fprintf(ppd, "%c", note.type);
    if (note.releaseTime != 0)
    {
        fwrite(&(note.releaseTime), sizeof(note.releaseTime), 1, ppd);
    }
    fwrite(&(note.ID), sizeof(note.ID), 1, ppd);
}

/*
Opens DSC as read and PPD layer as write byte, and convert DSC file to PPD layer (+ store flying time)
*/
int writeLayer(char* DSCPath, char* layerFilePath, Tchart* chart)
{
    //full opcodes (arguments of DSC files), first is the code, second is the length of the arguments
    int opcodes[107][2] = {{105,8},{46,3},{97,1},{56,2},{28,2},{93,2},{14,1},{96,4},{72,2},{60,2},{103,3},{62,4},{59,2},{50,2},{94,3},{13,2},{66,2},{95,3},{48,1},{81,24},{44,1},{43,2},{78,2},{41,2},{40,1},{75,3},{79,3},{30,1},{45,2},{76,2},{42,1},{34,2},{82,1},{27,4},{91,6},{77,4},{64,2},{36,1},{80,2},{38,7},{74,9},{39,1},{35,5},{9,6},{11,1},{0,0},{22,4},{18,3},{89,1},{10,2},{17,2},{55,1},{92,3},{20,5},{47,3},{87,3},{15,1},{101,4},{61,4},{85,3},{88,4},{51,3},{21,4},{23,5},{24,2},{98,1},{4,2},{2,4},{3,2},{5,2},{26,2},{19,5},{31,21},{16,3},{102,1},{68,1},{67,1},{25,0},{49,2},{71,3},{70,3},{57,3},{106,2},{65,1},{32,0},{83,2},{54,1},{52,6},{63,1},{12,6},{37,1},{7,4},{8,2},{73,1},{29,2},{33,3},{90,2},{86,1},{100,3},{104,3},{6,7},{84,1},{58,1},{1,1},{53,6},{99,3},{69,3}};
    signed int hex = 0;
    signed int tempHex = 0;
    float currentTime = 0;
    int i = 0;
    float timeChainL[2] = {0};
    float timeChainR[2] = {0};
    Tnote noteChainL;
    Tnote noteChainR;
    bool firstChainL = true;
    bool firstChainR = true;
    bool chainL = false;
    bool chainR = false;
    int chainPosL = 0;
    int chainPosR = 0;
    int currentPos = 0;
    FILE* dsc = NULL;
    FILE* ppd = NULL;

    dsc = fopen(DSCPath, "rb");
    ppd = fopen(layerFilePath, "wb+");

    if (dsc == NULL)
    {
        printf("Error when opening DSC file\n");
        return 1;
    }
    if (ppd == NULL)
    {
        printf("Couldn't create ppd later file\n");
        return 1;
    }

    fprintf(ppd, "%s", "PPD");

    //first 4 bytes are useless
    fseek(dsc, 4, SEEK_SET);

    //starting now
    hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);

    //Looking for the offset first (offset opcode is 25)
    while((hex != 0) && (hex != EOF) && (hex != 25))
    {
        //store time as it could be future offset
        if (hex == 1)
        {
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
            currentTime = (float)hex / (float)100000;
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
        }
        //skip opcode as it's not important
        else
        {
            if (findOpcode(opcodes, hex) == -1)
            {
                printf("wrong opcode : proceeding, might break\n");
            }
            for (i=0; i < (findOpcode(opcodes, hex)); i++)
            {
                tempHex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
            }
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
        }
    }

    //store collected offset
    if (hex == 25)
    {
        chart->offset = (currentTime * -1);
    }

    chart->offset += chart->customOffset;

    //return to the start
    fseek(dsc, 4, SEEK_SET);
    currentTime = 0;
    tempHex = 0;
    i = 0;
    hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);

    while ((hex != 0) && (hex != EOF))
    {
        //TIME
        if (hex == 1)
        {
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
            currentTime = (float)hex / (float)100;
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
            chart->isSlideL = false;
            chart->isSlideR = false;
        }
        //FLYING TIME
        else if (hex == 58)
        {
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
            if ((chart->flyingTime.len == 0) || (chart->flyingTime.value[chart->flyingTime.len-1] != hex))
            {
                addIntToArray(hex, &(chart->flyingTime));
                addFloatToArray(((currentTime / (float)1000) + chart->offset + ((float)hex / (float)1000) - 0.000001), &(chart->flyingTimeTiming));
            }
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
        }
        //TARGET
        else if (hex == 6)
        {
            Tnote note;
            convertTarget(dsc, &note, chart, &hex, currentTime, &chart);

            //Handle Chain Slides
            //Left
            if (note.type == 69)
            {
                float gap1 = note.time - timeChainL[0];
                float gap2 = timeChainL[0] - timeChainL[1];
                //If gap between 2 chain slides is the same : still a part of the chain, skip the note. Else, write previous chain, store current chain
                if ((firstChainL) || ((!chainL) && (!((gap1 + 0.01 > gap2) && (gap1 -0.01 < gap2)))))
                {
                    if (!firstChainL)
                    {
                        noteChainL.releaseTime = timeChainL[0];
                        currentPos = ftell(ppd);
                        fseek(ppd, chainPosL, SEEK_SET);
                        writeTarget(ppd, noteChainL);
                        fseek(ppd, currentPos, SEEK_SET);
                    }
                    chainPosL = ftell(ppd);
                    writeTarget(ppd, note);
                    noteChainL = note;
                    firstChainL = false;
                    chainL = true;
                }
                else
                {
                    chainL = false;
                    chart->distance.len--;
                    chart->amplitude.len--;
                    chart->frequency.len--;
                    chart->ID--;
                }
                timeChainL[1] = timeChainL[0];
                timeChainL[0] = note.time;
            }
            //Right
            else if (note.type == 68)
            {
                float gap1 = note.time - timeChainR[0];
                float gap2 = timeChainR[0] - timeChainR[1];
                if ((firstChainR) || ((!chainR) && (!((gap1 + 0.01 > gap2) && (gap1 -0.01 < gap2)))))
                {
                    if (!firstChainR)
                    {
                        noteChainR.releaseTime = timeChainR[0];
                        currentPos = ftell(ppd);
                        fseek(ppd, chainPosR, SEEK_SET);
                        writeTarget(ppd, noteChainR);
                        fseek(ppd, currentPos, SEEK_SET);
                    }
                    chainPosR = ftell(ppd);
                    writeTarget(ppd, note);
                    noteChainR = note;
                    firstChainR = false;
                    chainR = true;
                }
                else
                {
                    chainR = false;
                    chart->distance.len--;
                    chart->amplitude.len--;
                    chart->frequency.len--;
                    chart->ID--;
                }
                timeChainR[1] = timeChainR[0];
                timeChainR[0] = note.time;
            }
            else
            {
                writeTarget(ppd, note);
            }
        }
        //ELSE
        else
        {
            if (findOpcode(opcodes, hex) == -1)
            {
                printf("wrong opcode : proceeding, might break\n");
            }
            for (i=0; i < (findOpcode(opcodes, hex)); i++)
            {
                tempHex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
            }
            hex = (fgetc(dsc)) | (fgetc(dsc) << 8) | (fgetc(dsc) << 16) | (fgetc(dsc) << 24);
        }
    }

    //write last chains
    if (!firstChainL)
    {
        noteChainL.releaseTime = timeChainL[0];
        currentPos = ftell(ppd);
        fseek(ppd, chainPosL, SEEK_SET);
        writeTarget(ppd, noteChainL);
        fseek(ppd, currentPos, SEEK_SET);
    }
    if (!firstChainR)
    {
        noteChainR.releaseTime = timeChainR[0];
        currentPos = ftell(ppd);
        fseek(ppd, chainPosR, SEEK_SET);
        writeTarget(ppd, noteChainR);
        fseek(ppd, currentPos, SEEK_SET);
    }

    //PARAMETERS
    //there are parameters
    fprintf(ppd, "%c%c%c%c", 0, 0, 192, 255);

    //there are "ID" of them
    fwrite(&chart->ID, sizeof(chart->ID), 1, ppd);

    //foreach ID
    for (i=0; i<chart->ID; i++)
    {
        int tempInt = i+1;
        int len = 140;

        //write ID
        fwrite(&tempInt, sizeof(tempInt), 1, ppd);

        //Write number of characters
        len += countDigits(chart->distance.value[i]);
        len += countDigits(chart->amplitude.value[i]);
        len += countDigits(chart->frequency.value[i]);
        fwrite(&len, sizeof(len), 1, ppd);

        //Open parameters
        fprintf(ppd, "%s", "<Root>\r\n");

        //Write distance parameter
        fprintf(ppd, "%s", "  <Parameter Key=\"Distance\" Value=\"");
        fprintf(ppd, "%d", chart->distance.value[i]);
        fprintf(ppd, "%s", "\" />\r\n");

        //Write amplitude parameter
        fprintf(ppd, "%s", "  <Parameter Key=\"Amplitude\" Value=\"");
        fprintf(ppd, "%d", chart->amplitude.value[i]);
        fprintf(ppd, "%s", "\" />\r\n");

        //Write frequency parameter
        fprintf(ppd, "%s", "  <Parameter Key=\"Frequency\" Value=\"");
        fprintf(ppd, "%d", chart->frequency.value[i]);
        fprintf(ppd, "%s", "\" />\r\n");

        //Close parameters
        fprintf(ppd, "%s", "</Root>");
    }

    chart->endTime = (currentTime / (float)1000) + chart->offset;

    fclose(ppd);
    fclose(dsc);

    printf("Finished writing layer\n");

    return 0;
}

/*
write BPM.fsml file (convert DSC flying time)
*/
int writeBPM(char* scriptFilePath, Tchart* chart)
{
    FILE* bpm = NULL;
    int i = 0;

    bpm = fopen(scriptFilePath, "wb");

    if (bpm == NULL)
    {
        printf("Couldn't create BPM.fsml file\n");
        return 1;
    }

    fprintf(bpm, "%s", "<Root>\n  <Sources>\n");

    for (i=0; i<chart->flyingTimeTiming.len; i++)
    {
        float tempFloat = 0;
        tempFloat = (float)(60 * 4 * 1000) / (float)chart->flyingTime.value[i];
        tempFloat *= (float)260;
        tempFloat /= (float)240;

        fprintf(bpm, "    <Source Name=\"FlowScriptEnginePPD.FlowSourceObjects.Mark.ProcessMarkBPMWithinTimeFlowSourceObject\" FullName=\"FlowScriptEnginePPD.FlowSourceObjects.Mark.ProcessMarkBPMWithinTimeFlowSourceObject\" ID=\"");
        fprintf(bpm, "%d", i);
        fprintf(bpm, "\" ShowAll=\"True\" X=\"0\" Y=\"0\">\n");
        fprintf(bpm, "      <Value Name=\"BPM\" Value=\"");
        fprintf(bpm, "%f", tempFloat);
        fprintf(bpm, "\" />\n");
        fprintf(bpm, "      <Value Name=\"EndTime\" Value=\"");
        if (chart->flyingTime.len == i+1)
        {
            fprintf(bpm, "%f", chart->endTime);
        }
        else
        {
            fprintf(bpm, "%f", chart->flyingTimeTiming.value[i+1]);
        }
        fprintf(bpm, "\" />\n");
        fprintf(bpm, "      <Value Name=\"Handled\" Value=\"True\" />\n");
        fprintf(bpm, "      <Value Name=\"StartTime\" Value=\"");
        fprintf(bpm, "%f", chart->flyingTimeTiming.value[i]);
        fprintf(bpm, "\" />\n");
        fprintf(bpm, "    </Source>\n");
    }
    fprintf(bpm, "  </Sources>\n");
    fprintf(bpm, "  <Flows />\n");
    fprintf(bpm, "  <Comments />\n");
    fprintf(bpm, "  <Scopes />\n");
    fprintf(bpm, "</Root>\n");

    fclose(bpm);

    printf("Finished porting BPM changes\n");

    return 0;
}

/*
write data.ini file, convert bpm and endtime
*/
int writeIni(char* iniPath, Tchart* chart)
{
    FILE* ini = NULL;
    ini = fopen(iniPath, "wb");

    if (ini == NULL)
    {
        printf("Couldn't write ini file\n");
        return 1;
    }

    fprintf(ini, "[formattype]\nisini=1\n");
    fprintf(ini, "[setting]\nthumbtimestart=0\n");
    fprintf(ini, "thumbtimeend=%f\n", chart->endTime);
    fprintf(ini, "start=%f\n", chart->offset);
    fprintf(ini, "end=%f\n", chart->endTime);
    fprintf(ini, "bpm=%d\n", (int)floor((60 * 4 * 1000)/(float)chart->flyingTime.value[0]));
    fprintf(ini, "bpmstring=\n");
    fprintf(ini, "difficulty easy=\n");
    fprintf(ini, "difficulty normal=\n");
    fprintf(ini, "difficulty hard=\n");
    fprintf(ini, "difficulty extreme=\n");
    fprintf(ini, "moviecutleft=0\n");
    fprintf(ini, "moviecutright=0\n");
    fprintf(ini, "moviecuttop=0\n");
    fprintf(ini, "moviecutbottom=0\n");
    fprintf(ini, "guid=\n");
    fprintf(ini, "authorname=\n");
    fprintf(ini, "latency=0\n");

    fclose(ini);

    printf("Finished writing data.ini\n");

    return 0;
}

/*
write ppdproj file
*/
int writePpdproj(char* ppdprojPath, char* difficulty, Tchart* chart)
{
    FILE* ppdproj = NULL;
    ppdproj = fopen(ppdprojPath, "ab");

    if (ppdproj == NULL)
    {
        printf("Couldn't write ppdproj file\n");
        return 1;
    }

    fprintf(ppdproj, "[%s.layer0]Layer0\n", difficulty);
    fprintf(ppdproj, "[%s.layer0BPM]%f\n", difficulty, ((float)60/(float)chart->flyingTime.value[0] * (float)4000));
    fprintf(ppdproj, "[%s.layer0BPMOffset]0\n", difficulty);
    fprintf(ppdproj, "[%s.layer0DisplayWidth]70\n", difficulty);
    fprintf(ppdproj, "[%s.layer0Visible]1\n", difficulty);
    fprintf(ppdproj, "[%s.layer0DisplayMode]2\n", difficulty);
    fprintf(ppdproj, "[%s.layer0BeatType]2\n", difficulty);
    fprintf(ppdproj, "[%s.layer0MarkColor]-16777216\n", difficulty);
    fprintf(ppdproj, "[%s.selectedlayerindex]0\n", difficulty);
    fprintf(ppdproj, "[%s.bpm]%f\n", difficulty, ((float)60/(float)chart->flyingTime.value[0] * (float)4000));
    fprintf(ppdproj, "[%s.bpmstart]0\n", difficulty);
    fprintf(ppdproj, "[%s.lastseektime]0\n", difficulty);
    fprintf(ppdproj, "[%s.displaymode]2\n", difficulty);
    fprintf(ppdproj, "[%s.beattype]2\n", difficulty);
    fprintf(ppdproj, "[%s.bpmfixed]1\n", difficulty);
    fprintf(ppdproj, "[%s.drawtoggle]1\n", difficulty);
    fprintf(ppdproj, "[%s.showholdextent]1\n", difficulty);
    fprintf(ppdproj, "[%s.markfocus]0\n", difficulty);
    fprintf(ppdproj, "[%s.pandadisplayangle]0\n", difficulty);
    fprintf(ppdproj, "[%s.displaywidth]70\n", difficulty);
    fprintf(ppdproj, "[%s.speedscale]2\n", difficulty);
    fprintf(ppdproj, "[%s.fixeddistance]30\n", difficulty);
    fprintf(ppdproj, "[%s.antialias]0\n", difficulty);
    fprintf(ppdproj, "[%s.drawpos]1\n", difficulty);
    fprintf(ppdproj, "[%s.drawangle]1\n", difficulty);
    fprintf(ppdproj, "[%s.applymoved]0\n", difficulty);
    fprintf(ppdproj, "[%s.gridwidth]20\n", difficulty);
    fprintf(ppdproj, "[%s.gridheight]20\n", difficulty);
    fprintf(ppdproj, "[%s.gridoffsetx]0\n", difficulty);
    fprintf(ppdproj, "[%s.gridoffsety]0\n", difficulty);
    fprintf(ppdproj, "[%s.gridcolor]#8000FF\n", difficulty);
    fprintf(ppdproj, "[%s.displaygrid]1\n", difficulty);
    fprintf(ppdproj, "[%s.gcgridwidth]25\n", difficulty);
    fprintf(ppdproj, "[%s.gcgridheight]25\n", difficulty);
    fprintf(ppdproj, "[%s.gcgridheight]25\n", difficulty);
    fprintf(ppdproj, "[%s.gcgridoffsety]0\n", difficulty);
    fprintf(ppdproj, "[%s.gcgridcolor]Green\n", difficulty);
    fprintf(ppdproj, "[%s.gcdisplaygrid]0\n", difficulty);
    fprintf(ppdproj, "[%s.farness]0.5\n", difficulty);
    fprintf(ppdproj, "[%s.anglerestriction]0\n", difficulty);
    fprintf(ppdproj, "[%s.disabledscript]\n", difficulty);
    fprintf(ppdproj, "[%s.connect]1\n", difficulty);
    fprintf(ppdproj, "[%s.modlist]\n", difficulty);
    fprintf(ppdproj, "[%s.disableexpansion]0\n", difficulty);
    fprintf(ppdproj, "[%s.disableshader]0\n", difficulty);
    fprintf(ppdproj, "[memo]\n");
    fprintf(ppdproj, "[version]2\n");
    fprintf(ppdproj, "[selecteddifficulty]%s\n", difficulty);
    fprintf(ppdproj, "[disabledresources]\n");
    fprintf(ppdproj, "[timelineroworders]3,0,1,2,9,8,7,4,5,6\n");
    fprintf(ppdproj, "[timelinerowvisibilities]1,1,1,1,0,0,0,0,1,1\n");
    fprintf(ppdproj, "[timelinerowlimited]1\n");

    fclose(ppdproj);

    printf("Finished writing ppdproj file\n");

    return 0;
}

int writeEvd(char* evdPath, Tchart* chart)
{
    FILE* evd = NULL;
    int i = 0;
    
    evd = fopen(evdPath, "wb");

    if (evd == NULL)
    {
        printf("Couldn't open event file\n");
        return 1;
    }

    fprintf(evd, "%c%c%c%c", 0, 0, 0, 0); //time
    fprintf(evd, "%c", 7); //note type
    fprintf(evd, "%c", 2); //acft

    for (i=0; i<chart->flyingTimeTiming.len; i++)
    {
        float tempFloat = 0, time = 0;
        tempFloat = (float)(60 * 4 * 1000) / (float)chart->flyingTime.value[i];
        tempFloat *= (float)260;
        tempFloat /= (float)240;
        tempFloat /= 180;

        time = chart->flyingTimeTiming.value[i];

        fwrite(&(time), sizeof(time), 1, evd);
        fprintf(evd, "%c", 9);
        fwrite(&(tempFloat), sizeof(tempFloat), 1, evd);
    }

    fclose(evd);

    printf("Finished writing evd\n");

    return 0;
}

/*
copy a text file to other location (doesn't work if file has metadata)
*/
int copyTextFile(char* src, char* dest, char* mode)
{
    FILE* copy = NULL;
    FILE* paste = NULL;
    int i = 0;
    int len = 0;

    copy = fopen(src, "rb");
    paste = fopen(dest, mode);

    if ((copy == NULL) || (paste == NULL))
    {
        printf("Couldn't open file to copy paste\n");
        return 1;
    }

    fseek(copy, 0L, SEEK_END);
    len = ftell(copy);
    fseek(copy, 0L, SEEK_SET);

    char c = fgetc(copy);

    while(i < len)
    {
        fprintf(paste, "%c", c);
        c = fgetc(copy);
        i++;
    }

    fclose(copy);
    fclose(paste);

    return 0;
}

int writePPD(char* PPDPath, char* layerFilePath, char* CSInputPath, char* DivaScriptPath, char* scriptFilePath, char* basePpdPath)
{
    FILE* ppd = NULL;
    FILE* layer = NULL;
    FILE* csinput = NULL;
    FILE* divascript = NULL;
    FILE* bpm = NULL;
    FILE* event = NULL;

    int layerLen = 0;
    int csinputLen = 0;
    int divascriptLen = 0;
    int bpmLen = 0;
    int eventLen = 0;
    
    ppd = fopen(PPDPath, "wb");
    layer = fopen(layerFilePath, "rb");
    csinput = fopen(CSInputPath, "rb");
    divascript = fopen(DivaScriptPath, "rb");
    bpm = fopen(scriptFilePath, "rb");
    event = fopen(basePpdPath, "rb");

    if ((ppd == NULL) || (layer == NULL) || (csinput == NULL) || (divascript == NULL) || (bpm == NULL) || (event == NULL))
    {
        printf("couldn't open files to copy into PPD file\n");
        return 1;
    }

    //storing lengths :
    fseek(layer, 0L, SEEK_END);
    layerLen = ftell(layer);
    fseek(layer, 0L, SEEK_SET);

    fseek(csinput, 0L, SEEK_END);
    csinputLen = ftell(csinput);
    fseek(csinput, 0L, SEEK_SET);

    fseek(divascript, 0L, SEEK_END);
    divascriptLen = ftell(divascript);
    fseek(divascript, 0L, SEEK_SET);

    fseek(bpm, 0L, SEEK_END);
    bpmLen = ftell(bpm);
    fseek(bpm, 0L, SEEK_SET);

    fseek(event, 0L, SEEK_END);
    eventLen = ftell(event);
    fseek(event, 0L, SEEK_SET);

    //writing len of names
    fprintf(ppd, "PPDPACKV1%c%c%c%c%c%c", 15, 18, 11, 3, 3, 0);

    //writing names and file sizes
    fprintf(ppd, "Scripts\\CSInput");
    fwrite(&csinputLen, sizeof(csinputLen), 1, ppd);

    fprintf(ppd, "Scripts\\DivaScript");
    fwrite(&divascriptLen, sizeof(divascriptLen), 1, ppd);

    fprintf(ppd, "Scripts\\BPM");
    fwrite(&bpmLen, sizeof(bpmLen), 1, ppd);

    fprintf(ppd, "ppd");
    fwrite(&layerLen, sizeof(layerLen), 1, ppd);

    fprintf(ppd, "evd");
    fwrite(&eventLen, sizeof(eventLen), 1, ppd);

    fclose(ppd); fclose(layer); fclose(csinput); fclose(divascript); fclose(bpm); fclose(event);
    
    //copyFiles
    copyTextFile(CSInputPath, PPDPath, "ab");
    copyTextFile(DivaScriptPath, PPDPath, "ab");
    copyTextFile(scriptFilePath, PPDPath, "ab");
    copyTextFile(layerFilePath, PPDPath, "ab");
    copyTextFile(basePpdPath, PPDPath, "ab");

    //might break on windows
    remove(scriptFilePath);
    remove(layerFilePath);
    remove(basePpdPath);

    return 0;
}
