#pragma once
#include <SDL3/SDL.h>
#include "ArrayList.h"


typedef struct
{
	int distance;
	int amplitude;
	int frequency;
	int flyingTime;
}DTP_Parameter;


typedef struct
{
	int time;
	float timing;
}DTP_FlyingTime;


typedef struct
{
	Sint32 ID;
    bool isHold;
    bool isSlideL;
    bool isSlideR;
    float offset;
    float endTime;
    ArrayList* parameters;
    ArrayList* flyingTimes;
}DTP_Chart;


bool DTP_WriteLayer(const char* DSCPath, const char* layerFilePath, DTP_Chart* chart);

bool DTP_WriteIni(const char* iniPath, DTP_Chart* chart);

bool DTP_WritePpdproj(const char* ppdprojPath, const char* difficulty, DTP_Chart* chart);

bool DTP_WriteEvd(const char* evdPath, DTP_Chart* chart);

bool DTP_WritePPD(const char* PPDPath, const char* layerFilePath, const char* basePpdPath);

bool DTP_WriteData(unsigned char mem[], unsigned int size, const char* path);
