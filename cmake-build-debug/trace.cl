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

int getCoordinate(int *x, int side){
	return x[0] + x[1] * side + x[2] * side * side;
}

__kernel void trace(__global float *grad, __global float *fColor, __global float *lColor, __global float *fOpacity, __global float *lIntensity,  __global float *pixel, int distance, int side, __global float *checker, int viewW, int viewH)
{
    int position = get_global_id(0);
    int x = getx(position, viewW);
    int y = gety(position, viewW);
    float xx = (float)x * (float)side / (float)viewW;
    float yy = (float)y * (float)side / (float)viewH;
    int z = 0, p;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, opacity = 0, angle[3];
    bool flagEnd = false, flagInt;
    int state = 0;

    angle[0] = (xx - (float)side/2) / (float)distance;
    angle[1] = (yy - (float)side/2) / (float)distance;
    angle[2] = 1;

    float args[3];

    while (z < side) {
    	float newColor[3], newMedium[3], newOpacity, newIntensity, newGrad[3];
    	flagInt = false;
    	if (integer(xx) && integer(yy)){
    		flagInt = true;
    		// xx = (float)getRound(xx);
    		// yy = (float)getRound(yy);
    	}
    	if ((xx < 0) || (yy < 0) || (xx > side - 1) || (yy > side - 1)){
            state = 0;
    		flagEnd = true;
    		xx = checkRange(xx, side);
    		yy = checkRange(yy, side);
            p = ((int)(xx) + (int)(yy) * side + z * side * side);
            newMedium[0] = fColor[3 * p];
            newMedium[1] = fColor[3 * p + 1];
            newMedium[2] = fColor[3 * p + 2];
    //       reflectivity = fRefrecttivity[p];
            newOpacity = fOpacity[p];
            if (newOpacity == 0){
                newOpacity = 1;
                newMedium[0] = 1;
                newMedium[1] = 1;
                newMedium[2] = 1;
            }
            newColor[0] = lColor[3 * p] * newMedium[0];
            newColor[1] = lColor[3 * p + 1] * newMedium[1];
            newColor[2] = fColor[3 * p + 2] * newMedium[2];
            newColor[0] = newMedium[0];
            newColor[1] = newMedium[1];
            newColor[2] = newMedium[2];

            newGrad[0] = grad[3 * p];
            newGrad[1] = grad[3 * p + 1];
            newGrad[2] = grad[3 * p + 2];
            newIntensity = lIntensity[p];
    	} else if (flagInt) {
            state = 1;
    		p = getRound(xx) + getRound(yy) * side + z * side * side;
    		newMedium[0] = fColor[3 * p];
    		newMedium[1] = fColor[3 * p + 1];
    		newMedium[2] = fColor[3 * p + 2];
    		newColor[0] = 1;//lColor[3 * p];
    		newColor[1] = 1;//lColor[3 * p + 1];
    		newColor[2] = 1;//fColor[3 * p + 2];
            newColor[0] = newColor[0] * newMedium[0];
            newColor[1] = newColor[1] * newMedium[1];
            newColor[2] = newColor[2] * newMedium[2];
    		newOpacity = fOpacity[p];
            // if (flagEnd) {
            //     opacity = 1;
            // }
    		newGrad[0] = grad[3 * p];
    		newGrad[1] = grad[3 * p + 1];
    		newGrad[2] = grad[3 * p + 2];
    		newIntensity = lIntensity[p];
    	} else {
            state = 2;
    		int x1[] = {(int)(xx), (int)(yy), z};
    		int x2[] = {(int)(xx), (int)(yy) + 1, z};
    		int y1[] = {(int)(xx) + 1, (int)(yy), z};
    		int y2[] = {(int)(xx) + 1, (int)(yy) + 1, z};

    		float ratiox = getAbs(xx - (float)((int)xx));
    		float ratioy = getAbs(yy - (float)((int)yy));
    		float vec1[3], vec2[3];

    		// vec1[0] = fColor[getCoordinate(x1, side) * 3] * (1 - ratiox) + fColor[getCoordinate(y1, side) * 3] * ratiox;
    		// vec1[1] = fColor[getCoordinate(x1, side) * 3 + 1] * (1 - ratiox) + fColor[getCoordinate(y1, side) * 3 + 1] * ratiox;
    		// vec1[2] = fColor[getCoordinate(x1, side) * 3 + 2] * (1 - ratiox) + fColor[getCoordinate(y1, side) * 3 + 2] * ratiox;
    		// vec2[0] = fColor[getCoordinate(x2, side) * 3] * (1 - ratiox) + fColor[getCoordinate(y2, side) * 3] * ratiox;
    		// vec2[1] = fColor[getCoordinate(x2, side) * 3 + 1] * (1 - ratiox) + fColor[getCoordinate(y2, side) * 3 + 1] * ratiox;
    		// vec2[2] = fColor[getCoordinate(x2, side) * 3 + 2] * (1 - ratiox) + fColor[getCoordinate(y2, side) * 3 + 2] * ratiox;
    		// newMedium[0] = vec1[0] * (1 - ratioy) + vec2[0] * ratioy;
    		// newMedium[1] = vec1[1] * (1 - ratioy) + vec2[1] * ratioy;
    		// newMedium[2] = vec1[2] * (1 - ratioy) + vec2[2] * ratioy;
            newMedium[0] = fColor[getCoordinate(x1, side) * 3];
            newMedium[1] = fColor[getCoordinate(x1, side) * 3 + 1];
            newMedium[2] = fColor[getCoordinate(x1, side) * 3 + 2];

			vec1[0] = lColor[getCoordinate(x1, side) * 3] * (1 - ratiox) + lColor[getCoordinate(y1, side) * 3] * ratiox;
    		vec1[1] = lColor[getCoordinate(x1, side) * 3 + 1] * (1 - ratiox) + lColor[getCoordinate(y1, side) * 3 + 1] * ratiox;
    		vec1[2] = lColor[getCoordinate(x1, side) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(y1, side) * 3 + 2] * ratiox;
    		vec2[0] = lColor[getCoordinate(x2, side) * 3] * (1 - ratiox) + lColor[getCoordinate(y2, side) * 3] * ratiox;
    		vec2[1] = lColor[getCoordinate(x2, side) * 3 + 1] * (1 - ratiox) + lColor[getCoordinate(y2, side) * 3 + 1] * ratiox;
    		vec2[2] = lColor[getCoordinate(x2, side) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(y2, side) * 3 + 2] * ratiox;
    		newColor[0] = vec1[0] * (1 - ratioy) + vec2[0] * ratioy;
    		newColor[1] = vec1[1] * (1 - ratioy) + vec2[1] * ratioy;
    		newColor[2] = vec1[2] * (1 - ratioy) + vec2[2] * ratioy;
            newColor[0] = 1;
            newColor[1] = 1;
            newColor[2] = 1;
            newColor[0] = newColor[0] * newMedium[0];
            newColor[1] = newColor[1] * newMedium[1];
            newColor[2] = newColor[2] * newMedium[2];

    		vec1[0] = grad[getCoordinate(x1, side) * 3] * (1 - ratiox) + grad[getCoordinate(y1, side) * 3] * ratiox;
    		vec1[1] = grad[getCoordinate(x1, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(y1, side) * 3 + 1] * ratiox;
    		vec1[2] = grad[getCoordinate(x1, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(y1, side) * 3 + 2] * ratiox;
    		vec2[0] = grad[getCoordinate(x2, side) * 3] * (1 - ratiox) + grad[getCoordinate(y2, side) * 3] * ratiox;
    		vec2[1] = grad[getCoordinate(x2, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(y2, side) * 3 + 1] * ratiox;
    		vec2[2] = grad[getCoordinate(x2, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(y2, side) * 3 + 2] * ratiox;
    		newGrad[0] = vec1[0] * (1 - ratioy) + vec2[0] * ratioy;
    		newGrad[1] = vec1[1] * (1 - ratioy) + vec2[1] * ratioy;
    		newGrad[2] = vec1[2] * (1 - ratioy) + vec2[2] * ratioy;

    		newOpacity = fOpacity[getCoordinate(x1, side)]; //(fOpacity[getCoordinate(x1, side)] * (1 - ratiox) + fOpacity[getCoordinate(y1, side)] * ratiox) * (1 - ratioy) + (fOpacity[getCoordinate(x2, side)] * (1 - ratiox) + fOpacity[getCoordinate(y2, side)] * ratiox) * ratioy;
  		    newIntensity = (lIntensity[getCoordinate(x1, side)] * (1 - ratiox) + lIntensity[getCoordinate(y1, side)] * ratiox) * (1 - ratioy) + (lIntensity[getCoordinate(x2, side)] * (1 - ratiox) + lIntensity[getCoordinate(y2, side)] * ratiox) * ratioy;

            args[0] = lColor[getCoordinate(x1, side) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(y1, side) * 3 + 2] * ratiox;
            args[1] = lColor[getCoordinate(x2, side) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(y2, side) * 3 + 2] * ratiox;
            args[2] = args[0] * (1 - ratioy) + args[1] * ratioy;
        }
        checker[position] = color[2];

    	color[0] = color[0] + medium[0] * (1 - opacity) * newOpacity * newColor[0];
    	color[1] = color[1] + medium[1] * (1 - opacity) * newOpacity * newColor[1];
    	color[2] = color[2] + medium[2] * (1 - opacity) * newOpacity * newColor[2];

        // if (z == side - 1){
        //     int max = getMax(angle);
        //     color[0] = angle[0] / angle[max];
        //     color[1] = angle[1] / angle[max];
        //     color[2] = angle[2] / angle[max];
        // }

    	medium[0] = medium[0] * newMedium[0];
    	medium[1] = medium[1] * newMedium[1];
    	medium[2] = medium[2] * newMedium[2];

    	opacity = opacity + newOpacity * (1.0 - opacity);
    	// if (flagEnd) {
    	// 	break;
    	// }
    	// if (getAbs(1 - opacity) < 0.05){
    	// 	break;
    	// }

    	angle[0] = angle[0] + newGrad[0] * 100 / side;
    	angle[1] = angle[1] + newGrad[1] * 100 / side;
    	angle[2] = angle[2] + newGrad[2] * 100;
    	xx = xx + angle[0] / angle[2];
    	yy = yy + angle[1] / angle[2];
    	z = z + 1;

    }
    pixel[position * 3] = color[0];
    pixel[position * 3 + 1] = color[1];
    pixel[position * 3 + 2] = color[2];

    // pixel[position * 3] = args[0];
    // pixel[position * 3 + 1] = args[1];
    // pixel[position * 3 + 2] = args[2];
}