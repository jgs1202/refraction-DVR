//
// Created by Nozomi on 2019-05-11.
//

#include "variables.h"

const int WIDTH = 100;
const int HEIGHT = WIDTH;
const int DEPTH = WIDTH;
//const int WIDTH = 5;
//const int HEIGHT = 5;
//const int DEPTH = 5;
const int STEP = 100 / WIDTH;
const int MAX_SOURCE_SIZE = 0x100000;
const int FIELDSZ = (WIDTH * HEIGHT * DEPTH);
const int PLAINSZ = (WIDTH * HEIGHT);
int animationCount = 0;
bool updateState = true;
bool dragState = false;
int xBegin, yBegin;
int mButton;
float shiftX = 0;
float shiftY = 0;

float* grad = new float[FIELDSZ * 3];
float* fColor = new float[FIELDSZ * 3];
float* fMedium = new float[FIELDSZ * 3];
float* fOpacity = new float[FIELDSZ];
float* fRefractivity = new float[FIELDSZ];

float* lColor = new float[FIELDSZ * 3];
float* lDirection = new float[FIELDSZ * 3];
float* lIntensity = new float[FIELDSZ];

int viewW = WIDTH;
int viewH = WIDTH;

int PLATFORM_MAX = 4;
int DEVICE_MAX  = 4;

int plainY = HEIGHT * 60 / 100;
float* castPositionY = new float[WIDTH * HEIGHT * 10 * 2];
int castIndexY;

int plainZ = WIDTH * 6 / 10;

int plainX = WIDTH / 2;
float* castPositionX = new float[WIDTH * HEIGHT * 10 * 2];
int castIndexX;

float* photonIrradiance = new float[FIELDSZ];
int maxPhotons = WIDTH * WIDTH * WIDTH * 10;
float* photonPosition = new float[maxPhotons * 3];
float* photonDirection= new float[maxPhotons * 3];
float* photonGradDirection = new float[maxPhotons * 3];
float* photonColor= new float[maxPhotons * 3];
int* photonStartTime= new int[maxPhotons];
int photonEnd = 0;
//std::vector <float> surfaces;