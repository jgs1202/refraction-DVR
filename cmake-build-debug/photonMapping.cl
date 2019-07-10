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

__kernel void trace(__global float *grad, __global float *photonDirection, __global float *photonPosition, __global float *photonIrradiance, __global float *photonColor, __global float *fColor, __global float *fOpacity, int side, __global float *checker)
{
    int position = get_global_id(0);
    bool flagEnd = false;
    float x, y, z, dirx, diry, dirz;
    int count, state;

    for (int i=0; i<100; ++i){
        atom_add_float(&checker[i], 1);
    }

    x = photonPosition[position * 3 + 0];
    y = photonPosition[position * 3 + 1];
    z = photonPosition[position * 3 + 2];

    dirx = photonDirection[position * 3 + 0];
    dirx = photonDirection[position * 3 + 1];
    dirx = photonDirection[position * 3 + 2];

    count = 0;

   while ((!flagEnd) && (count < 1000)) {
        if ((x < 0) || (y < 0) || (z < 0)|| (x > side - 1) || (y > side - 1) || (z > side - 1)){
            state = 2;
            flagEnd = true;
            x = checkRange(x, side);
            y = checkRange(y, side);
            z = checkRange(z, side);
   //          p = ((int)(xx) + (int)(yy) * side + (int)(z) * side * side);
   //          newMedium[0] = fColor[3 * p];
   //          newMedium[1] = fColor[3 * p];
   //          newMedium[2] = fColor[3 * p];
   //          // newMedium[0] = pow(newMedium[0], 1/calcStep);
   //          // newMedium[1] = pow(newMedium[1], 1/calcStep);
   //          // newMedium[2] = pow(newMedium[2], 1/calcStep);
   //          // reflectivity = fRefrecttivity[p];
   //          newOpacity = (double)fOpacity[p];
   //          if (newOpacity == 0){
   //              newOpacity = 1;
   //              newMedium[0] = 1;
   //              newMedium[1] = 1;
   //              newMedium[2] = 1;
   //          }

   //          newColor[0] = 1 * newMedium[0];
   //          newColor[1] = 1 * newMedium[1];
   //          newColor[2] = 1 * newMedium[2];
   //          // newColor[0] = lColor[3 * p] * newMedium[0];
   //          // newColor[1] = lColor[3 * p + 1] * newMedium[1];
   //          // newColor[2] = fColor[3 * p + 2] * newMedium[2];
   //          newIntensity = lIntensity[p];
        } else {
            state = 1;
   //          ltf[0] = (int)(xx);
   //          ltf[1] = (int)(yy);
   //          ltf[2] = (int)(z);
   //          int lbf[] = {(int)(xx), (int)(yy) + 1, (int)(z)};
   //          int rtf[] = {(int)(xx) + 1, (int)(yy), (int)(z)};
   //          int rbf[] = {(int)(xx) + 1, (int)(yy) + 1, (int)(z)};
   //          int ltb[] = {(int)(xx), (int)(yy), (int)(z) + 1};
   //          int lbb[] = {(int)(xx), (int)(yy) + 1, (int)(z) + 1};
   //          int rtb[] = {(int)(xx) + 1, (int)(yy), (int)(z) + 1};
   //          int rbb[] = {(int)(xx) + 1, (int)(yy) + 1, (int)(z) + 1};

   //          float ratiox = getAbs(xx - (float)((int)xx));
   //          float ratioy = getAbs(yy - (float)((int)yy));
   //          float ratioz = getAbs(z - (float)((int)z));

   //          float tf[3], bf[3], tb[3], bb[3], t[3], b[3];

   //          newMedium[0] = fColor[getCoordinate(ltf, side) * 3];
   //          newMedium[1] = fColor[getCoordinate(ltf, side) * 3 + 1];
   //          newMedium[2] = fColor[getCoordinate(ltf, side) * 3 + 2];
   //          newMedium[0] = pow(newMedium[0], 1/calcStep);
   //          newMedium[1] = pow(newMedium[1], 1/calcStep);
   //          newMedium[2] = pow(newMedium[2], 1/calcStep);

   //          // newColor[0] = 1;
   //          // newColor[1] = 1;
   //          // newColor[2] = 1;
   //          newColor[0] = lColor[getCoordinate(ltf, side) * 3];
   //          newColor[1] = lColor[getCoordinate(ltf, side) * 3 + 1];
   //          newColor[2] = lColor[getCoordinate(ltf, side) * 3 + 2];
   //          newColor[0] = newColor[0] * newMedium[0];
   //          newColor[1] = newColor[1] * newMedium[1];
   //          newColor[2] = newColor[2] * newMedium[2];

   //          tf[0] = grad[getCoordinate(ltf, side) * 3] * (1 - ratiox) + grad[getCoordinate(rtf, side) * 3] * ratiox;
   //          tf[1] = grad[getCoordinate(ltf, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rtf, side) * 3 + 1] * ratiox;
   //          tf[2] = grad[getCoordinate(ltf, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rtf, side) * 3 + 2] * ratiox;
   //          bf[0] = grad[getCoordinate(lbf, side) * 3] * (1 - ratiox) + grad[getCoordinate(rbf, side) * 3] * ratiox;
   //          bf[1] = grad[getCoordinate(lbf, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rbf, side) * 3 + 1] * ratiox;
   //          bf[2] = grad[getCoordinate(lbf, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rbf, side) * 3 + 2] * ratiox;
   //          tb[0] = grad[getCoordinate(ltb, side) * 3] * (1 - ratiox) + grad[getCoordinate(rtb, side) * 3] * ratiox;
   //          tb[1] = grad[getCoordinate(ltb, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rtb, side) * 3 + 1] * ratiox;
   //          tb[2] = grad[getCoordinate(ltb, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rtb, side) * 3 + 2] * ratiox;
   //          bb[0] = grad[getCoordinate(lbb, side) * 3] * (1 - ratiox) + grad[getCoordinate(rbb, side) * 3] * ratiox;
   //          bb[1] = grad[getCoordinate(lbb, side) * 3 + 1] * (1 - ratiox) + grad[getCoordinate(rbb, side) * 3 + 1] * ratiox;
   //          bb[2] = grad[getCoordinate(lbb, side) * 3 + 2] * (1 - ratiox) + grad[getCoordinate(rbb, side) * 3 + 2] * ratiox;
   //          t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
   //          t[1] = tf[1] * (1 - ratioz) + tb[1] * ratioz;
   //          t[2] = tf[2] * (1 - ratioz) + tb[2] * ratioz;
   //          b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
   //          b[1] = bf[1] * (1 - ratioz) + bb[1] * ratioz;
   //          b[2] = bf[2] * (1 - ratioz) + bb[2] * ratioz;
   //          newGrad[0] = t[0] * (1 - ratioy) + b[0] * ratioy;
   //          newGrad[1] = t[1] * (1 - ratioy) + b[1] * ratioy;
   //          newGrad[2] = t[2] * (1 - ratioy) + b[2] * ratioy;

   //          newOpacity = (double)fOpacity[getCoordinate(ltf, side)]; //(fOpacity[getCoordinate(x1, side)] * (1 - ratiox) + fOpacity[getCoordinate(y1, side)] * ratiox) * (1 - ratioy) + (fOpacity[getCoordinate(x2, side)] * (1 - ratiox) + fOpacity[getCoordinate(y2, side)] * ratiox) * ratioy;
   //          newOpacity = newOpacity / (double)calcStep;

   //          tf[0] = lIntensity[getCoordinate(ltf, side)] * (1 - ratiox) + lIntensity[getCoordinate(rtf, side)] * ratiox;
   //          bf[0] = lIntensity[getCoordinate(lbf, side)] * (1 - ratiox) + lIntensity[getCoordinate(rbf, side)] * ratiox;
   //          tb[0] = lIntensity[getCoordinate(ltb, side)] * (1 - ratiox) + lIntensity[getCoordinate(rtb, side)] * ratiox;
   //          bb[0] = lIntensity[getCoordinate(lbb, side)] * (1 - ratiox) + lIntensity[getCoordinate(rbb, side)] * ratiox;
   //          t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
   //          b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
   //          newIntensity = t[0] * (1 - ratioy) + b[0] * ratioy;
        }

   //      color[0] = color[0] + medium[0] * (1 - (float)opacity) * newOpacity * newColor[0];
   //      color[1] = color[1] + medium[1] * (1 - (float)opacity) * newOpacity * newColor[1];
   //      color[2] = color[2] + medium[2] * (1 - (float)opacity) * newOpacity * newColor[2];

   //      medium[0] = medium[0] * newMedium[0];
   //      medium[1] = medium[1] * newMedium[1];
   //      medium[2] = medium[2] * newMedium[2];

   //      opacity = opacity + newOpacity * (1.0 - opacity);

   //      // if (getAbs(1 - opacity) < 0.05){
   //      //  break;
   //      // }

   //      angle[0] = angle[0] + newGrad[0] * 100 / (float)side / (float)calcStep;
   //      angle[1] = angle[1] + newGrad[1] * 100 / side / (float)calcStep;
   //      angle[2] = angle[2] + newGrad[2] * 100 / side / (float)calcStep;
   //      square = angle[0] * angle[0] + angle[1] * angle[1] + angle[2] * angle[2];
   //      path[0] = angle[0] / sqrt(square);
   //      path[1] = angle[1] / sqrt(square);
   //      path[2] = angle[2] / sqrt(square);
   //      xx = xx + path[0] / (float)calcStep;
   //      yy = yy + path[1] / (float)calcStep;
   //      z = z + path[2] / (float)calcStep;
        count = count + 1;
   //      lastAngleX = angle[0];
    }
   //  float a = 0.001;
   //  checker[position] = newOpacity;//fColor[3 * ((int)(xx) + (int)(yy) * side + (int)(z) * side * side)];
   //  // pixel[position * 3] = (lastAngleX - iniAngleX) * 10;
   //  // pixel[position * 3 + 1] = 0;//color[1];
   //  // pixel[position * 3 + 2] = (iniAngleX - lastAngleX) * 10;

   //  pixel[position * 3] = color[0];
   //  pixel[position * 3 + 1] = color[1];
   //  pixel[position * 3 + 2] = color[2];
}