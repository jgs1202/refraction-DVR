void addVec(float *a, float *b, float *c){
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
}

void subVec(float *a, float *b, float *c){
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

void multiVec(float *a, float *b, float *c){
    c[0] = a[0] * b[0];
    c[1] = a[1] * b[1];
    c[2] = a[2] * b[2];
}

void divVec(float *a, float *b, float *c){
    c[0] = a[0] / b[0];
    c[1] = a[1] / b[1];
    c[2] = a[2] / b[2];
}

void multiFloat(float *a, float b, float *c){
    c[0] = a[0] * b;
    c[1] = a[1] * b;
    c[2] = a[2] * b;
}

void divFloat(float *a, float b, float *c){
    c[0] = a[0] / b;
    c[1] = a[1] / b;
    c[2] = a[2] / b;
}

int checkMinus(float x){
    if (x < 0) {
        x = 0;
    }
    return (int)x;
}                                                                                                                                                                              

int getx (int p, int side) {
    return (p - (int)(p / side) * side);
}

int gety (int p, int side) {
    return (int)(p / side);
}

float checkRange(float x, int side){
    if (x > side - 1){
        x = side - 1;
    } else if (x < 0) {
        x = 0;
    }
    return x;
}

int checkRangeInt(int x, int side){
    if (x > side - 1){
        x = side - 1;
    } else if (x < 0) {
        x = 0;
    }
    return x;
}

int coordinate(int *x, int side){
    return x[0] + x[1] * side;
}

__kernel void lightSource(__global float *grad, __global float *fColor, __global float *fMedium, __global float *fOpacity, __global float *lColor, __global float *lDirection, __global float *lIntensity, int side, int plain , __global float *checker)
{
    int position = get_global_id(0);
    float lightColor[3], lightDirection[3], medium[3], opacity, intensity, g[3];

    for (plain; plain < side; ++plain){

        float previousDirection[3];
        previousDirection[0] = lDirection[(plain - 1) * side * side * 3 + position * 3] / lDirection[(plain - 1) * side * side * 3 + position * 3 + 2] + grad[(plain - 1) * side * side * 3 + position * 3] * 100 / (float)side;
        previousDirection[1] = lDirection[(plain - 1) * side * side * 3 + position * 3 + 1] / lDirection[(plain - 1) * side * side * 3 + position * 3 + 2] + grad[(plain - 1) * side * side * 3 + position * 3 + 1] * 100 / (float)side;
        previousDirection[2] = lDirection[(plain - 1) * side * side * 3 + position * 3 + 2] / lDirection[(plain - 1) * side * side * 3 + position * 3 + 2] + grad[(plain - 1) * side * side * 3 + position * 3 + 2] * 100 / (float)side;
        int backPoint, p1[2], p2[2], p3[2], p4[2];
        // bool flagInt;
        int x, y;
        float backx, backy, xx, yy;

        float coeff = 1 / previousDirection[2];
        backx = previousDirection[0] * coeff;
        backy = previousDirection[1] * coeff;
        y = (int) (position / side);
        x = position - y * side;
        yy = (float)y - backy;
        xx = (float)x - backx;
        p1[0] = (int)checkRange(xx, side);
        p1[1] = (int)checkRange(yy, side);

        p2[0] = p1[0] + 1;
        p2[1] = p1[1];
        p3[0] = p1[0] + 1;
        p3[1] = p1[1] + 1;
        p4[0] = p1[0];
        p4[1] = p1[1] + 1;

        int x1, x2, y1, y2;
        float d1[3], d2[3], c1[3], c2[3], m1[3], m2[3], o1, o2, i1, i2, g1[3], g2[3], vec1[3], vec2[3];

        xx = checkRange(xx, side);
        yy = checkRange(yy, side);
        p1[0] = checkRangeInt(p1[0], side);
        p1[1] = checkRangeInt(p1[1], side);
        p2[0] = checkRangeInt(p2[0], side);
        p2[1] = checkRangeInt(p2[1], side);
        p3[0] = checkRangeInt(p3[0], side);
        p3[1] = checkRangeInt(p3[1], side);
        p4[0] = checkRangeInt(p4[0], side);
        p4[1] = checkRangeInt(p4[1], side);

        x1 = coordinate(p1, side);
        x2 = coordinate(p4, side);
        y1 = coordinate(p2, side);
        y2 = coordinate(p3, side);

        float ratio1 = (xx - (float)getx(x1, side)); 
        float ratio2 = (yy - (float)gety(x1, side));

        // d1[0] = lDirection[(plain - 1) * side * side * 3 + 3 * x1] * (1 - ratio1) + lDirection[(plain - 1) * side * side * 3 + 3 * y1] * ratio1;
        // d1[1] = lDirection[(plain - 1) * side * side * 3 + 3 * x1 + 1] * (1 - ratio1) + lDirection[(plain - 1) * side * side * 3 + 3 * y1 + 1] * ratio1;
        // d1[2] = lDirection[(plain - 1) * side * side * 3 + 3 * x1 + 2] * (1 - ratio1) + lDirection[(plain - 1) * side * side * 3 + 3 * y1 + 2] * ratio1;
        // d2[0] = lDirection[(plain - 1) * side * side * 3 + 3 * x2] * (1 - ratio1) + lDirection[(plain - 1) * side * side * 3 + 3 * y2] * ratio1;
        // d2[1] = lDirection[(plain - 1) * side * side * 3 + 3 * x2 + 1] * (1 - ratio1) + lDirection[(plain - 1) * side * side * 3 + 3 * y2 + 1] * ratio1;
        // d2[2] = lDirection[(plain - 1) * side * side * 3 + 3 * x2 + 2] * (1 - ratio1) + lDirection[(plain - 1) * side * side * 3 + 3 * y2 + 2] * ratio1;
        // lightDirection[0] = d1[0] * (1 - ratio2) + d2[0] * ratio2;
        // lightDirection[1] = d1[1] * (1 - ratio2) + d2[1] * ratio2;
        // lightDirection[2] = d1[2] * (1 - ratio2) + d2[2] * ratio2;
        lightDirection[0] = lDirection[(plain - 1) * side * side * 3 + position * 3];
        lightDirection[1] = lDirection[(plain - 1) * side * side * 3 + position * 3];
        lightDirection[2] = lDirection[(plain - 1) * side * side * 3 + position * 3 * 3];

        g1[0] = grad[(plain - 1) * side * side + 3 * x1] * (1 - ratio1) + grad[(plain - 1) * side * side * 3 + 3 * y1] * ratio1;
        g1[1] = grad[(plain - 1) * side * side + 3 * x1 + 1] * (1 - ratio1) + grad[(plain - 1) * side * side * 3 + 3 * y1 + 1] * ratio1;
        g1[2] = grad[(plain - 1) * side * side + 3 * x1 + 2] * (1 - ratio1) + grad[(plain - 1) * side * side * 3 + 3 * y1 + 2] * ratio1;
        g2[0] = grad[(plain - 1) * side * side + 3 * x2] * (1 - ratio1) + grad[(plain - 1) * side * side * 3 + 3 * y2] * ratio1;
        g2[1] = grad[(plain - 1) * side * side + 3 * x2 + 1] * (1 - ratio1) + grad[(plain - 1) * side * side * 3 + (plain - 1) * side * side + (plain - 1) * side * side + 3 * y2 + 1] * ratio1;
        g2[2] = grad[(plain - 1) * side * side + 3 * x2 + 2] * (1 - ratio1) + grad[(plain - 1) * side * side * 3 + (plain - 1) * side * side + 3 * y2 + 2] * ratio1;
        grad[0] = g1[0] * (1 - ratio2) + g2[0] * ratio2;
        grad[1] = g1[1] * (1 - ratio2) + g2[1] * ratio2;
        grad[2] = g1[2] * (1 - ratio2) + g2[2] * ratio2;

        // c1[0] = lColor[(plain - 1) * side * side * 3 + 3 * x1] * (1 - ratio1) + lColor[(plain - 1) * side * side * 3 + 3 * y1] * ratio1;
        // c1[1] = lColor[(plain - 1) * side * side * 3 + 3 * x1 + 1] * (1 - ratio1) + lColor[(plain - 1) * side * side * 3 + 3 * y1 + 1] * ratio1;
        // c1[2] = lColor[(plain - 1) * side * side * 3 + 3 * x1 + 2] * (1 - ratio1) + lColor[(plain - 1) * side * side * 3 + 3 * y1 + 2] * ratio1;
        // c2[0] = lColor[(plain - 1) * side * side * 3 + 3 * x2] * (1 - ratio1) + lColor[(plain - 1) * side * side * 3 + 3 * y2] * ratio1;
        // c2[1] = lColor[(plain - 1) * side * side * 3 + 3 * x2 + 1] * (1 - ratio1) + lColor[(plain - 1) * side * side * 3 + 3 * y2 + 1] * ratio1;
        // c2[2] = lColor[(plain - 1) * side * side * 3 + 3 * x2 + 2] * (1 - ratio1) + lColor[(plain - 1) * side * side * 3 + 3 * y2 + 2] * ratio1;
        // lightColor[0] = c1[0] * (1 - ratio2) + c2[0] * ratio2;
        // lightColor[1] = c1[1] * (1 - ratio2) + c2[1] * ratio2;
        // lightColor[2] = c1[2] * (1 - ratio2) + c2[2] * ratio2;
        lightColor[0] = lColor[(plain - 1) * side * side * 3 + 3 * position];
        lightColor[1] = lColor[(plain - 1) * side * side * 3 + 3 * position + 1];
        lightColor[2] = lColor[(plain - 1) * side * side * 3 + 3 * position + 2];


        // i1 = lIntensity[(plain - 1) * side * side + x1] * (1 - ratio1) + lIntensity[(plain - 1) * side * side + y1] * ratio1;
        // i2 = lIntensity[(plain - 1) * side * side + x2] * (1 - ratio1) + lIntensity[(plain - 1) * side * side + y2] * ratio1;
        // intensity =  i1 * (1 - ratio2) + i2 * ratio2;
        intensity = lIntensity[(plain - 1) * side * side + position];

        opacity = fOpacity[(plain - 1) * side * side + position];

        medium[0] = fMedium[(plain - 1) * side * side * 3 + position * 3];
        medium[1] = fMedium[(plain - 1) * side * side * 3 + position * 3 + 1];
        medium[2] = fMedium[(plain - 1) * side * side * 3 + position * 3 + 2];

        lColor[(plain) * side * side * 3 + position * 3] = lightColor[0] * medium[0] * (1 - opacity) * intensity;
        lColor[(plain) * side * side * 3 + position * 3 + 1] = lightColor[1] * medium[1] * (1 - opacity) * intensity;
        lColor[(plain) * side * side * 3 + position * 3 + 2] = lightColor[2] * medium[2] * (1 - opacity) * intensity;
        lDirection[(plain) * side * side * 3 + position * 3 ] = lightDirection[0] + grad[0] * 100 / (float)side;
        lDirection[(plain) * side * side * 3 + position * 3 + 1] = lightDirection[1] + grad[1] * 100 / (float)side;
        lDirection[(plain) * side * side * 3 + position * 3 + 2] = lightDirection[2] + grad[2] * 100 / (float)side;
        lIntensity[(plain) * side * side + position] = intensity; //* (1 - lp.opacity);
        checker[position] = lColor[(plain - 1) * side * side * 3 + (position - 10) * 3];//lColor[(plain * 0 + 1) * side * side * 3 + position * 3];
    }
}