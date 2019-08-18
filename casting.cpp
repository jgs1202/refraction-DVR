//
// Created by Nozomi on 2019-05-11.
//

#include "casting.h"
#include "variables.h"
#include "myMath.h"
#include "vector.h"
#include "gpuSetting.h"
#include "render.h"

#include <cmath>
#include <OpenCL/opencl.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>

void visualize(void) {
    //  make image of the object field
    std::vector<float> gradPlainY(PLAINSZ);
    std::vector<float> gradPlainX(PLAINSZ);
    for (int z=0; z<DEPTH; ++z) {
        for (int x = 0; x < WIDTH; ++x) {
            gradPlainY[x + z * WIDTH] = grad[(x + plainY * WIDTH + z * WIDTH * HEIGHT) * 3];
            gradPlainX[x + z * WIDTH] = grad[(plainX + x * WIDTH + z * WIDTH * HEIGHT) * 3 + 1];
        }
    }
    float maxY = *std::max_element(gradPlainY.begin(), gradPlainY.end()), valueY, normalY;
    float maxX = *std::max_element(gradPlainX.begin(), gradPlainX.end()), valueX, normalX;
    cv::Mat gradAndPathsY(cv::Size(WIDTH * 5, HEIGHT * 5), CV_8UC3);
    cv::Mat gradAndPathsX(cv::Size(WIDTH * 5, HEIGHT * 5), CV_8UC3);
    cv::Mat refracIndexY(cv::Size(WIDTH, HEIGHT), CV_8UC1);
    cv::Mat refracIndexX(cv::Size(WIDTH, HEIGHT), CV_8UC1);
    for (int z=0; z<DEPTH; ++z){
        for (int x=0; x<WIDTH; ++x){
            for (int i=0; i<5; ++i){
                for (int j=0; j<5; ++j) {
                    valueY = grad[(x + plainY * WIDTH + z * WIDTH * HEIGHT) * 3];
                    normalY = valueY / maxY;
                    gradAndPathsY.at<cv::Vec3b>((x + z * 5 * WIDTH) * 5 + i + j * 5 * WIDTH)[0] = (valueY > 0)?normalY * 255 : 0;
                    gradAndPathsY.at<cv::Vec3b>((x + z * 5 * WIDTH) * 5 + i + j * 5 * WIDTH)[1] = 100 * gaussFunc((std::abs(normalY) * 255));
                    gradAndPathsY.at<cv::Vec3b>((x + z * 5 * WIDTH) * 5 + i + j * 5 * WIDTH)[2] = (valueY > 0)? 0 :  - 1 * normalY * 255;

                    valueX = grad[(plainX + x * WIDTH + z * WIDTH * HEIGHT) * 3 + 1];
                    normalX = valueX / maxX;
                    gradAndPathsX.at<cv::Vec3b>((z + x * 5 * WIDTH) * 5 + j + i * 5 * WIDTH)[0] = (valueX > 0)?normalX * 255 : 0;
                    gradAndPathsX.at<cv::Vec3b>((z + x * 5 * WIDTH) * 5 + j + i * 5 * WIDTH)[1] = 100 * gaussFunc((std::abs(normalX) * 255));
                    gradAndPathsX.at<cv::Vec3b>((z + x * 5 * WIDTH) * 5 + j + i * 5 * WIDTH)[2] = (valueX > 0)? 0 :  - 1 * normalX * 255;
                }
            }
        }
    }
    int x, z;
    for (int i=0; i<castIndexY; ++i){
        x = (int)roundInt(castPositionY[i * 2] * 5);
        z = (int)roundInt(castPositionY[i * 2 + 1] * 5);
        gradAndPathsY.at<cv::Vec3b>(x + z * 5 * WIDTH)[0] = 255;
        gradAndPathsY.at<cv::Vec3b>(x + z * 5 * WIDTH)[1] = 255;
        gradAndPathsY.at<cv::Vec3b>(x + z * 5 * WIDTH)[2] = 255;

        x = (int)roundInt(castPositionX[i * 2] * 5);
        z = (int)roundInt(castPositionX[i * 2 + 1] * 5);
        gradAndPathsX.at<cv::Vec3b>(z + x * 5 * WIDTH)[0] = 255;
        gradAndPathsX.at<cv::Vec3b>(z + x * 5 * WIDTH)[1] = 255;
        gradAndPathsX.at<cv::Vec3b>(z + x * 5 * WIDTH)[2] = 255;
    }
    float maxRefraction = 1.8;
    for (int z=0; z<DEPTH; ++z){
        for (int x=0; x<WIDTH; ++x){
            valueY = fRefractivity[(x + plainY * WIDTH + z * WIDTH * HEIGHT)];
            normalY = valueY / maxRefraction;
            refracIndexY.at<unsigned char>(x + z * WIDTH) = exponential(normalY) / exponential(1) * 255;

            valueX = fRefractivity[(plainX + x * WIDTH + z * WIDTH * HEIGHT)];
            normalX = valueX / maxRefraction;
            refracIndexX.at<unsigned char>(z + x * WIDTH) = exponential(normalX) / exponential(1) * 255;
        }
    }
    imwrite("images/gradY.png", gradAndPathsY);
    imwrite("images/gradX.png", gradAndPathsX);
    imwrite("images/refractive_indexY.png", refracIndexY);
    imwrite("images/refractive_indexX.png", refracIndexX);
    printf("img written\n");
}

void visRays(float* rayBeginX, float* rayAngleX, float* rayBeginY, float* rayAngleY, int distance) {
    std::uniform_real_distribution<double> rnd(0.0,1.0);
    std::random_device rd;
    std::mt19937 mt(rd());
    int magnify = 5, magArrow = 100;
    cv::Scalar white(255, 255, 255);
    cv::Mat rayDirY(cv::Size(distance * 3, distance * 3), CV_8UC3, white);
    cv::Mat rayDirX(cv::Size(distance * 3, distance * 3), CV_8UC3, white);

    int sx, sy, ex, ey;
    float dx, dy, thre = 0.3;
    for (int ray=0; ray<WIDTH; ++ray) {
        if (rnd(mt) < thre) {
            sx = (int) (rayBeginY[2 * ray] + distance * 1.5);
            sy = (int) (rayBeginY[2 * ray + 1] + distance * 1.5);
            ex = (int) (sx + rayAngleY[2 * ray] * magArrow);
            ey = (int) (sy + rayAngleY[2 * ray + 1] * magArrow);
//        std::cout << sx << " " << ex << " " << sy << " " << ey << " " << rayAngle[2 * ray + 1] * magArrow << std::endl;
            cv::arrowedLine(rayDirY, cv::Point(sx, sy), cv::Point(ex, ey), cv::Scalar(255, 0, 0), 1, 4);
        }
    }
    cv::circle(rayDirY, cv::Point((int)(distance * 1.5), (int)(distance * 1.5)), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDirY, cv::Point((int)(distance * 1.5) + WIDTH, (int)(distance * 1.5)), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDirY, cv::Point((int)(distance * 1.5) + WIDTH, (int)(distance * 1.5) + WIDTH), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDirY, cv::Point((int)(distance * 1.5), (int)(distance * 1.5) + WIDTH), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    imwrite("images/rayDirectionY.png", rayDirY);

    for (int ray=0; ray<WIDTH; ++ray) {
        if (rnd(mt) < thre) {
            sx = (int) (rayBeginY[2 * ray] + distance * 1.5);
            sy = (int) (rayBeginY[2 * ray + 1] + distance * 1.5);
            ex = (int) (sx + rayAngleY[2 * ray] * magArrow);
            ey = (int) (sy + rayAngleY[2 * ray + 1] * magArrow);
//        std::cout << sx << " " << ex << " " << sy << " " << ey << " " << rayAngle[2 * ray + 1] * magArrow << std::endl;
            cv::arrowedLine(rayDirX, cv::Point(sx, sy), cv::Point(ex, ey), cv::Scalar(255, 0, 0), 1, 4);
        }
    }
    cv::circle(rayDirX, cv::Point((int)(distance * 1.5), (int)(distance * 1.5)), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDirX, cv::Point((int)(distance * 1.5) + WIDTH, (int)(distance * 1.5)), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDirX, cv::Point((int)(distance * 1.5) + WIDTH, (int)(distance * 1.5) + WIDTH), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDirX, cv::Point((int)(distance * 1.5), (int)(distance * 1.5) + WIDTH), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    imwrite("images/rayDirectionX.png", rayDirX);
    printf("img written\n");
}

void visTrajectory(int distance) {
    int magnify = 5, magArrow = 100;
    cv::Scalar white(255, 255, 255);
    cv::Mat traje(cv::Size(distance * 3, distance * 3), CV_8UC3, white);

    float sy, sx;
    int total = WIDTH * WIDTH;
    sx = M_PI * 2 / total;
    sy = 0;
    double p[] = {(double)distance, 0., 0.}, q[3];

    for (int step=0; step<total; ++step) {
        // update position
        q[0] = p[0];
        q[1] = p[1] * cos(step * sy) - p[2] * sin(step * sy);
        q[2] = p[1] * sin(step * sy) + p[2] * cos(step * sy);
        q[0] = p[0] * cos(step * sx) + p[2] * sin(step * sx);
        q[1] = p[1];
        q[2] = - p[0] * sin(step * sx) + p[2] * cos(step * sx);

        traje.at<cv::Vec3b>((int)(q[0] + distance * 1.5) + (int)(q[2] + distance * 1.5) * distance * 3)[1] = 0;
        traje.at<cv::Vec3b>((int)(q[0] + distance * 1.5) + (int)(q[2] + distance * 1.5) * distance * 3)[2] = 0;
    }
    imwrite("images/trajectory.png", traje);
    printf("img written\n");
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

float getAbs(float x){
    float a;
    if (x < 0){
        a = -x;
    } else {
        a = x;
    }
    return a;
}

bool checkRangeVector(vector v, int side) {
    if (v.x < 0) {
        return false;
    } else if (v.y < 0) {
        return false;
    } else if (v.z < 0) {
        return false;
    } else if (v.x > side - 1) {
        return false;
    } else if (v.y > side - 1) {
        return false;
    } else if (v.z > side - 1) {
        return false;
    } else {
        return true;
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
    if ((a.x == b.x) && (a.y == b.y) && (a.z == b.z)) {
        return true;
    } else {
        return false;
    }
}

float vectorDis(vector a, vector b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

vector compare(plain p0, plain p1, vector big, vector end) {
    bool crossA = false, crossB = false;
    float ratio1, ratio2;
    vector out1 = {1., 1., 1.};
    vector out2 = {1., 1., 1.};
    vector onPlainA = {p0.a * p0.d, p0.b * p0.d, p0.c * p0.d};
    vector onPlainB = {p1.a * p1.d, p1.b * p1.d, p1.c * p1.d};
    vector PAtoS = {onPlainA.x - big.x, onPlainA.y - big.y, onPlainA.z - big.z};
    vector PAtoE = {onPlainA.x - end.x, onPlainA.y - end.y, onPlainA.z - end.z};
    vector PBtoS = {onPlainB.x - big.x, onPlainB.y - big.y, onPlainB.z - big.z};
    vector PBtoE = {onPlainB.x - end.x, onPlainB.y - end.y, onPlainB.z - end.z};
//    std::cout << big.x << " " << big.y << " " << big.z << std::endl;
//    std::cout << end.x << " " << end.y << " " << end.z << std::endl;

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
    } else if ( ( dotPAS >= 0.0 && dotPAE <= 0.0 ) || ( dotPAS <= 0.0 && dotPAE >= 0.0 ) ) {
        //内積の片方がプラスで片方がマイナスなので、交差している
        crossA = true;
        vector ab = {end.x - big.x, end.y - big.y, end.z - big.z};
        ratio1 = getAbs(dotPAS) / (getAbs(dotPAS) + getAbs(dotPAE));
        out1.x = big.x + ab.x * ratio1;
        out1.y = big.y + ab.y * ratio1;
        out1.z = big.z + ab.z * ratio1;
        if (getAbs(out1.x) < 0.01) {out1.x = 0.0;}
        if (getAbs(out1.y) < 0.01) {out1.y = 0.0;}
        if (getAbs(out1.z) < 0.01) {out1.z = 0.0;}
        if (getAbs(out1.x - (float)(WIDTH - 1)) < 0.01) {out1.x = (float)(WIDTH - 1);}
        if (getAbs(out1.y - (float)(WIDTH - 1)) < 0.01) {out1.y = (float)(WIDTH - 1);}
        if (getAbs(out1.z - (float)(WIDTH - 1)) < 0.01) {out1.z = (float)(WIDTH - 1);}
//        std::cout <<  out1.x << " " << out1.y << " " << out1.z;
        if (!checkRangeVector(out1, WIDTH)) {
//            printf(" out of field\n");
            out1.x = 1;
            out1.y = 1;
            out1.z = 1;
        }
    } else {
        //交差していない
        crossA = false;
    }

    if( dotPBS == 0.0 && dotPBE == 0.0 ) {
        //両端が平面上にあり、交点を計算できない。
        crossB =  false;
    } else if ( ( dotPBS >= 0.0 && dotPBE <= 0.0 ) || ( dotPBS <= 0.0 && dotPBE >= 0.0 ) ) {
        //内積の片方がプラスで片方がマイナスなので、交差している
        crossB = true;
        vector ab = {end.x - big.x, end.y - big.y, end.z - big.z};
        ratio2 = getAbs(dotPBS) / (getAbs(dotPBS) + getAbs(dotPBE));
        out2.x = big.x + ab.x * ratio2;
        out2.y = big.y + ab.y * ratio2;
        out2.z = big.z + ab.z * ratio2;
        if (getAbs(out2.x) < 0.01) {out2.x = 0.0;}
        if (getAbs(out2.y) < 0.01) {out2.y = 0.0;}
        if (getAbs(out2.z) < 0.01) {out2.z = 0.0;}
        if (getAbs(out2.x - (float)(WIDTH - 1)) < 0.01) {out2.x = (float)(WIDTH - 1);}
        if (getAbs(out2.y - (float)(WIDTH - 1)) < 0.01) {out2.y = (float)(WIDTH - 1);}
        if (getAbs(out2.z - (float)(WIDTH - 1)) < 0.01) {out2.z = (float)(WIDTH - 1);}
//        std::cout << " " << out2.x << " " << out2.y << " " << out2.z << std::endl;
        if (!checkRangeVector(out2, WIDTH)) {
//            printf(" out of field\n");
            out2.x = 1;
            out2.y = 1;
            out2.z = 1;
        }
    } else {
        //交差していない
        crossB = false;
    }
    vector out;
    if ((crossA) && (crossB)) {
//        printf("case 0\n");
        if (checkVectorOne(out1)) {
            out = out2;
        } else if (checkVectorOne(out2)) {
            out = out1;
        } else if (compareDistance(out1, out2, big)) {
            out = out2;
        } else {
            out = out1;
        }
    } else if (crossA) {
//        printf("case 1\n");
        out = out1;
    } else if (crossB) {
//        printf("case 2\n");
        out = out2;
    } else {
//        printf("case 3\n");
        vector vec = {1., 1., 1.};
        out = vec;
    }
//    std::cout << out.x << " " << out.y << " " << out.z << " ratio: " << dotPAS << std::endl;
    return out;
}

vector crossPoint(float inix, float iniy, float iniz, float anglex, float angley, float anglez, float distance, float side) {
    // plains ax+by+cz+d=0;
    plain p0 = {1., 0., 0., side-1};
    plain p1 = {1., 0., 0., 0};
    plain p2 = {0., 1., 0., side-1};
    plain p3 = {0., 1., 0., 0};
    plain p4 = {0., 0., 1., side-1};
    plain p5 = {0., 0., 1., 0};

    vector big = {inix, iniy, iniz};
    vector end = {inix + anglex * distance * 2, iniy + angley * distance * 2, iniz + anglez * distance * 2};

    vector out0 = compare(p0, p1, big, end);
    vector out1 = compare(p2, p3, big, end);
    vector out2 = compare(p4, p5, big, end);

    float dis0 = vectorDis(big, out0); //3
    float dis1 = vectorDis(big, out1); //1
    float dis2 = vectorDis(big, out2); //2

    vector can0, can1, can2;
    can0 = (dis0 <= dis1)? ((dis0 <= dis2)? out0 : out2) : ((dis1 <= dis2)? out1 : out2);
    can2 = (dis0 >= dis1)? ((dis0 >= dis2)? out0 : out2) : ((dis1 >= dis2)? out1 : out2);
    can1 = (checkSameVector(can0, out0)) ? (checkSameVector(can2, out1) ? out2 : out1) : (checkSameVector(can2, out0)? (checkSameVector(can0, out1)? out2 : out1) : out0);

    if (!checkVectorOne(can0)) {
//        printf("comp 0\n");
        return can0;
    } else if (!checkVectorOne(can1)) {
//        printf("comp 1\n");
        return can1;
    } else {
//        printf("comp 2\n");
        return can2;
    }
}

void rayRotation(float* iniViewP, float* rotViewP, float* iniEndP, float* rotEndP, float sx, float sy, float* angle){
    float tmp[3];

    tmp[0] = iniViewP[0];
    tmp[1] = iniViewP[1] * cos(sy) - iniViewP[2] * sin(sy);
    tmp[2] = iniViewP[1] * sin(sy) + iniViewP[2] * cos(sy);
    rotViewP[0] = tmp[0] * cos(sx) + tmp[2] * sin(sx);
    rotViewP[1] = tmp[1];
    rotViewP[2] = - tmp[0] * sin(sx) + tmp[2] * cos(sx);

    tmp[0] = iniEndP[0];
    tmp[1] = iniEndP[1] * cos(sy) - iniEndP[2] * sin(sy);
    tmp[2] = iniEndP[1] * sin(sy) + iniEndP[2] * cos(sy);
    rotEndP[0] = tmp[0] * cos(sx) + tmp[2] * sin(sx);
    rotEndP[1] = tmp[1];
    rotEndP[2] = - tmp[0] * sin(sx) + tmp[2] * cos(sx);

    angle[0] = - rotViewP[0] + rotEndP[0];
    angle[1] = - rotViewP[1] + rotEndP[1];
    angle[2] = - rotViewP[2] + rotEndP[2];

    rotViewP[0] = rotViewP[0] + (float)(WIDTH / 2);
    rotViewP[1] = rotViewP[1] + (float)(WIDTH / 2);
    rotViewP[2] = rotViewP[2] + (float)(WIDTH / 2);
}

void renderRotate(float distance, float *pixel) {
    float angle[3], dot[3], sx, sy, xx, yy, z;
    float iniViewP[3], iniEndP[3], rotViewP[3], rotEndP[3];
    int side = WIDTH;
//    float* rayBeginX = new float[WIDTH * 2];
//    float* rayAngleX = new float[WIDTH * 2];
//    float* rayBeginY = new float[WIDTH * 2];
//    float* rayAngleY = new float[WIDTH * 2];
    float* rayPositions = new float[viewH * viewW * 3];
    float* rayAngles = new float[viewH * viewW * 3];

    sx = shiftX;
    sy = shiftY;

    iniViewP[0] = 0.;
    iniViewP[1] = 0.;
    iniViewP[2] = -distance;

    for (int y = 0; y < viewH; ++y) {
        for (int x = 0; x < viewW; ++x) {
            xx = (float)x * (float)side / (float)viewW;
            yy = (float)y * (float)side / (float)viewH;
            iniEndP[0] = (xx - (float)side/2) / (distance - 50);// - (float)side / 2);
            iniEndP[1] = (yy - (float)side/2) / (distance - 50);// - (float)side / 2);
            iniEndP[2] = -distance + 1;

            rayRotation(iniViewP, rotViewP, iniEndP, rotEndP, sx, sy, angle);
            vector cross = crossPoint(rotViewP[0], rotViewP[1], rotViewP[2], angle[0], angle[1], angle[2], distance, side);
            xx = cross.x;
            yy = cross.y;
            z = cross.z;
            rayPositions[3 * (x + y * viewW) + 0] = xx;
            rayPositions[3 * (x + y * viewW) + 1] = yy;
            rayPositions[3 * (x + y * viewW) + 2] = z;
            rayAngles[3 * (x + y * viewW) + 0] = angle[0];
            rayAngles[3 * (x + y * viewW) + 1] = angle[1];
            rayAngles[3 * (x + y * viewW) + 2] = angle[2];

            if (!checkVectorOne(cross)){
//                trace3d(angle, xx, yy, z, dot);
//                setVec3Float(&pixel[(x + y * viewW) * 3], dot);
//                dot[0] = 1.;
//                dot[1] = 1.;
//                dot[2] = 1.;
            } else {
                dot[0] = 0.;
                dot[1] = 0.;
                dot[2] = 0.;
            }

//            if (y == plainY) {
//                rayBeginY[2 * x + 0] = rotViewP[0];
//                rayBeginY[2 * x + 1] = rotViewP[2];
//                rayAngleY[2 * x + 0] = angle[0];
//                rayAngleY[2 * x + 1] = angle[2];
//            }
//            if (x == plainX) {
//                rayBeginX[2 * y + 0] = rotViewP[0];
//                rayBeginX[2 * y + 1] = rotViewP[1];
//                rayAngleX[2 * y + 0] = angle[0];
//                rayAngleX[2 * y + 1] = angle[1];
//            }
//            std::cout << iniViewP[0] << " " << iniViewP[1] << " " << iniViewP[2] << " " << iniEndP[0] << " " << iniEndP[1] << " " << iniEndP[2] << std::endl;
//            std::cout << rotViewP[0] << " " << rotViewP[1] << " " << rotViewP[2] << " " << rotEndP[0] << " " << rotEndP[1] << " " << rotEndP[2] << std::endl;
//            std::cout << x << " " << y << " " << xx << " " << yy << " " << z << std::endl;
        }
    }
    gpuRender(distance, pixel, rayPositions, rayAngles);
    delete[] rayPositions;
    delete[] rayAngles;
//    visTrajectory(distance);
//    visRays(rayBeginX, rayAngleX, rayBeginY, rayAngleY, distance);
//    delete[] rayBeginX;
//    delete[] rayAngleX;
//    delete[] rayBeginY;
//    delete[] rayAngleY;
}

void renderlColorRotate(float distance, float *pixel) {
    float angle[3], dot[3], sx, sy, xx, yy, z;
    float iniViewP[3], iniEndP[3], rotViewP[3], rotEndP[3];
    int side = WIDTH;
//    float* rayBeginX = new float[WIDTH * 2];
//    float* rayAngleX = new float[WIDTH * 2];
//    float* rayBeginY = new float[WIDTH * 2];
//    float* rayAngleY = new float[WIDTH * 2];
    float* rayPositions = new float[viewH * viewW * 3];
    float* rayAngles = new float[viewH * viewW * 3];

    sx = shiftX;
    sy = shiftY;

    iniViewP[0] = 0.;
    iniViewP[1] = 0.;
    iniViewP[2] = -distance;

    for (int y = 0; y < viewH; ++y) {
        for (int x = 0; x < viewW; ++x) {
            xx = (float)x * (float)side / (float)viewW;
            yy = (float)y * (float)side / (float)viewH;
            iniEndP[0] = (xx - (float)side/2) / (distance - 50);// - (float)side / 2);
            iniEndP[1] = (yy - (float)side/2) / (distance - 50);// - (float)side / 2);
            iniEndP[2] = -distance + 1;

            rayRotation(iniViewP, rotViewP, iniEndP, rotEndP, sx, sy, angle);
            vector cross = crossPoint(rotViewP[0], rotViewP[1], rotViewP[2], angle[0], angle[1], angle[2], distance, side);
            xx = cross.x;
            yy = cross.y;
            z = cross.z;
            rayPositions[3 * (x + y * viewW) + 0] = xx;
            rayPositions[3 * (x + y * viewW) + 1] = yy;
            rayPositions[3 * (x + y * viewW) + 2] = z;
            rayAngles[3 * (x + y * viewW) + 0] = angle[0];
            rayAngles[3 * (x + y * viewW) + 1] = angle[1];
            rayAngles[3 * (x + y * viewW) + 2] = angle[2];
        }
    }
    gpuRenderlColor(distance, pixel, rayPositions, rayAngles);
    delete[] rayPositions;
    delete[] rayAngles;
}

void render(float distance, float *pixel) {
    float angle[3], dot[3];
    float xx, yy, z;
    z = 0;
    for (int y = 0; y < viewH; ++y) {
        for (int x = 0; x < viewW; ++x) {
            xx = (float)x * WIDTH / viewW;
            yy = (float)y * HEIGHT / viewH;
            angle[0] = (xx - WIDTH/2) / distance;
            angle[1] = (yy - HEIGHT/2) / distance;
            angle[2] = 1;
            trace3d(angle, xx, yy, z, dot);
            setVec3Float(&pixel[(x + y * viewW) * 3], dot);
        }
    }
    visualize();
}