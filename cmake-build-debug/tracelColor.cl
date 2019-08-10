int getx (int p, int side) {
    return (p - (int)(p / side) * side);
}

int gety (int p, int side) {
    return (int)(p / side);
}

bool integer(float x){
	if (((x - (int)(x)) == 0) || ((1 - (x - (int)(x))) == 0)){
		return true;
	} else {
		return false;
	}
}

float checkRange(float x, int side){
    float xx = x;
    if (x >= side - 1){
        xx = side - 1;
    } else if (x < 0) {
        xx = 0;
    }
    return xx;
}

int getMax(float *vec){
    int max;
    if (vec[0] > vec[1]){
        if (vec[0] > vec[2]){
            max = 0;
        } else {
            max = 2;
        }
    } else if (vec[1] >= vec[0]){
        if (vec[1] > vec[2]) {
            max = 1;
        } else {
            max = 2;
        }
    }
}

float getAbs(float x){
	float a;
	if (x < 0){
		a = -x;
	} else {
		a = x;
	}
	return a;
}

int getRound(float x){
	int a;
    if (x - (int)x < 0.5){
    	a = (int)x;
    } else {
    	a = (int)x + 1;
    }
    return a;
}

unsigned long getCoordinate(int *x, int side){
	return (x[0] + x[1] * side + x[2] * side * side);
}

__kernel void trace(__global float *fColor, __global float *lColor, __global float *fOpacity, __global float *lIntensity,  __global float *pixel, int distance, int side, __global float *checker, int viewW, int viewH, float sx, float sy)
{
    int position = get_global_id(0);
    int x = getx(position, viewW);
    int y = gety(position, viewW);
    float xx = (float)x * (float)side / (float)viewW;
    float yy = (float)y * (float)side / (float)viewH;
    float z = 0, square;
    float args[3], newColor[3], newMedium[3], newIntensity, newGrad[3];   
    int ltf[3]; 
    unsigned long p;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, angle[3], path[3], iniViewP[3], iniEndP[3], rotViewP[3], rotEndP[3];
    double opacity = 0, newOpacity;
    bool flagEnd = false;
    int state = 0, count = 0;
    float calcStep = 3;

    angle[0] = (xx - (float)side/2) / (float)distance;
    angle[1] = (yy - (float)side/2) / (float)distance;
    angle[2] = 1;

    while ((!flagEnd) && (count < 10000) && (opacity < 0.99)) {
    	if ((xx < 0) || (yy < 0) || (z < 0)|| (xx > side - 1) || (yy > side - 1) || (z > side - 1)){
            state = 2;
    		flagEnd = true;
    		xx = checkRange(xx, side);
    		yy = checkRange(yy, side);
            z = checkRange(z, side);
            p = ((int)(xx) + (int)(yy) * side + (int)(z) * side * side);
            newColor[0] = lColor[3 * p];
            newColor[1] = lColor[3 * p + 1];
            newColor[2] = lColor[3 * p + 2];
            newIntensity = lIntensity[p];
    	} else {
            state = 1;
            ltf[0] = (int)(xx);
            ltf[1] = (int)(yy);
            ltf[2] = (int)(z);
            newColor[0] = lColor[getCoordinate(ltf, side) * 3];
            newColor[1] = lColor[getCoordinate(ltf, side) * 3 + 1];
            newColor[2] = lColor[getCoordinate(ltf, side) * 3 + 2];
        }

        if ((color[0] == 0.) && (color[1] == 0) && (color[2] == 0)) {
            color[0] = newColor[0];
            color[1] = newColor[1];
            color[2] = newColor[2];
        }
        square = angle[0] * angle[0] + angle[1] * angle[1] + angle[2] * angle[2];
        path[0] = angle[0] / sqrt(square);
        path[1] = angle[1] / sqrt(square);
        path[2] = angle[2] / sqrt(square);
    	xx = xx + path[0] / (float)calcStep;
    	yy = yy + path[1] / (float)calcStep;
    	z = z + path[2] / (float)calcStep;
        count = count + 1;
    }
    checker[position] = color[0];

    pixel[position * 3] = color[0];
    pixel[position * 3 + 1] = color[1];
    pixel[position * 3 + 2] = color[2];
}