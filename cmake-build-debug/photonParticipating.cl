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

__kernel void trace(__global float *grad, __global float *photonDirection, __global float *photonGradDirection, __global float *photonPosition, __global float *photonColor, __global float *lColor, __global float *fColor, __global float *fOpacity, __global float *fRefractivity, int side, __global float *checker)
{
    int position = get_global_id(0);
    bool flagEnd = false;
    float x, y, z, dirx, diry, dirz, newMedium[3], newGrad[3], newOpacity, color[3], square, refrac;
    int count, state, calcStep, p;

    x = photonPosition[position * 3 + 0];
    y = photonPosition[position * 3 + 1];
    z = photonPosition[position * 3 + 2];

    dirx = photonDirection[position * 3 + 0];
    diry = photonDirection[position * 3 + 1];
    dirz = photonDirection[position * 3 + 2];

    float gradDirx = photonGradDirection[position * 3 + 0];
    float gradDiry = photonGradDirection[position * 3 + 1];
    float gradDirz = photonGradDirection[position * 3 + 2];

    color[0] = photonColor[0];
    color[1] = photonColor[1];
    color[2] = photonColor[2];

    count = 0;
    calcStep = 1;
    refrac = 0;

    if ((x < 0) || (y < 0) || (z < 0)|| (x > side - 1) || (y > side - 1) || (z > side - 1)){
        state = 2;
    } else {
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

        // store colot to light color buffer
        lColor[getCoordinate(ltf, side) * 3 + 0] = color[0];
        lColor[getCoordinate(ltf, side) * 3 + 1] = color[1];
        lColor[getCoordinate(ltf, side) * 3 + 2] = color[2];

        refrac = fRefractivity[getCoordinate(ltf, side)];

        newGrad[0] = newGrad[0] * 100 / (float)side / (float) calcStep / refrac;
        newGrad[1] = newGrad[1] * 100 / (float)side / (float) calcStep / refrac;
        newGrad[2] = newGrad[2] * 100 / (float)side / (float) calcStep / refrac;
        gradDirx = gradDirx + newGrad[0];
        gradDiry = gradDiry + newGrad[1];
        gradDirz = gradDirz + newGrad[2];

        // dirx = dirx / refrac;
        // diry = diry / refrac;
        // dirz = dirz / refrac;

        dirx = dirx + newGrad[0];
        diry = diry + newGrad[1];
        dirz = dirz + newGrad[2];


        // update color and direction of photon
        photonColor[3 * position + 0] = color[0] * newMedium[0] * (1 - newOpacity);
        photonColor[3 * position + 1] = color[1] * newMedium[1] * (1 - newOpacity);
        photonColor[3 * position + 2] = color[2] * newMedium[2] * (1 - newOpacity);

        // photonGradDirection[3 * position + 0] = gradDirx;
        // photonGradDirection[3 * position + 1] = gradDiry;
        // photonGradDirection[3 * position + 2] = gradDirz;

        photonDirection[3 * position + 0] = dirx;
        photonDirection[3 * position + 1] = diry;
        photonDirection[3 * position + 2] = dirz;

    }
    x = x + dirx;// + gradDirx;
    y = y + diry;// + gradDiry;
    z = z + dirz;// + gradDirz;

    photonPosition[3 * position + 0] = x;
    photonPosition[3 * position + 1] = y;
    photonPosition[3 * position + 2] = z;

    checker[position] = x;
}