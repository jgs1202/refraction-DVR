//
// Created by Nozomi on 2019-05-11.
//

#include "objectData.h"
#include "variables.h"
#include "myMath.h"
#include "vector.h"
#include "stdio.h"
#include "cmath"

// position, radius, surface color, reflectivity, transparency, refractivity, emission color
void inSphere(float *center, float radius, float *surfaceColor, float reflectivity, float opacity, float refractivity, float *emissionColor){
    for (unsigned int i=0; i < WIDTH; ++i){
        for (unsigned int j=0; j<HEIGHT; ++j){
            for (unsigned int k=0; k<DEPTH; ++k){
                if (std::abs(i - center[0]) > radius){
                    continue;
                } else {
                } if (std::abs(j - center[1]) > radius){
                    continue;
                } if (std::abs(k - center[2]) > radius){
                    continue;
                } if (((i - center[0]) * (i - center[0]) + (j - center[1]) * (j - center[1]) + (k - center[2]) * (k - center[2])) > radius * radius){
                    continue;
                }
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = surfaceColor[0];
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = surfaceColor[1];
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = surfaceColor[2];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = surfaceColor[0];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = surfaceColor[1];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = surfaceColor[2];
//                fReflectivity[i + j * WIDTH + k * WIDTH * HEIGHT].reflectivity = reflectivity;
                fOpacity[(i + j * WIDTH + k * WIDTH * HEIGHT)] = opacity;
//                f[i + j * WIDTH + k * WIDTH * HEIGHT) * 3].emissionColor = emissionColor;
                fRefractivity[(i + j * WIDTH + k * WIDTH * HEIGHT)] = refractivity;
            }
        }
    }
}

//void setSurface(float *p1, float *p2, float *p3){
//    // p1, p2, p3を外側に向かって反時計回り（右ねじ）
//    float vec1[3], vec2[3], cross[3];
//    surfaces.push_back(p1[0]);
//    surfaces.push_back(p1[1]);
//    surfaces.push_back(p1[2]);
//    surfaces.push_back(p2[0]);
//    surfaces.push_back(p2[1]);
//    surfaces.push_back(p2[2]);
//    surfaces.push_back(p3[0]);
//    surfaces.push_back(p3[1]);
//    surfaces.push_back(p3[2]);
//
//    subVec(p2, p1, vec1);
//    subVec(p3, p1, vec2);
//    cross[0] =  vec1[1] * vec2[2] - vec1[2] * vec2[1];
//    cross[1] =  vec1[2] * vec2[0] - vec1[0] * vec2[2];
//    cross[2] =  vec1[0] * vec2[1] - vec1[1] * vec2[0];
//    vecNormalize(cross, cross);
//    surfaces.push_back(cross[0]);
//    surfaces.push_back(cross[1]);
//    surfaces.push_back(cross[2]);
//}

void inPlain (float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float refractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity){
    unsigned int dis1 = std::floor(vectorDistance(p1, p2)) + 1;
    unsigned int dis2 = std::floor(vectorDistance(p1, p3)) + 1;
    float vec1[3], vec2[3], sub1[3], sub2[3];
    subVec(p2, p1, sub1);
    subVec(p3, p1, sub2);
    vecNormalize(sub1, vec1);
    vecNormalize(sub2, vec2);
    float current[3];
    float integer[3];
    for (unsigned int x=0; x < dis1; ++x){
        for (unsigned int y=0; y < dis2; ++y){
            current[0] = p1[0] + vec1[0] * x + vec2[0] * y;
            current[1] = p1[1] + vec1[1] * x + vec2[1] * y;
            current[2] = p1[2] + vec1[2] * x + vec2[2] * y;
            integer[0] = roundInt(current[0]);
            integer[1] = roundInt(current[1]);
            integer[2] = roundInt(current[2]);
            if ((integer[0] < 0) || (integer[0] >= WIDTH - 1) || (integer[1] < 0) || (integer[1] >= HEIGHT - 1) || (integer[2] < 0) || (integer[2] >= DEPTH - 1)){
                continue;
            } else {
//                std::cout << " " << dis2 << " " << integer[0] << " " << integer[1] << " " << integer[2] << std::endl;
                setVec3Float(&fColor[getCoordinate(integer)], color);
//                std::cout << getCoordinate(integer) <<" " << fColor[getCoordinate(integer)] << " " << opacity << std::endl;
                setVec3Float(&fMedium[getCoordinate(integer)], color);
//                fRelectivity[getCoordinate(integer)] = reflectivity;
                fOpacity[getScalarCoo(integer)] = opacity;
                fRefractivity[getScalarCoo(integer)] = refractivity;
            }
        }
    }
//    setSurface(p1, p2, p3);
//    setSurface(p1, p4, p3);
}

void inRectangle(float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float refractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity){
    // the lengths of sides
    unsigned int dis1 = std::floor(vectorDistance(p1, p2)) + 1;
    unsigned int dis2 = std::floor(vectorDistance(p1, p3)) + 1;
    unsigned int dis3 = std::floor(vectorDistance(p1, p4)) + 1;
    float vec1[3], vec2[3], vec3[3], sub1[3], sub2[3], sub3[3];
    subVec(p2, p1, sub1);
    subVec(p3, p1, sub2);
    subVec(p4, p1, sub3);
//    std::cout << sub3[0] << sub3[1] << sub3[2] << std::endl;
    vecNormalize(sub1, vec1);
    vecNormalize(sub2, vec2);
    vecNormalize(sub3, vec3);
    float current[3];
    float integer[3];
    for (int x=0; x < dis1; ++x) {
        for (int y = 0; y < dis2; ++y) {
            for (int z = 0; z < dis3; ++z){
                current[0] = p1[0] + vec1[0] * x + vec2[0] * y + vec3[0] * z;
                current[1] = p1[1] + vec1[1] * x + vec2[1] * y + vec3[1] * z;
                current[2] = p1[2] + vec1[2] * x + vec2[2] * y + vec3[2] * z;
                integer[0] = roundInt(current[0]);
                integer[1] = roundInt(current[1]);
                integer[2] = roundInt(current[2]);
                if ((integer[0] < 0) || (integer[0] >= WIDTH) || (integer[1] < 0) || (integer[1] >= HEIGHT) ||
                    (integer[2] < 0) || (integer[2] >= DEPTH)) {
                    continue;
                } else {
//                    std::cout << "inrec " << getCoordinate(integer) <<" " << fColor[getCoordinate(integer)] << " " << opacity << std::endl;
                    setVec3Float(&fColor[getCoordinate(integer)], color);
                    setVec3Float(&fMedium[getCoordinate(integer)], color);
//                    fReflectivity[getCoordinate(integer)] = reflectivity;
                    fOpacity[getScalarCoo(integer)] = opacity;
                    fRefractivity[getScalarCoo(integer)] = refractivity;
                }
            }
        }
    }
    // set surfaces
//    float q1[3], q2[3], q3[3], q4[3];
//    subVec(p2, p1, sub1);
//    subVec(p3, p1, sub2);
//    subVec(p4, p1, sub3);
//    addVec(sub1, sub2, vec1);
//    addVec(sub1, sub3, vec2);
//    addVec(p3, vec2, q1);
//    addVec(p4, sub2, q2);
//    addVec(p1, vec2, q3);
//    addVec(p2, sub2, q4);

//    setSurface(p1, p4, p3);
//    setSurface(q2, p3, p4);
//    setSurface(p1, p3, p2);
//    setSurface(q4, p2, p3);
//    setSurface(p1, p2, p4);
//    setSurface(q3, p4, p2);
//
//    setSurface(q1, q4, q2);
//    setSurface(p3, q2, q4);
//    setSurface(q1, q2, q3);
//    setSurface(p4, q3, q2);
//    setSurface(q1, q3, q4);
//    setSurface(p2, q4, q3);
}