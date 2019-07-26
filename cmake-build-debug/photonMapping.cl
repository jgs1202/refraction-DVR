#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

float atom_add_float(__global float* const address, const float value)
{
  uint oldval, newval, readback;
  
  *(float*)&oldval = *address;
  *(float*)&newval = (*(float*)&oldval + value);
  while ((readback = atom_cmpxchg((__global uint*)address, oldval, newval)) != oldval) {
    oldval = readback;
    *(float*)&newval = (*(float*)&oldval + value);
  }
  return *(float*)&oldval;
}
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

__kernel void trace(__global float *grad, __global float *photonDirection, __global float *photonPosition, __global float *photonIrradiance, __global float *photonColor, __global float *fColor, __global float *initialColor, __global float *fOpacity, int side, __global float *checker, float intensity)
{
    int position = get_global_id(0);
    bool flagEnd = false;
    float x, y, z, dirx, diry, dirz, newMedium[3], newGrad[3], newOpacity, color[3], square;
    int count, state, calcStep, p;

    for (int i=0; i<100; ++i){
        atom_add_float(&checker[i], 1);
    }

    x = photonPosition[position * 3 + 0];
    y = photonPosition[position * 3 + 1];
    z = photonPosition[position * 3 + 2];

    dirx = photonDirection[position * 3 + 0];
    dirx = photonDirection[position * 3 + 1];
    dirx = photonDirection[position * 3 + 2];

    color[0] = initialColor[0];
    color[1] = initialColor[1];
    color[2] = initialColor[2];

    count = 0;
    calcStep = 1;

   while ((!flagEnd) && (count < 1000)) {
        if ((x < 0) || (y < 0) || (z < 0)|| (x > side - 1) || (y > side - 1) || (z > side - 1)){
            state = 2;
            flagEnd = true;
            x = checkRange(x, side);
            y = checkRange(y, side);
            z = checkRange(z, side);
            p = ((int)(x) + (int)(y) * side + (int)(z) * side * side);
            newMedium[0] = fColor[3 * p];
            newMedium[1] = fColor[3 * p];
            newMedium[2] = fColor[3 * p];
            newMedium[0] = pow(newMedium[0], 1/calcStep);
            newMedium[1] = pow(newMedium[1], 1/calcStep);
            newMedium[2] = pow(newMedium[2], 1/calcStep);
            // reflectivity = fRefrecttivity[p];
            newOpacity = fOpacity[p];
        } else {
            // get x,y,z
            // get opacity and gradient and color of current position
            // update photon's color
            // store values
            // update x y z

            state = 1;
            int ltf[] = {(int)(x), (int)(y), (int)(z)};
            int lbf[] = {(int)(x), (int)(y) + 1, (int)(z)};
            int rtf[] = {(int)(x) + 1, (int)(y), (int)(z)};
            int rbf[] = {(int)(x) + 1, (int)(y) + 1, (int)(z)};
            int ltb[] = {(int)(x), (int)(y), (int)(z) + 1};
            int lbb[] = {(int)(x), (int)(y) + 1, (int)(z) + 1};
            int rtb[] = {(int)(x) + 1, (int)(y), (int)(z) + 1};
            int rbb[] = {(int)(x) + 1, (int)(y) + 1, (int)(z) + 1};

            float ratiox = getAbs(x - (float)((int)x));
            float ratioy = getAbs(y - (float)((int)y));
            float ratioz = getAbs(z - (float)((int)z));

            float tf[3], bf[3], tb[3], bb[3], t[3], b[3];

            newMedium[0] = fColor[getCoordinate(ltf, side) * 3];
            newMedium[1] = fColor[getCoordinate(ltf, side) * 3 + 1];
            newMedium[2] = fColor[getCoordinate(ltf, side) * 3 + 2];
            newMedium[0] = pow(newMedium[0], 1/calcStep);
            newMedium[1] = pow(newMedium[1], 1/calcStep);
            newMedium[2] = pow(newMedium[2], 1/calcStep);

            tf[0] = grad[getCoordinate(ltf, side) * 3] * (1 - ratiox) + grad[getCoordinate(rtf, side) * 3] * ratiox;
            tf[1] = grad[getCoordinate(ltf, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rtf, side) * 3 + 1] * ratiox;
            tf[2] = grad[getCoordinate(ltf, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rtf, side) * 3 + 2] * ratiox;
            bf[0] = grad[getCoordinate(lbf, side) * 3] * (1 - ratiox) + grad[getCoordinate(rbf, side) * 3] * ratiox;
            bf[1] = grad[getCoordinate(lbf, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rbf, side) * 3 + 1] * ratiox;
            bf[2] = grad[getCoordinate(lbf, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rbf, side) * 3 + 2] * ratiox;
            tb[0] = grad[getCoordinate(ltb, side) * 3] * (1 - ratiox) + grad[getCoordinate(rtb, side) * 3] * ratiox;
            tb[1] = grad[getCoordinate(ltb, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rtb, side) * 3 + 1] * ratiox;
            tb[2] = grad[getCoordinate(ltb, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rtb, side) * 3 + 2] * ratiox;
            bb[0] = grad[getCoordinate(lbb, side) * 3] * (1 - ratiox) + grad[getCoordinate(rbb, side) * 3] * ratiox;
            bb[1] = grad[getCoordinate(lbb, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rbb, side) * 3 + 1] * ratiox;
            bb[2] = grad[getCoordinate(lbb, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rbb, side) * 3 + 2] * ratiox;
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
            newOpacity = newOpacity / calcStep;
        }

        color[0] = color[0] * newMedium[0] * (1 - newOpacity);
        color[1] = color[1] * newMedium[1] * (1 - newOpacity);
        color[2] = color[2] * newMedium[2] * (1 - newOpacity);

        if (count != 0) {
            dirx = dirx + grad[0] * 100 / (float)side / (float) calcStep;
            diry = diry + grad[1] * 100 / (float)side / (float) calcStep;
            dirz = dirz + grad[2] * 100 / (float)side / (float) calcStep;
        }

        //store values
        p = ((int)(x) + (int)(y) * side + (int)(z) * side * side);
        // atom_add_float(&photonIrradiance[p], intensity);
        // atom_add_float(&photonColor[3 * p + 0], color[0]);
        // atom_add_float(&photonColor[3 * p + 1], color[1]);
        // atom_add_float(&photonColor[3 * p + 2], color[2]);

        if ((color[0] < 0.01) && (color[1] < 0.01) && (color[2] < 0.01)){
         flagEnd = true;
        }
        square = dirx * dirx + diry * diry + dirz * dirz;
        x = x + dirx / sqrt(square) / (float)calcStep;
        y = y + diry / sqrt(square) / (float)calcStep;
        z = z + dirz / sqrt(square) / (float)calcStep;
        count = count + 1;
    }
}