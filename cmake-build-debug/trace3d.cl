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
    float z = 0;
    int p;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, opacity = 0, angle[3], path[3];
    bool flagEnd = false;
    int state = 0;

    angle[0] = (xx - (float)side/2) / (float)distance;
    angle[1] = (yy - (float)side/2) / (float)distance;
    angle[2] = 1;

    float args[3];

    while (z < side) {
    	float newColor[3], newMedium[3], newOpacity, newIntensity, newGrad[3];
    	if ((xx < 0) || (yy < 0) || (z < 0)|| (xx >= side - 1) || (yy >= side - 1) || (z >= DEPTH - 1)){
            state = 0;
    		flagEnd = true;
    		xx = checkRange(xx, side);
    		yy = checkRange(yy, side);
            z = checkRange(z, side)
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
            newColor[0] = 1 * newMedium[0];
            newColor[1] = 1 * newMedium[1];
            newColor[2] = 1 * newMedium[2];
            newIntensity = lIntensity[p];
    	} else {
            state = 1;
            float ltf[] = {(float)((int)(xx)), (float)((int)(yy)), (float)((int)(z))};
            float lbf[] = {(float)((int)(xx)), (float)((int)(yy)) + 1, (float)((int)(z))};
            float rtf[] = {(float)((int)(xx)) + 1, (float)((int)(yy)), (float)((int)(z))};
            float rbf[] = {(float)((int)(xx)) + 1, (float)((int)(yy)) + 1, (float)((int)(z))};
            float ltb[] = {(float)((int)(xx)), (float)((int)(yy)), (float)((int)(z)) + 1};
            float lbb[] = {(float)((int)(xx)), (float)((int)(yy)) + 1, (float)((int)(z)) + 1};
            float rtb[] = {(float)((int)(xx)) + 1, (float)((int)(yy)), (float)((int)(z)) + 1};
            float rbb[] = {(float)((int)(xx)) + 1, (float)((int)(yy)) + 1, (float)((int)(z)) + 1};

    		float ratiox = getAbs(xx - (float)((int)xx));
    		float ratioy = getAbs(yy - (float)((int)yy));
    		float ratioz = getAbs(z - (float)((int)z));

            float tf[3], bf[3], tb[3], bb[3], t[3], b[3];

            newMedium[0] = fColor[getCoordinate(ltf, side) * 3];
            newMedium[1] = fColor[getCoordinate(ltf, side) * 3 + 1];
            newMedium[2] = fColor[getCoordinate(ltf, side) * 3 + 2];

			tf[0] = lColor[getCoordinate(ltf) * 3] * (1 - ratiox) + lColor[getCoordinate(rtf) * 3] * ratiox;
            tf[1] = lColor[getCoordinate(ltf) * 3 + 1] * (1 - ratiox) + lColor[getCoordinate(rtf) * 3 + 1] * ratiox;
            tf[2] = lColor[getCoordinate(ltf) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(rtf) * 3 + 2] * ratiox;
            bf[0] = lColor[getCoordinate(lbf) * 3] * (1 - ratiox) + lColor[getCoordinate(rbf)] * ratiox;
            bf[1] = lColor[getCoordinate(lbf) * 3 + 1] * (1 - ratiox) + lColor[getCoordinate(rbf) * 3 + 1] * ratiox;
            bf[2] = lColor[getCoordinate(lbf) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(rbf) * 3 + 2] * ratiox;
            tb[0] = lColor[getCoordinate(ltb) * 3] * (1 - ratiox) + lColor[getCoordinate(rtb) * 3] * ratiox;
            tb[1] = lColor[getCoordinate(ltb) * 3 + 1] * (1 - ratiox) + lColor[getCoordinate(rtb) * 3 + 1] * ratiox;
            tb[2] = lColor[getCoordinate(ltb) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(rtb) * 3 + 2] * ratiox;
            bb[0] = lColor[getCoordinate(lbb) * 3] * (1 - ratiox) + lColor[getCoordinate(rbb) * 3] * ratiox;
            bb[1] = lColor[getCoordinate(lbb) * 3 + 1] * (1 - ratiox) + lColor[getCoordinate(rbb) * 3 + 1] * ratiox;
            bb[2] = lColor[getCoordinate(lbb) * 3 + 2] * (1 - ratiox) + lColor[getCoordinate(rbb) * 3 + 2] * ratiox;
            t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
            t[1] = tf[1] * (1 - ratioz) + tb[1] * ratioz;
            t[2] = tf[2] * (1 - ratioz) + tb[2] * ratioz;
            b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
            b[1] = bf[1] * (1 - ratioz) + bb[1] * ratioz;
            b[2] = bf[2] * (1 - ratioz) + bb[2] * ratioz;
            newColor[0] = t[0] * (1 - ratioy) + b[0] * ratioy;
            newColor[1] = t[1] * (1 - ratioy) + b[1] * ratioy;
            newColor[2] = t[2] * (1 - ratioy) + b[2] * ratioy;
            newColor[0] = 1;
            newColor[1] = 1;
            newColor[2] = 1;
            newColor[0] = color[0] * medium[0];
            newColor[1] = color[1] * medium[1];
            newColor[2] = color[2] * medium[2];

    		tf[0] = grad[getCoordinate(ltf) * 3] * (1 - ratiox) + grad[getCoordinate(rtf) * 3] * ratiox;
            tf[1] = grad[getCoordinate(ltf) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rtf) * 3 + 1] * ratiox;
            tf[2] = grad[getCoordinate(ltf) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rtf) * 3 + 2] * ratiox;
            bf[0] = grad[getCoordinate(lbf) * 3] * (1 - ratiox) + grad[getCoordinate(rbf) * 3] * ratiox;
            bf[1] = grad[getCoordinate(lbf) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rbf) * 3 + 1] * ratiox;
            bf[2] = grad[getCoordinate(lbf) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rbf) * 3 + 2] * ratiox;
            tb[0] = grad[getCoordinate(ltb) * 3] * (1 - ratiox) + grad[getCoordinate(rtb) * 3] * ratiox;
            tb[1] = grad[getCoordinate(ltb) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rtb) * 3 + 1] * ratiox;
            tb[2] = grad[getCoordinate(ltb) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rtb) * 3 + 2] * ratiox;
            bb[0] = grad[getCoordinate(lbb) * 3] * (1 - ratiox) + grad[getCoordinate(rbb) * 3] * ratiox;
            bb[1] = grad[getCoordinate(lbb) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rbb) * 3 + 1] * ratiox;
            bb[2] = grad[getCoordinate(lbb) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rbb) * 3 + 2] * ratiox;
            t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
            t[1] = tf[1] * (1 - ratioz) + tb[1] * ratioz;
            t[2] = tf[2] * (1 - ratioz) + tb[2] * ratioz;
            b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
            b[1] = bf[1] * (1 - ratioz) + bb[1] * ratioz;
            b[2] = bf[2] * (1 - ratioz) + bb[2] * ratioz;
            newGrad[0] = t[0] * (1 - ratioy) + b[0] * ratioy;
            newGrad[1] = t[1] * (1 - ratioy) + b[1] * ratioy;
            newGrad[2] = t[2] * (1 - ratioy) + b[2] * ratioy;

    		newOpacity = fOpacity[getCoordinate(ltf, side)]; //(fOpacity[getCoordinate(x1, side)] * (1 - ratiox) + fOpacity[getCoordinate(y1, side)] * ratiox) * (1 - ratioy) + (fOpacity[getCoordinate(x2, side)] * (1 - ratiox) + fOpacity[getCoordinate(y2, side)] * ratiox) * ratioy;
  		    
            tf[0] = lIntensity[getCoordinate(ltf)] * (1 - ratiox) + lIntensity[getCoordinate(rtf)] * ratiox;
            bf[0] = lIntensity[getCoordinate(lbf)] * (1 - ratiox) + lIntensity[getCoordinate(rbf)] * ratiox;
            tb[0] = lIntensity[getCoordinate(ltb)] * (1 - ratiox) + lIntensity[getCoordinate(rtb)] * ratiox;
            bb[0] = lIntensity[getCoordinate(lbb)] * (1 - ratiox) + lIntensity[getCoordinate(rbb)] * ratiox;
            t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
            b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
            newIntensity = t[0] * (1 - ratioy) + b[0] * ratioy;
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
    	angle[2] = angle[2] + newGrad[2] * 100 / side;
        float square = angle[0] * angle[0] + angle[1] * angle[1] + angle[2] * angle[2];
        path[0] = angle[0] / sqrt(square);
        path[1] = angle[1] / sqrt(square);
        path[2] = angle[2] / sqrt(square);
    	xx = xx + path[0];
    	yy = yy + path[1]
    	z = z + path[1];

    }
    pixel[position * 3] = color[0];
    pixel[position * 3 + 1] = color[1];
    pixel[position * 3 + 2] = color[2];

    // pixel[position * 3] = args[0];
    // pixel[position * 3 + 1] = args[1];
    // pixel[position * 3 + 2] = args[2];
}