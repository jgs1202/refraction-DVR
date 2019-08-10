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

// typedef struct plain {
//     float a;
//     float b;
//     float c;
//     float d;
// }

// plain init (float a, float b, float c, float d) {
//     plain p;
//     p.a = a;
//     p.b = b;
//     p.c = c;
//     p.d = d;
//     return p;
// }

// typedef struct vector {
//     float x;
//     float y;
//     float z;
// }

// vector init (float x, float y, float z) {
//     vector v;
//     v.x = x;
//     v.y = y;
//     v.z = z;
//     return v;
// }

// bool compareDistance(vector v0, vector v1, vector big) {
//     float dis0 = (big.x - v0.x) * (big.x - v0.x) + (big.y - v0.y) * (big.y - v0.y) + (big.z - v0.z) * (big.z - v0.z);
//     float dis1 = (big.x - v1.x) * (big.x - v1.x) + (big.y - v1.y) * (big.y - v1.y) + (big.z - v1.z) * (big.z - v1.z);
//     if (dis0 > dis1) {
//         return true;
//     }
// }

// vector compare(plain p0, plain p1, vector big, vector end) {
//     bool crossA, crossB;
//     vector out1 = {1., 1., 1.};
//     vector out2 = {1., 1., 1.};
//     vector onPlainA = {p0.a * p0.d, p0.b * p0.d, p0.c * p0.d};
//     vector onPlainB = {p1.a * p1.d, p1.b * p1.d, p1.c * p1.d};
//     vector PAtoS = {onPlainA.x - big.x, onPlainA.y - big.y, onPlainA.z - big.z};
//     vector PAtoE = {onPlainA.x - end.x, onPlainA.y - end.y, onPlainA.z - end.z};
//     vector PBtoS = {onPlainB.x - big.x, onPlainB.y - big.y, onPlainB.z - big.z};
//     vector PBtoE = {onPlainB.x - end.x, onPlainB.y - end.y, onPlainB.z - end.z};

//     double dotPAS = PAtoS.x * p0.a + PAtoS.y * p0.b + PAtoS.z * p0.c;
//     double dotPAE = PAtoE.x * p0.a + PAtoE.y * p0.b + PAtoE.z * p0.c;
//     double dotPBS = PBtoS.x * p1.a + PBtoS.y * p1.b + PBtoS.z * p1.c;
//     double dotPBE = PBtoE.x * p1.a + PBtoE.y * p1.b + PBtoE.z * p1.c;
//     if (getAbs(dotPAS) < 0.00001) {dotPAS = 0.0;} 
//     if (getAbs(dotPAE) < 0.00001) {dotPAE = 0.0;} 
//     if (getAbs(dotPBS) < 0.00001) {dotPBS = 0.0;} 
//     if (getAbs(dotPBE) < 0.00001) {dotPBE = 0.0;} 

//     if( dotPAS == 0.0 && dotPAE == 0.0 ) {
//         //両端が平面上にあり、交点を計算できない。
//         crossA =  false;
//     } else
//     if ( ( dotPAS >= 0.0 && dotPAE <= 0.0 ) || ( dotPAS <= 0.0 && dotPAE >= 0.0 ) ) {
//         //内積の片方がプラスで片方がマイナスなので、交差している
//         crossA = true;
//         vector ab = {end.x - big.x, end.y - big.y, end.z - big.z};
//         float ratio = getAbs(dotPAS) / (getAbs(dotPAS) + getAbs(dotPAE));
//         out1.x = big.x + ab.x * ratio;
//         out1.y = big.y + ab.y * ratio;
//         out1.z = big.z + ab.z * ratio;
//     } else {
//         //交差していない
//         crossA = false;
//     }

//     if( dotPBS == 0.0 && dotPBE == 0.0 ) {
//         //両端が平面上にあり、交点を計算できない。
//         crossB =  false;
//     } else
//     if ( ( dotPBS >= 0.0 && dotPBE <= 0.0 ) || ( dotPBS <= 0.0 && dotPBE >= 0.0 ) ) {
//         //内積の片方がプラスで片方がマイナスなので、交差している
//         crossB = true;
//         vector ab = {end.x - big.x, end.y - big.y, end.z - big.z};
//         float ratio = getAbs(dotPBS) / (getAbs(dotPBS) + getAbs(dotPBE));
//         out2.x = big.x + ab.x * ratio;
//         out2.y = big.y + ab.y * ratio;
//         out2.z = big.z + ab.z * ratio;
//     } else {
//         //交差していない
//         crossB = false;
//     }

//     if (crossA && crossB) {
//         if (compareDistance(out1, out2, big)) {
//             return out2;
//         } else {
//             return out1;
//         } 
//     } else if (crossA) {
//         return out1;
//     } else if (crossB) {
//         return out2;
//     } else {
//         vector vec = {1., 1., 1.};
//         return vec;
//     }
// }

// vector crossPoint(float inix, float iniy, float iniz, float anglex, float angley, float anglez, float distance, float side) {
//     // plains ax+by+cz+d=0;
//     plain p0 = {1., 0., 0., side/2};
//     plain p1 = {1., 0., 0., -side/2};
//     plain p2 = {0., 1., 0., side/2};
//     plain p3 = {0., 1., 0., -side/2};
//     plain p4 = {0., 0., 1., side/2};
//     plain p5 = {0., 0., 1., -side/2};

//     vector big = {inix, iniy, iniz};
//     vector end = {inix + anglex * distance * 2, iniy + angley * distance * 2, iniz + anglez * distance * 2};

//     vector out0 = compare(p0, p1, big, end);
//     vector out1 = compare(p2, p3, big, end);
//     vector out2 = compare(p4, p5, big, end);

// }

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

    // iniViewP[0] = (float)side/2;
    // iniViewP[1] = (float)side/2;
    // iniViewP[2] = -distance;

    // iniEndP[0] = (float)side / 2 + (xx - (float)side/2) / (distance + (float)side / 2);
    // iniEndP[1] = (float)side / 2 + (xx - (float)side/2) / (distance + (float)side / 2);
    // iniEndP[2] = -distance + 1;

    // rotViewP[0] = iniViewP[0] * (1 + cos(sx)) + iniViewP[2] * sin(sx);
    // rotViewP[1] = iniViewP[1] * cos(sy) - iniViewP[2] * sin(sy) + iniViewP[1];
    // rotViewP[2] = iniViewP[1] * sin(sy) + iniViewP[2] * cos(sy) - iniViewP[0] * sin(sx) + iniViewP[2] * cos(sx);

    // rotEndP[0] = iniEndP[0] * (1 + cos(sx)) + iniEndP[2] * sin(sx);
    // rotEndP[1] = iniEndP[1] * cos(sy) - iniEndP[2] * sin(sy) + iniEndP[1];
    // rotEndP[2] = iniEndP[1] * sin(sy) + iniEndP[2] * cos(sy) - iniEndP[0] * sin(sx) + iniEndP[2] * cos(sx);

    // angle[0] = rotViewP[0] - rotEndP[0];
    // angle[1] = rotViewP[1] - rotEndP[1];
    // angle[2] = rotViewP[2] - rotEndP[2];

    // xx = rotViewP[0];
    // yy = rotViewP[1];
    // z = rotViewP[2];

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
}int getx (int p, int side) {
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

typedef struct plain {
    float a;
    float b;
    float c;
    float d;
};

plain init (float a, float b, float c, float d) {
    plain p;
    p.a = a;
    p.b = b;
    p.c = c;
    p.d = d;
    return p;
}

typedef struct vector {
    float x;
    float y;
    float z;
};

vector init (float x, float y, float z) {
    vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

bool compareDistance(vector v0, vector v1, vector big) {
    float dis0 = (big.x - v0.x) * (big.x - v0.x) + (big.y - v0.y) * (big.y - v0.y) + (big.z - v0.z) * (big.z - v0.z);
    float dis1 = (big.x - v1.x) * (big.x - v1.x) + (big.y - v1.y) * (big.y - v1.y) + (big.z - v1.z) * (big.z - v1.z);
    if (dis0 > dis1) {
        return true;
    }
}

vector compare(plain p0, plain p1, vector big, vector end) {
    bool crossA, crossB;
    vector out1 = {1., 1., 1.};
    vector out2 = {1., 1., 1.};
    vector onPlainA = {p0.a * p0.d, p0.b * p0.d, p0.c * p0.d};
    vector onPlainB = {p1.a * p1.d, p1.b * p1.d, p1.c * p1.d};
    vector PAtoS = {onPlainA.x - big.x, onPlainA.y - big.y, onPlainA.z - big.z};
    vector PAtoE = {onPlainA.x - end.x, onPlainA.y - end.y, onPlainA.z - end.z};
    vector PBtoS = {onPlainB.x - big.x, onPlainB.y - big.y, onPlainB.z - big.z};
    vector PBtoE = {onPlainB.x - end.x, onPlainB.y - end.y, onPlainB.z - end.z};

    double dotPAS = PAtoS.x * p0.a + PAtoS.y * p0.b + PAtoS.z * p0.c;
    double dotPAE = PAtoE.x * p0.a + PAtoE.y * p0.b + PAtoE.z * p0.c;
    double dotPBS = PBtoS.x * p1.a + PBtoS.y * p1.b + PBtoS.z * p1.c;
    double dotPBE = PBtoE.x * p1.a + PBtoE.y * p1.b + PBtoE.z * p1.c;
    if (getAbs(dotPAS) < 0.00001) {dotPAS = 0.0;} 
    if (getAbs(dotPAE) < 0.00001) {dotPAE = 0.0;} 
    if (getAbs(dotPBS) < 0.00001) {dotPBS = 0.0;} 
    if (getAbs(dotPBE) < 0.00001) {dotPBE = 0.0;} 

    if( dotPAS == 0.0 && dotPAE == 0.0 ) {
        //両端が平面上にあり、交点を計算できない。
        crossA =  false;
    } else
    if ( ( dotPAS >= 0.0 && dotPAE <= 0.0 ) || ( dotPAS <= 0.0 && dotPAE >= 0.0 ) ) {
        //内積の片方がプラスで片方がマイナスなので、交差している
        crossA = true;
        vector ab = {end.x - big.x, end.y - big.y, end.z - big.z};
        float ratio = getAbs(dotPAS) / (getAbs(dotPAS) + getAbs(dotPAE));
        out1.x = big.x + ab.x * ratio;
        out1.y = big.y + ab.y * ratio;
        out1.z = big.z + ab.z * ratio;
    } else {
        //交差していない
        crossA = false;
    }

    if( dotPBS == 0.0 && dotPBE == 0.0 ) {
        //両端が平面上にあり、交点を計算できない。
        crossB =  false;
    } else
    if ( ( dotPBS >= 0.0 && dotPBE <= 0.0 ) || ( dotPBS <= 0.0 && dotPBE >= 0.0 ) ) {
        //内積の片方がプラスで片方がマイナスなので、交差している
        crossB = true;
        vector ab = {end.x - big.x, end.y - big.y, end.z - big.z};
        float ratio = getAbs(dotPBS) / (getAbs(dotPBS) + getAbs(dotPBE));
        out2.x = big.x + ab.x * ratio;
        out2.y = big.y + ab.y * ratio;
        out2.z = big.z + ab.z * ratio;
    } else {
        //交差していない
        crossB = false;
    }

    if (crossA && crossB) {
        if (compareDistance(out1, out2, big)) {
            return out2;
        } else {
            return out1;
        } 
    } else if (crossA) {
        return out1;
    } else if (crossB) {
        return out2;
    } else {
        vector vec = {1., 1., 1.};
        return vec;
    }
}

bool checkVectorOne(vector v) {
    if (v.x != 1.) {
        return false;
    } else if (v.y != 1.) {
        return false;
    } else if (v.z != 1.) {
        return false;
    } else {
        return true;
    }
}

bool checkSameVector(vector a, vector b) {
    (if (a.x == b.x) && (a.y == b.y) && (a.z == b.z)) {
        return true;
    } else {
        return false;
    }
}

float vectorDis(vector a, vector b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z))
}

vector crossPoint(float inix, float iniy, float iniz, float anglex, float angley, float anglez, float distance, float side) {
    // plains ax+by+cz+d=0;
    plain p0 = {1., 0., 0., side/2};
    plain p1 = {1., 0., 0., -side/2};
    plain p2 = {0., 1., 0., side/2};
    plain p3 = {0., 1., 0., -side/2};
    plain p4 = {0., 0., 1., side/2};
    plain p5 = {0., 0., 1., -side/2};

    vector big = {inix, iniy, iniz};
    vector end = {inix + anglex * distance * 2, iniy + angley * distance * 2, iniz + anglez * distance * 2};

    vector out0 = compare(p0, p1, big, end);
    vector out1 = compare(p2, p3, big, end);
    vector out2 = compare(p4, p5, big, end);

    float dis0 = vectorDis(big, out0);
    float dis1 = vectorDis(big, out1);
    float dis2 = vectorDis(big, out2);

    vector can0, can1, can2;
    can0 = (dis0 <= dis1)? ((dis0 <= dis2)? dis0 : dis2) : ((dis1 <= dis2) dis1: dis2);
    can2 = (dis0 >= dis1)? ((dis0 >= dis2)? dis0 : dis2) : ((dis1 >= dis2) dis1: dis2);
    can1 = (checkSameVector(can0, dis0))? (checkSameVector(can2, dis1)? dis2 : dis1) : (checkSameVector(can2, dis0)? (checkSameVector(can0, dis1)? dis1: dis2): dis0);

    if (!checkVectorOne(can0)) {
        return can0;
    } else if (!checkVectorOne(can1)) {
        return can1;
    } else {
        return can2;
    }
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

    iniViewP[0] = (float)side/2;
    iniViewP[1] = (float)side/2;
    iniViewP[2] = -distance;

    iniEndP[0] = (float)side / 2 + (xx - (float)side/2) / (distance + (float)side / 2);
    iniEndP[1] = (float)side / 2 + (xx - (float)side/2) / (distance + (float)side / 2);
    iniEndP[2] = -distance + 1;

    rotViewP[0] = iniViewP[0] * (1 + cos(sx)) + iniViewP[2] * sin(sx);
    rotViewP[1] = iniViewP[1] * cos(sy) - iniViewP[2] * sin(sy) + iniViewP[1];
    rotViewP[2] = iniViewP[1] * sin(sy) + iniViewP[2] * cos(sy) - iniViewP[0] * sin(sx) + iniViewP[2] * cos(sx);

    rotEndP[0] = iniEndP[0] * (1 + cos(sx)) + iniEndP[2] * sin(sx);
    rotEndP[1] = iniEndP[1] * cos(sy) - iniEndP[2] * sin(sy) + iniEndP[1];
    rotEndP[2] = iniEndP[1] * sin(sy) + iniEndP[2] * cos(sy) - iniEndP[0] * sin(sx) + iniEndP[2] * cos(sx);

    angle[0] = rotViewP[0] - rotEndP[0];
    angle[1] = rotViewP[1] - rotEndP[1];
    angle[2] = rotViewP[2] - rotEndP[2];

    vector cross = crossPoint(rotViewP[0], rotViewP[1], rotViewP[2], angle[0], angle[1], angle[2], distance, side);
    xx = cross.x;
    yy = cross.y;
    z = cross.z;

    if (!checkSameVector(cross)) {
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
    }

    checker[position] = color[0];

    pixel[position * 3] = color[0];
    pixel[position * 3 + 1] = color[1];
    pixel[position * 3 + 2] = color[2];
}