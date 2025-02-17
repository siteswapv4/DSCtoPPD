#include "functions.h"
#include "data.h"

typedef enum
{
    DSC_OPCODE_INVALID = -1,
    DSC_OPCODE_END = 0,
    DSC_OPCODE_TIME = 1,
    DSC_OPCODE_TARGET = 6,
    DSC_OPCODE_START = 25,
    DSC_OPCODE_BARTIMESET = 28,
    DSC_OPCODE_FLYINGTIME = 58,
    DSC_OPCODE_MAX = 107
}DSC_Opcode;


typedef struct
{
	Sint32 ID;
    float time;
    float xpos;
    float ypos;
    float angle;
    Sint8 type;
    float releaseTime;
}DTP_Note;


const int DSC_OPCODE_LEN[DSC_OPCODE_MAX] = {0, 1, 4, 2, 2, 2, 7, 4, 2, 6, 2, 1, 6, 2, 1, 1, 3, 2, 3, 5, 5, 4, 4, 5, 2, 0, 2, 4, 2, 2, 1, 21, 0, 3, 2, 5, 1, 1, 7, 1, 1, 2, 1, 2, 1, 2, 3, 3, 1, 2, 2, 3, 6, 6, 1, 1, 2, 3, 1, 2, 2, 4, 4, 1, 2, 1, 2, 1, 1, 3, 3, 3, 2, 1, 9, 3, 2, 4, 2, 3, 2, 24, 1, 2, 1, 3, 1, 3, 4, 1, 2, 6, 3, 2, 3, 3, 4, 1, 1, 3, 3, 4, 1, 3, 3, 8, 2};


const char* PPD_HEADER = "PPD";


bool DTP_IsDSC(SDL_IOStream* dsc)
{
    char header[5] = {33, 9, 5, 20, 0};
    char actualHeader[5] = {0};
    Sint64 position = SDL_TellIO(dsc);

    SDL_SeekIO(dsc, 0L, SDL_IO_SEEK_SET);

    SDL_ReadIO(dsc, actualHeader, 4);

    SDL_SeekIO(dsc, position, SDL_IO_SEEK_SET);
    
    return !SDL_strncmp(header, actualHeader, 4);
}


Sint32 DTP_GetOpcode(SDL_IOStream* dsc)
{
    Sint32 opcode;

    if (SDL_ReadIO(dsc, &opcode, sizeof(Sint32)) != sizeof(Sint32)) {
        return DSC_OPCODE_INVALID;
    }

    if (opcode > DSC_OPCODE_MAX - 1) {
        return DSC_OPCODE_INVALID;
    }

    return opcode;
}



void DTP_ConvertTarget(SDL_IOStream* dsc, DTP_Note* note, DTP_Chart* chart, float currentTime)
{
	Sint32 value = 0;
	DTP_Parameter* parameter = SDL_malloc(sizeof(DTP_Parameter));

    note->time = 0;
    note->xpos = 0;
    note->ypos = 0;
    note->angle = 0;
    note->type = 0;
    note->releaseTime = 0;
    note->ID = 0;

    note->time = currentTime;
    if (chart->flyingTimes->len > 0)
    {
    	DTP_FlyingTime* flyingTime = chart->flyingTimes->data[chart->flyingTimes->len - 1];
    	
        note->time += flyingTime->time;
		parameter->flyingTime = flyingTime->time;
    }
	else
	{
		SDL_Log("No flying time but target opcode, possibly missing an opcode");
		note->time += 1500;
		parameter->flyingTime = 1500;
	}
	
    note->time /= 1000.0f;
    note->time += chart->offset;

	SDL_ReadIO(dsc, &value, sizeof(Sint32));
    switch (value)
    {
        case 0:
        case 18:
            note->type = 3;
            break;
        case 1:
        case 19:
            note->type = 2;
            break;
        case 2:
        case 20:
            note->type = 1;
            break;
        case 3:
        case 21:
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
        case 23:
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
        case 24:
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

	SDL_ReadIO(dsc, &value, sizeof(Sint32));
    note->xpos = (float)value * (800.0f / 480000.0f);

	SDL_ReadIO(dsc, &value, sizeof(Sint32));
    note->ypos = (float)value * (450.0f / 270000.0f);

	SDL_ReadIO(dsc, &value, sizeof(Sint32));
    note->angle = (float)value / 1000.0f;

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

    note->angle *= SDL_PI_F / 180.0f;

	SDL_ReadIO(dsc, &value, sizeof(Sint32));
	parameter->distance = value;
	
	SDL_ReadIO(dsc, &value, sizeof(Sint32));
	parameter->amplitude = value;
	
	SDL_ReadIO(dsc, &value, sizeof(Sint32));
	if (value % 2 == 0)
	{
		parameter->frequency = -value;
	}
	else
	{
		parameter->frequency = value;
	}
	
	arrayListAdd(chart->parameters, parameter);

    if (chart->isHold)
    {
        note->releaseTime = note->time + 0.01f;
    }

    chart->ID++;
    note->ID = chart->ID;

    chart->isHold = false;
}


void DTP_WriteTarget(SDL_IOStream* ppd, DTP_Note note)
{
	SDL_WriteIO(ppd, &note.time, sizeof(float));
	SDL_WriteIO(ppd, &note.xpos, sizeof(float));
	SDL_WriteIO(ppd, &note.ypos, sizeof(float));
	SDL_WriteIO(ppd, &note.angle, sizeof(float));
	SDL_WriteIO(ppd, &note.type, sizeof(Sint8));

    if (note.releaseTime != 0)
    {
    	SDL_WriteIO(ppd, &note.releaseTime, sizeof(float));
    }
    
    SDL_WriteIO(ppd, &note.ID, sizeof(Sint32));
}


bool DTP_WriteLayer(const char* DSCPath, const char* layerFilePath, DTP_Chart* chart)
{
    Sint32 opcode;
    Sint32 temp;
    float currentTime = 0;
    float timeChainL[2] = {0};
    float timeChainR[2] = {0};
    DTP_Note noteChainL;
    DTP_Note noteChainR;
    bool firstChainL = true;
    bool firstChainR = true;
    bool chainL = false;
    bool chainR = false;
    int chainPosL = 0;
    int chainPosR = 0;
    int currentPos = 0;
    SDL_IOStream* dsc = NULL;
    SDL_IOStream* ppd = NULL;

    if ((dsc = SDL_IOFromFile(DSCPath, "rb")) == NULL)
    {
    	SDL_Log("Error when opening DSC file");
        return false;
    }
    
    if ((ppd = SDL_IOFromFile(layerFilePath, "wb")) == NULL)
    {
    	SDL_Log("Couldn't create ppd later file");
        return false;
    }

	SDL_WriteIO(ppd, PPD_HEADER, SDL_strlen(PPD_HEADER));

	if (!DTP_IsDSC(dsc))
	{
		SDL_Log("Not a DSC file (wrong header)");
		return false;
	}
	
    SDL_SeekIO(dsc, 4L, SDL_IO_SEEK_SET);

	if ((opcode = DTP_GetOpcode(dsc)) == DSC_OPCODE_INVALID)
	{
		goto invalidOpcode;
	}

    while ((opcode != DSC_OPCODE_END) && (opcode != DSC_OPCODE_START))
    {
        if (opcode == DSC_OPCODE_TIME)
        {
			SDL_ReadIO(dsc, &temp, sizeof(Sint32));
            currentTime = (float)temp / 100000.0f;
        }
        else
        {
			SDL_SeekIO(dsc, DSC_OPCODE_LEN[opcode] * sizeof(Sint32), SDL_IO_SEEK_CUR);
        }
        
        if ((opcode = DTP_GetOpcode(dsc)) == DSC_OPCODE_INVALID)
        {
        	goto invalidOpcode;
        }
    }

    if (opcode == DSC_OPCODE_START)
    {
        chart->offset = (currentTime * -1);
    }

    chart->offset -= 0.06f; // somehow fixes latency ?

    SDL_SeekIO(dsc, 4L, SDL_IO_SEEK_SET);
    currentTime = 0;

	if ((opcode = DTP_GetOpcode(dsc)) == DSC_OPCODE_INVALID)
	{
		goto invalidOpcode;
	}

    while (opcode != DSC_OPCODE_END)
    {
    	switch (opcode)
    	{
        	case DSC_OPCODE_TIME:
		        SDL_ReadIO(dsc, &temp, sizeof(Sint32));
		        currentTime = (float)temp / 100.0f;
		        chart->isSlideL = false;
		        chart->isSlideR = false;
		        break;
		        
		    case DSC_OPCODE_FLYINGTIME:
				SDL_ReadIO(dsc, &temp, sizeof(Sint32));
				
		        if ((chart->flyingTimes->len == 0) || (((DTP_FlyingTime*)chart->flyingTimes->data[chart->flyingTimes->len - 1])->time != temp))
		        {
		        	DTP_FlyingTime* flyingTime = SDL_malloc(sizeof(DTP_FlyingTime));
		        	
		        	flyingTime->time = temp;
		        	flyingTime->timing = currentTime / 1000.0f + chart->offset - 0.000001f;
		            
		            arrayListAdd(chart->flyingTimes, flyingTime);
		        }
		        break;
        
        	case DSC_OPCODE_TARGET:
            	DTP_Note note;
            	DTP_ConvertTarget(dsc, &note, chart, currentTime);

		        if (note.type == 69)
		        {
		            float gap1 = note.time - timeChainL[0];
		            float gap2 = timeChainL[0] - timeChainL[1];

		            if ((firstChainL) || ((!chainL) && (!((gap1 + 0.01f > gap2) && (gap1 - 0.01f < gap2)))))
		            {
		                if (!firstChainL)
		                {
		                    noteChainL.releaseTime = timeChainL[0];
		                    currentPos = SDL_TellIO(ppd);
		                    SDL_SeekIO(ppd, chainPosL, SDL_IO_SEEK_SET);
		                    DTP_WriteTarget(ppd, noteChainL);
		                    SDL_SeekIO(ppd, currentPos, SDL_IO_SEEK_SET);
		                }
		                chainPosL = SDL_TellIO(ppd);
		                DTP_WriteTarget(ppd, note);
		                noteChainL = note;
		                firstChainL = false;
		                chainL = true;
		            }
		            else
		            {
		                chainL = false;
		                SDL_free(chart->parameters->data[chart->parameters->len - 1]);
		                chart->parameters->len--;
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
		            if ((firstChainR) || ((!chainR) && (!((gap1 + 0.01f > gap2) && (gap1 -0.01f < gap2)))))
		            {
		                if (!firstChainR)
		                {
		                    noteChainR.releaseTime = timeChainR[0];
		                    currentPos = SDL_TellIO(ppd);
		                    SDL_SeekIO(ppd, chainPosR, SDL_IO_SEEK_SET);
		                    DTP_WriteTarget(ppd, noteChainR);
		                    SDL_SeekIO(ppd, currentPos, SDL_IO_SEEK_SET);
		                }
		                chainPosR = SDL_TellIO(ppd);
		                DTP_WriteTarget(ppd, note);
		                noteChainR = note;
		                firstChainR = false;
		                chainR = true;
		            }
		            else
		            {
		                chainR = false;
		                SDL_free(chart->parameters->data[chart->parameters->len - 1]);
		                chart->parameters->len--;
		                chart->ID--;
		            }
		            timeChainR[1] = timeChainR[0];
		            timeChainR[0] = note.time;
		        }
		        else
		        {
		            DTP_WriteTarget(ppd, note);
		        }
        		break;
        		
        	default:
        		SDL_SeekIO(dsc, DSC_OPCODE_LEN[opcode] * sizeof(Sint32), SDL_IO_SEEK_CUR);
        		break;
        }
        
        if ((opcode = DTP_GetOpcode(dsc)) == DSC_OPCODE_INVALID)
		{
			goto invalidOpcode;
		}
    }

    if (!firstChainL)
    {
        noteChainL.releaseTime = timeChainL[0];
        currentPos = SDL_TellIO(ppd);
        SDL_SeekIO(ppd, chainPosL, SDL_IO_SEEK_SET);
        DTP_WriteTarget(ppd, noteChainL);
        SDL_SeekIO(ppd, currentPos, SDL_IO_SEEK_SET);
    }
    if (!firstChainR)
    {
        noteChainR.releaseTime = timeChainR[0];
        currentPos = SDL_TellIO(ppd);
        SDL_SeekIO(ppd, chainPosR, SDL_IO_SEEK_SET);
        DTP_WriteTarget(ppd, noteChainR);
        SDL_SeekIO(ppd, currentPos, SDL_IO_SEEK_SET);
    }

	Uint8 parameterHeader[4] = {0, 0, 192, 255};
	SDL_WriteIO(ppd, parameterHeader, 4);

    SDL_WriteIO(ppd, &chart->ID, sizeof(chart->ID));

    for (int i = 0; i < chart->ID; i++)
    {
    	DTP_Parameter* parameter = chart->parameters->data[i];
        int tempInt = i + 1;
		char* parameterString;

        SDL_WriteIO(ppd, &tempInt, sizeof(tempInt));

        SDL_asprintf(&parameterString, "<Root>\r\n  <Parameter Key=\"Distance\" Value=\"%d\" />\r\n  <Parameter Key=\"Amplitude\" Value=\"%d\" />\r\n  <Parameter Key=\"Frequency\" Value=\"%d\" />\r\n  <Parameter Key=\"FlyingTime\" Value=\"%d\" />\r\n</Root>", parameter->distance, parameter->amplitude, parameter->frequency, parameter->flyingTime);

        Sint32 len = SDL_strlen(parameterString);
        SDL_WriteIO(ppd, &len, sizeof(Sint32));
        SDL_WriteIO(ppd, parameterString, len);
        
        SDL_free(parameterString);
    }

    chart->endTime = (currentTime / 1000.0f) + chart->offset;

    SDL_CloseIO(ppd);
    SDL_CloseIO(dsc);

    SDL_Log("Finished writing layer\n");

    return true;
    
invalidOpcode:
	SDL_Log("Invalid opcode found in file, cannot proceed");
	return false;
}


bool DTP_WriteIni(const char* iniPath, DTP_Chart* chart)
{
    SDL_IOStream* ini;
    DTP_FlyingTime* flyingTime;
    
    if (chart->flyingTimes->len == 0)
    {
    	return false;
    }
    
	flyingTime = chart->flyingTimes->data[0];
    
    if ((ini = SDL_IOFromFile(iniPath, "wb")) == NULL)
    {
        SDL_Log("Couldn't write ini file");
        return false;
    }

    SDL_IOprintf(ini, "[formattype]\r\nisini=1\r\n");
    SDL_IOprintf(ini, "[setting]\r\nthumbtimestart=0\r\n");
    SDL_IOprintf(ini, "thumbtimeend=%f\r\n", chart->endTime);
    SDL_IOprintf(ini, "start=%f\r\n", chart->offset);
    SDL_IOprintf(ini, "end=%f\r\n", chart->endTime);
    SDL_IOprintf(ini, "bpm=%d\r\n", (int)SDL_floor(240000.0f / (float)flyingTime->time));
    SDL_IOprintf(ini, "bpmstring=\r\n");
    SDL_IOprintf(ini, "difficulty easy=\r\n");
    SDL_IOprintf(ini, "difficulty normal=\r\n");
    SDL_IOprintf(ini, "difficulty hard=\r\n");
    SDL_IOprintf(ini, "difficulty extreme=\r\n");
    SDL_IOprintf(ini, "moviecutleft=0\r\n");
    SDL_IOprintf(ini, "moviecutright=0\r\n");
    SDL_IOprintf(ini, "moviecuttop=0\r\n");
    SDL_IOprintf(ini, "moviecutbottom=0\r\n");
    SDL_IOprintf(ini, "guid=\r\n");
    SDL_IOprintf(ini, "authorname=\r\n");
    SDL_IOprintf(ini, "latency=0\r\n");

    SDL_CloseIO(ini);

    SDL_Log("Finished writing data.ini");

    return 0;
}


bool DTP_WritePpdproj(const char* ppdprojPath, const char* difficulty, DTP_Chart* chart)
{
    SDL_IOStream* ppdproj;
    DTP_FlyingTime* flyingTime;
    
    if (chart->flyingTimes->len == 0)
    {
    	return false;
    }
    
	flyingTime = chart->flyingTimes->data[0];
	
    if ((ppdproj = SDL_IOFromFile(ppdprojPath, "ab")) == NULL)
    {
        SDL_Log("Couldn't write ppdproj file");
        return false;
    }

    SDL_IOprintf(ppdproj, "[%s.layer0]Layer0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.layer0BPM]%f\n", difficulty, (240000.0f / (float)flyingTime->time));
    SDL_IOprintf(ppdproj, "[%s.layer0BPMOffset]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.layer0DisplayWidth]70\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.layer0Visible]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.layer0DisplayMode]2\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.layer0BeatType]2\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.layer0MarkColor]-16777216\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.selectedlayerindex]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.bpm]%f\n", difficulty, (240000.0f / (float)flyingTime->time));
    SDL_IOprintf(ppdproj, "[%s.bpmstart]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.lastseektime]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.displaymode]2\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.beattype]2\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.bpmfixed]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.drawtoggle]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.showholdextent]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.markfocus]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.pandadisplayangle]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.displaywidth]70\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.speedscale]2\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.fixeddistance]30\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.antialias]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.drawpos]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.drawangle]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.applymoved]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gridwidth]20\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gridheight]20\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gridoffsetx]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gridoffsety]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gridcolor]#8000FF\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.displaygrid]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gcgridwidth]25\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gcgridheight]25\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gcgridheight]25\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gcgridoffsety]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gcgridcolor]Green\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.gcdisplaygrid]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.farness]0.5\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.anglerestriction]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.disabledscript]\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.connect]1\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.modlist]\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.disableexpansion]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[%s.disableshader]0\n", difficulty);
    SDL_IOprintf(ppdproj, "[memo]\n");
    SDL_IOprintf(ppdproj, "[version]2\n");
    SDL_IOprintf(ppdproj, "[selecteddifficulty]%s\n", difficulty);
    SDL_IOprintf(ppdproj, "[disabledresources]\n");
    SDL_IOprintf(ppdproj, "[timelineroworders]3,0,1,2,9,8,7,4,5,6\n");
    SDL_IOprintf(ppdproj, "[timelinerowvisibilities]1,1,1,1,0,0,0,0,1,1\n");
    SDL_IOprintf(ppdproj, "[timelinerowlimited]1\n");

    SDL_CloseIO(ppdproj);

    SDL_Log("Finished writing ppdproj file");

    return true;
}

bool DTP_WriteEvd(const char* evdPath, DTP_Chart* chart)
{
    SDL_IOStream* evd;
    
    if ((evd = SDL_IOFromFile(evdPath, "wb")) == NULL)
    {
        SDL_Log("Couldn't open event file");
        return false;
    }

    SDL_IOprintf(evd, "%c%c%c%c", 0, 0, 0, 0); //time
	SDL_IOprintf(evd, "%c%c%c%c%c%c%c%c%c%c%c", 8, 3, 0, 1, 2, 7, 4, 5, 6, 8, 9); // Note order
	
	SDL_IOprintf(evd, "%c%c%c%c", 0, 0, 0, 0); //time
    SDL_IOprintf(evd, "%c", 7); //note type
    SDL_IOprintf(evd, "%c", 2); //acft

    for (int i = 0; i < chart->flyingTimes->len; i++)
    {
    	DTP_FlyingTime* flyingTime = chart->flyingTimes->data[i];
    	
        float tempFloat = 0;
        float time = 0;
        tempFloat = (60.0f * 4.0f * 1000.0f) / (float)flyingTime->time;
        tempFloat *= 260.0f;
        tempFloat /= 240.0f;
        tempFloat /= 180.0f;

        time = flyingTime->timing;

        SDL_WriteIO(evd, &time, sizeof(float));
        SDL_IOprintf(evd, "%c", 9);
        SDL_WriteIO(evd, &tempFloat, sizeof(float));
    }

    SDL_CloseIO(evd);

    SDL_Log("Finished writing evd");

    return true;
}


bool DTP_AppendToStream(SDL_IOStream* destination, SDL_IOStream* source)
{
	Sint64 start = SDL_TellIO(source);
	SDL_SeekIO(source, 0L, SDL_IO_SEEK_END);
	Sint64 len = SDL_TellIO(source);

	char* buffer = SDL_malloc(len);
	
	SDL_SeekIO(source, 0L, SDL_IO_SEEK_SET);
	SDL_ReadIO(source, buffer, len);
	SDL_WriteIO(destination, buffer, len);
	
	SDL_free(buffer);
	
	SDL_SeekIO(source, start, SDL_IO_SEEK_SET);
	
	return true;
}


bool DTP_WritePPD(const char* PPDPath, const char* layerFilePath, const char* basePpdPath)
{
    SDL_IOStream* ppd, *layer, *csinput, *divascript, *bpm, *event;

    int layerLen = 0;
    int csinputLen = CSInput_fsml_len;
    int divascriptLen = DivaScript_fsml_len;
    int bpmLen = BPM_fsml_len;
    int eventLen = 0;
    
    ppd = SDL_IOFromFile(PPDPath, "wb");
    layer = SDL_IOFromFile(layerFilePath, "rb");
    csinput = SDL_IOFromConstMem(CSInput_fsml, CSInput_fsml_len);
    divascript = SDL_IOFromConstMem(DivaScript_fsml, DivaScript_fsml_len);
    bpm = SDL_IOFromConstMem(BPM_fsml, BPM_fsml_len);
    event = SDL_IOFromFile(basePpdPath, "rb");

    if ((ppd == NULL) || (layer == NULL) || (csinput == NULL) || (divascript == NULL) || (bpm == NULL) || (event == NULL))
    {
        SDL_Log("%s", SDL_GetError());
        return false;
    }

    SDL_SeekIO(layer, 0L, SDL_IO_SEEK_END);
    layerLen = SDL_TellIO(layer);
    SDL_SeekIO(layer, 0L, SDL_IO_SEEK_SET);

    SDL_SeekIO(event, 0L, SDL_IO_SEEK_END);
    eventLen = SDL_TellIO(event);
    SDL_SeekIO(event, 0L, SDL_IO_SEEK_SET);

    SDL_IOprintf(ppd, "PPDPACKV1%c%c%c%c%c%c", 15, 18, 11, 3, 3, 0);

    SDL_IOprintf(ppd, "Scripts\\CSInput");
    SDL_WriteIO(ppd, &csinputLen, sizeof(int));

    SDL_IOprintf(ppd, "Scripts\\DivaScript");
    SDL_WriteIO(ppd, &divascriptLen, sizeof(int));

    SDL_IOprintf(ppd, "Scripts\\BPM");
    SDL_WriteIO(ppd, &bpmLen, sizeof(int));

    SDL_IOprintf(ppd, "ppd");
    SDL_WriteIO(ppd, &layerLen, sizeof(int));

    SDL_IOprintf(ppd, "evd");
    SDL_WriteIO(ppd, &eventLen, sizeof(int));

    DTP_AppendToStream(ppd, csinput);
    DTP_AppendToStream(ppd, divascript);
    DTP_AppendToStream(ppd, bpm);
    DTP_AppendToStream(ppd, layer);
    DTP_AppendToStream(ppd, event);
	
	SDL_CloseIO(ppd);
	SDL_CloseIO(csinput);
	SDL_CloseIO(divascript);
	SDL_CloseIO(bpm);
	SDL_CloseIO(layer);
	SDL_CloseIO(event);

    SDL_RemovePath(layerFilePath);
   	SDL_RemovePath(basePpdPath);
   	
   	SDL_Log("Finished writing PPD");

    return true;
}


bool DTP_WriteData(unsigned char mem[], unsigned int size, const char* path)
{
	SDL_IOStream* source;
	SDL_IOStream* destination;
	
	if ((source = SDL_IOFromConstMem(mem, size)) == NULL)
	{
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	
	if ((destination = SDL_IOFromFile(path, "wb")) == NULL)
	{
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	
	DTP_AppendToStream(destination, source);
	
	SDL_CloseIO(source);
	SDL_CloseIO(destination);
	
	return true;	
}
