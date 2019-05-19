//
// Created by Nozomi on 2019-05-11.
//

#include "variables.h"

const int WIDTH = 110;
const int HEIGHT = 110;
const int DEPTH = 110;
//const int WIDTH = 5;
//const int HEIGHT = 5;
//const int DEPTH = 5;
const int STEP = 1;
const int MAX_SOURCE_SIZE = 0x100000;
const int FIELDSZ = (WIDTH * HEIGHT * DEPTH);
const int PLAINSZ = (WIDTH * HEIGHT);

float* grad = new float[FIELDSZ * 3];
float* fColor = new float[FIELDSZ * 3];
float* fMedium = new float[FIELDSZ * 3];
float* fOpacity = new float[FIELDSZ];
float* fRefractivity = new float[FIELDSZ];

float* lColor = new float[FIELDSZ * 3];
float* lDirection = new float[FIELDSZ * 3];
float* lIntensity = new float[FIELDSZ];

int viewW = 100;
int viewH = 100;

int PLATFORM_MAX = 4;
int DEVICE_MAX  = 4;


//std::vector <float> surfaces;