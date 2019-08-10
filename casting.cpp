//
// Created by Nozomi on 2019-05-11.
//

#include "casting.h"
#include "variables.h"
#include "myMath.h"
#include "vector.h"
#include "gpuSetting.h"

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

void visRays(float* rayBegin, float* rayAngle, int distance) {
    std::uniform_real_distribution<double> rnd(0.0,1.0);
    std::random_device rd;
    std::mt19937 mt(rd());
    int magnify = 5, magArrow = 100;
    cv::Scalar white(255, 255, 255);
    cv::Mat rayDir(cv::Size(distance * 3, distance * 3), CV_8UC3, white);

    int sx, sy, ex, ey;
    float dx, dy, thre = 0.3;
    for (int ray=0; ray<WIDTH; ++ray) {
        if (rnd(mt) < thre) {
            sx = (int) (rayBegin[2 * ray] + distance * 1.5);
            sy = (int) (rayBegin[2 * ray + 1] + distance * 1.5);
            ex = (int) (sx + rayAngle[2 * ray] * magArrow);
            ey = (int) (sy + rayAngle[2 * ray + 1] * magArrow);
//        std::cout << sx << " " << ex << " " << sy << " " << ey << " " << rayAngle[2 * ray + 1] * magArrow << std::endl;
            cv::arrowedLine(rayDir, cv::Point(sx, sy), cv::Point(ex, ey), cv::Scalar(255, 0, 0), 1, 4);
        }
    }
    cv::circle(rayDir, cv::Point((int)(distance * 1.5), (int)(distance * 1.5)), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDir, cv::Point((int)(distance * 1.5) + WIDTH, (int)(distance * 1.5)), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDir, cv::Point((int)(distance * 1.5) + WIDTH, (int)(distance * 1.5) + WIDTH), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    cv::circle(rayDir, cv::Point((int)(distance * 1.5), (int)(distance * 1.5) + WIDTH), 1, cv::Scalar(0, 0, 255), 10, 8, 0);
    imwrite("images/rayDirectionY.png", rayDir);
    printf("img written\n");
}

void biInter(float* tf, float* bf, float* tb, float* bb, float ratioy, float ratioz, float* value){
    float t[3], b[3];
    t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
    t[1] = tf[1] * (1 - ratioz) + tb[1] * ratioz;
    t[2] = tf[2] * (1 - ratioz) + tb[2] * ratioz;
    b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
    b[1] = bf[1] * (1 - ratioz) + bb[1] * ratioz;
    b[2] = bf[2] * (1 - ratioz) + bb[2] * ratioz;

    value[0] = t[0] * (1 - ratioy) + b[0] * ratioy;
    value[1] = t[1] * (1 - ratioy) + b[1] * ratioy;
    value[2] = t[2] * (1 - ratioy) + b[2] * ratioy;
}

void trilinearTrace(float xx, float yy, float z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y){
    unsigned int p;
    if ((xx < 0) || (yy < 0) | (z < 0) || (xx >= WIDTH - 1) || (yy >= HEIGHT - 1) || (z >= DEPTH - 1)) {
        xx = checkRange(xx, WIDTH);
        yy = checkRange(yy, WIDTH);
        z = checkRange(z, WIDTH);
        flagEnd = true;
        p = ((int) (xx) + (int) (yy) * WIDTH + (int) z * WIDTH * HEIGHT);
        opacity = fOpacity[p];
        medium[0] = fColor[3 * p];
        medium[1] = fColor[3 * p + 1];
        medium[2] = fColor[3 * p + 2];
        if (opacity == 0) {
            opacity = 1;
            medium[0] = 1;
            medium[1] = 1;
            medium[2] = 1;
        }
        color[0] = lColor[3 * p] * medium[0];
        color[1] = lColor[3 * p + 1] * medium[1];
        color[2] = lColor[3 * p + 2] * medium[2];
        intensity = lIntensity[p];
    } else {
        float ltf[] = {std::floor(xx), std::floor(yy), std::floor(z)};
        float lbf[] = {std::floor(xx), std::floor(yy) + 1, std::floor(z)};
        float rtf[] = {std::floor(xx) + 1, std::floor(yy), std::floor(z)};
        float rbf[] = {std::floor(xx) + 1, std::floor(yy) + 1, std::floor(z)};
        float ltb[] = {std::floor(xx), std::floor(yy), std::floor(z) + 1};
        float lbb[] = {std::floor(xx), std::floor(yy) + 1, std::floor(z) + 1};
        float rtb[] = {std::floor(xx) + 1, std::floor(yy), std::floor(z) + 1};
        float rbb[] = {std::floor(xx) + 1, std::floor(yy) + 1, std::floor(z) + 1};

        float ratiox = std::abs(xx - std::floor(xx));
        float ratioy = std::abs(yy - std::floor(yy));
        float ratioz = std::abs(z - std::floor(z));

        float tf[3], bf[3], tb[3], bb[3], t[3], b[3];

        medium[0] = fColor[getCoordinate(ltf)];
        medium[1] = fColor[getCoordinate(ltf) + 1];
        medium[2] = fColor[getCoordinate(ltf) + 2];

        color[0] = lColor[getCoordinate(ltf)];
        color[1] = lColor[getCoordinate(ltf) + 1];
        color[2] = lColor[getCoordinate(ltf) + 2];
        color[0] = color[0] * medium[0];
        color[1] = color[1] * medium[1];
        color[2] = color[2] * medium[2];

        //    reflectivity = fReflectivity[getScalarCoo(ltf)];
        opacity = fOpacity[getScalarCoo(ltf)];

        tf[0] = grad[getCoordinate(ltf)] * (1 - ratiox) + grad[getCoordinate(rtf)] * ratiox;
        tf[1] = grad[getCoordinate(ltf) + 1] * (1 - ratiox) + grad[getCoordinate(rtf) + 1] * ratiox;
        tf[2] = grad[getCoordinate(ltf) + 2] * (1 - ratiox) + grad[getCoordinate(rtf) + 2] * ratiox;
        bf[0] = grad[getCoordinate(lbf)] * (1 - ratiox) + grad[getCoordinate(rbf)] * ratiox;
        bf[1] = grad[getCoordinate(lbf) + 1] * (1 - ratiox) + grad[getCoordinate(rbf) + 1] * ratiox;
        bf[2] = grad[getCoordinate(lbf) + 2] * (1 - ratiox) + grad[getCoordinate(rbf) + 2] * ratiox;
        tb[0] = grad[getCoordinate(ltb)] * (1 - ratiox) + grad[getCoordinate(rtb)] * ratiox;
        tb[1] = grad[getCoordinate(ltb) + 1] * (1 - ratiox) + grad[getCoordinate(rtb) + 1] * ratiox;
        tb[2] = grad[getCoordinate(ltb) + 2] * (1 - ratiox) + grad[getCoordinate(rtb) + 2] * ratiox;
        bb[0] = grad[getCoordinate(lbb)] * (1 - ratiox) + grad[getCoordinate(rbb)] * ratiox;
        bb[1] = grad[getCoordinate(lbb) + 1] * (1 - ratiox) + grad[getCoordinate(rbb) + 1] * ratiox;
        bb[2] = grad[getCoordinate(lbb) + 2] * (1 - ratiox) + grad[getCoordinate(rbb) + 2] * ratiox;
        biInter(tf, bf, tb, bb, ratioy, ratioz, gradient);

        tf[0] = lIntensity[getScalarCoo(ltf)] * (1 - ratiox) + lIntensity[getScalarCoo(rtf)] * ratiox;
        bf[0] = lIntensity[getScalarCoo(lbf)] * (1 - ratiox) + lIntensity[getScalarCoo(rbf)] * ratiox;
        tb[0] = lIntensity[getScalarCoo(ltb)] * (1 - ratiox) + lIntensity[getScalarCoo(rtb)] * ratiox;
        bb[0] = lIntensity[getScalarCoo(lbb)] * (1 - ratiox) + lIntensity[getScalarCoo(rbb)] * ratiox;
        t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
        b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
        intensity = t[0] * (1 - ratioy) + b[0] * ratioy;
    }
}


void trace3d(float *angle, float x, float y, float z, float *pixel) {
    float xx = x, yy = y;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, opacity = 0, path[3];
    float calcStep = 1;
    while (!(xx < 0) || !(yy < 0) || !(z < 0)|| !(xx > WIDTH - 1) || !(yy > WIDTH - 1) || !(z > WIDTH - 1)){
        float newColor[3], newMedium[3], newReflectivity, newGradient[3], newIntensity, newOpacity;
        bool flagEnd = false;

        trilinearTrace(xx, yy, z, newColor, newMedium, newReflectivity, newOpacity, newGradient, newIntensity, flagEnd, x, y);
        float vec1[3], vec2[3];

        // update color += medium * (1 - opacity) * newOpacity * newColor;
        multiFloat(medium, (1 - opacity) * newOpacity, vec1);
        multiVec(vec1, newColor, vec1);
        addVec(color, vec1, color);

//      update medium *= newMedium;
        multiVec(medium, newMedium, medium);
        opacity += (1 - opacity) * newOpacity;

        int AOIx = -1, AOIy = -1;

        if (flagEnd){
            if ((x == AOIx) && (y == AOIy)) {
                std::cout << "break1 " << z << " " << opacity << " " << newOpacity << std::endl;
            }
            break;
        }
        if (std::abs(opacity - 1.0) < 0.001){
            if ((x == AOIx) && (y == AOIy)){
                std::cout << "break2 " << z << ", " << opacity  << ", " << newOpacity << std::endl;
            }
            break;
        }
//      update angle +=  newGradient;
        multiFloat(newGradient, 100 / (float)WIDTH, newGradient);
        multiFloat(newGradient, 1 / calcStep, newGradient);
        addVec(angle, newGradient, angle);
        vecNormalize(angle, path);
        if ((x == AOIx) && (y == AOIy)){
//            std::cout << path[0] << newGradient[0] / (float)WIDTH << std::endl;
        }

        if (y == plainY) {
            castPositionY[castIndexY * 2] = xx;
            castPositionY[castIndexY * 2 + 1] = z;
            castIndexY += 1;
        }

        if (x == plainX) {
            castPositionX[castIndexX * 2] = yy;
            castPositionX[castIndexX * 2 + 1] = z;
            castIndexX += 1;
        }

        xx += path[0] / calcStep;
        yy += path[1] / calcStep;
        z += path[2] / calcStep;
    }
    setVec3Float(pixel, color);
}

// view point is always (0, 0, -distance)
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
        if (!checkRangeVector(out1, WIDTH)) {
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
        if (!checkRangeVector(out2, WIDTH)) {
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
        if (compareDistance(out1, out2, big)) {
            out = out2;
        } else {
            return out1;
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

vector crossPoint(float inix, float iniy, float iniz, float anglex, float angley, float anglez, float distance, float side) {
    // plains ax+by+cz+d=0;
    plain p0 = {1., 0., 0., side};
    plain p1 = {1., 0., 0., 0};
    plain p2 = {0., 1., 0., side};
    plain p3 = {0., 1., 0., 0};
    plain p4 = {0., 0., 1., side};
    plain p5 = {0., 0., 1., 0};

    vector big = {inix, iniy, iniz};
    vector end = {inix + anglex * distance * 2, iniy + angley * distance * 2, iniz + anglez * distance * 2};

    vector out0 = compare(p0, p1, big, end);
    vector out1 = compare(p2, p3, big, end);
    vector out2 = compare(p4, p5, big, end);

    float dis0 = vectorDis(big, out0);
    float dis1 = vectorDis(big, out1);
    float dis2 = vectorDis(big, out2);

    vector can0, can1, can2;
    can0 = (dis0 <= dis1)? ((dis0 <= dis2)? out0 : out2) : ((dis1 <= dis2)? out1 : out2);
    can2 = (dis0 >= dis1)? ((dis0 >= dis2)? out0 : out2) : ((dis1 >= dis2)? out1 : out2);
    can1 = (checkSameVector(can0, out0)) ? (checkSameVector(can2, out1) ? out2 : out1) : (checkSameVector(can2, out0)? (checkSameVector(can0, out1)? out1 : out2) : out0);

    if (!checkVectorOne(can0)) {
        return can0;
    } else if (!checkVectorOne(can1)) {
        return can1;
    } else {
        return can2;
    }
}

void renderRotate(float distance, float *pixel) {
    float angle[3], dot[3], iniViewP[3], iniEndP[3], rotViewP[3], rotEndP[3], sx, sy, xx, yy, z;
    int side = WIDTH;
    sx = M_PI * 2;
    sy = 0;

    float* rayBegin = new float[WIDTH * 2];
    float* rayAngle = new float[WIDTH * 2];

    iniViewP[0] = (float)side/2;
    iniViewP[1] = (float)side/2;
    iniViewP[2] = -distance;

    rotViewP[0] = iniViewP[0];
    rotViewP[1] = iniViewP[1] * cos(sy) - iniViewP[2] * sin(sy);
    rotViewP[2] = iniViewP[1] * sin(sy) + iniViewP[2] * cos(sy);

    rotViewP[0] = rotViewP[0] * cos(sx) + rotViewP[2] * sin(sx);
    rotViewP[1] = rotViewP[1];
    rotViewP[2] = - rotViewP[0] * sin(sx) + rotViewP[2] * cos(sx);

    for (int y = 0; y < viewH; ++y) {
        for (int x = 0; x < viewW; ++x) {
            xx = (float)x;
            yy = (float)y;

            iniEndP[0] = (float)side / 2 + (xx - (float)side/2) / (distance - 50);// - (float)side / 2);
            iniEndP[1] = (float)side / 2 + (yy - (float)side/2) / (distance - 50);// - (float)side / 2);
            iniEndP[2] = -distance + 1;

            rotEndP[0] = iniEndP[0];
            rotEndP[1] = iniEndP[1] * cos(sy) - iniEndP[2] * sin(sy);
            rotEndP[2] = iniEndP[1] * sin(sy) + iniEndP[2] * cos(sy);

            rotEndP[0] = rotEndP[0] * cos(sx) + rotEndP[2] * sin(sx);
            rotEndP[1] = rotEndP[1];
            rotEndP[2] = - rotEndP[0] * sin(sx) + rotEndP[2] * cos(sx);

            angle[0] = - rotViewP[0] + rotEndP[0];
            angle[1] = - rotViewP[1] + rotEndP[1];
            angle[2] = - rotViewP[2] + rotEndP[2];

            if (y == plainY) {
                rayBegin[2 * x + 0] = rotViewP[0];
                rayBegin[2 * x + 1] = rotViewP[2];
                rayAngle[2 * x + 0] = angle[0];
                rayAngle[2 * x + 1] = angle[2];
            }

            vector cross = crossPoint(rotViewP[0], rotViewP[1], rotViewP[2], angle[0], angle[1], angle[2], distance, side);
            xx = cross.x;
            yy = cross.y;
            z = cross.z;

//            printf("\n");
//            std::cout << iniViewP[0] << " " << iniViewP[1] << " " << iniViewP[2] << " " << iniEndP[0] << " " << iniEndP[1] << " " << iniEndP[2] << std::endl;
//            std::cout << rotViewP[0] << " " << rotViewP[1] << " " << rotViewP[2] << " " << rotEndP[0] << " " << rotEndP[1] << " " << rotEndP[2] << std::endl;
//            std::cout << x << " " << y << " " << xx << " " << yy << " " << z << std::endl;

            if (!checkVectorOne(cross)){
                trace3d(angle, xx, yy, z, dot);
                dot[0] = 1.;
                dot[1] = 1.;
                dot[2] = 1.;
            } else {
                dot[0] = 0.;
                dot[1] = 0.;
                dot[2] = 0.;
            }
            setVec3Float(&pixel[(x + y * viewW) * 3], dot);
        }
    }

    std::cout << rayBegin[0] << " " << rayBegin[1] << " " << rayAngle[0] << " " << rayAngle[1] << std::endl;
    visRays(rayBegin, rayAngle, distance);

    delete[] rayBegin;
    delete[] rayAngle;
}

void gpuRender(float distance, float *pixel){
    std::cout << "rendering..." << std::endl;
    cl_int side = WIDTH;
    cl_int dis = (int)distance;
    cl_int w = viewW;
    cl_int h = viewH;

    float* checker;
    checker = (float *)malloc(sizeof(float) * w * h);

    clock_t start,end;
    start = clock();

    // プラットフォーム一覧を取得
    cl_platform_id platforms[PLATFORM_MAX];
    cl_uint platformCount;

    EC(clGetPlatformIDs(PLATFORM_MAX, platforms, &platformCount), "clGetPlatformIDs");
    if (platformCount == 0) {
        std::cerr << "No platform.\n";
        exit(1);
    }

    // 見つかったプラットフォームの情報を印字
//    for (int i = 0; i < platformCount; i++) {
//        char vendor[100] = {0};
//        char version[100] = {0};
//        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, nullptr), "clGetPlatformInfo");
//        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, nullptr), "clGetPlatformInfo");
//        std::cout << "Platform id: " << platforms[i] << ", Vendor: " << vendor << ", Version: " << version << "\n";
//    }

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
    }

    // 見つかったデバイスの情報を印字
//    std::cout << deviceCount << " device(s) found.\n";
//    for (int i = 0; i < deviceCount; i++) {
//        char name[100] = {0};
//        size_t len;
//        EC(clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(name), name, &len), "clGetDeviceInfo");
//        std::cout << "Device id: " << i << ", Name: " << name << "\n";
//    }

    // コンテキストの作成
    cl_context ctx = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    EC2("clCreateContext");

    FILE *fp;
//    char fileName[] = "./trace.cl";
    char fileName[] = "./trace3d.cl";
    fp = fopen(fileName, "r");
    if(!fp) {
        std::cout << "no file.\n";
        exit(1);
    }

    char *source_str;
    size_t source_size;
    source_str = (char*)malloc(0x10000);
    source_size = fread(source_str, 1, 0x10000, fp);
    fclose(fp);

    // プログラムのビルド
    cl_program program = NULL;
    program = clCreateProgramWithSource(ctx, 1, (const char **)&source_str, (const size_t *)&source_size, &err);
    clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    // カーネルの作成
    cl_kernel kernel = clCreateKernel(program, "trace", &err);
    EC2("clCreateKernel");

    // コマンドキューの作成
    cl_command_queue q = clCreateCommandQueue(ctx, devices[0], 0, &err);
    EC2("clCreateCommandQueue");

    cl_mem memGrad = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, grad, &err);
    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fOpacity, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memlIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, lIntensity, &err);
    cl_mem memPixel = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW*3, pixel, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW, checker, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memGrad), "clSetKernelArg0");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memfColor), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memlColor), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memfOpacity), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memlIntensity), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_mem), &memPixel), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_int), &dis), "clSetKernelAjrg6");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_int), &side), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_mem), &memChecker), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_int), &w), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_int), &h), "clSetKernelArg7");

    size_t global_item_size = w * h;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memPixel, CL_TRUE, 0, sizeof(float) * w * h * 3, pixel, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * w * h, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    for (int position=0; position<w; ++position){
//        std::cout << checker[position + (int)(h * 70/ 100 * w)] << " ";
//    }
//    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memGrad), "clReleaseMemObject");
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfOpacity), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memlIntensity), "clReleaseMemObject");
    EC(clReleaseMemObject(memPixel), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    free(checker);
}


void renderlColor(float distance, float shiftX, float shiftY, float *pixel){

    std::cout << "rendering..." << std::endl;
    cl_int side = WIDTH;
    cl_int dis = (int)distance;
    cl_int w = viewW;
    cl_int h = viewH;
    cl_float sx = shiftX;
    cl_float sy = shiftY;

    float* checker;
    checker = (float *)malloc(sizeof(float) * w * h);

    clock_t start,end;
    start = clock();

    // プラットフォーム一覧を取得
    cl_platform_id platforms[PLATFORM_MAX];
    cl_uint platformCount;

    EC(clGetPlatformIDs(PLATFORM_MAX, platforms, &platformCount), "clGetPlatformIDs");
    if (platformCount == 0) {
        std::cerr << "No platform.\n";
        exit(1);
    }

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
    }

    // コンテキストの作成
    cl_context ctx = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    EC2("clCreateContext");

    FILE *fp;
    char fileName[] = "./tracelColor.cl";
    fp = fopen(fileName, "r");
    if(!fp) {
        std::cout << "no file.\n";
        exit(1);
    }

    char *source_str;
    size_t source_size;
    source_str = (char*)malloc(0x10000);
    source_size = fread(source_str, 1, 0x10000, fp);
    fclose(fp);

    // プログラムのビルド
    cl_program program = NULL;
    program = clCreateProgramWithSource(ctx, 1, (const char **)&source_str, (const size_t *)&source_size, &err);
    clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    // カーネルの作成
    cl_kernel kernel = clCreateKernel(program, "trace", &err);
    EC2("clCreateKernel");

    // コマンドキューの作成
    cl_command_queue q = clCreateCommandQueue(ctx, devices[0], 0, &err);
    EC2("clCreateCommandQueue");

    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fOpacity, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memlIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, lIntensity, &err);
    cl_mem memPixel = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW*3, pixel, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW, checker, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memfColor), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memlColor), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memfOpacity), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memlIntensity), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memPixel), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_int), &dis), "clSetKernelAjrg6");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_int), &side), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_mem), &memChecker), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_int), &w), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_int), &h), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_float), &sx), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 11, sizeof(cl_float), &sy), "clSetKernelArg7");

    size_t global_item_size = w * h;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memPixel, CL_TRUE, 0, sizeof(float) * w * h * 3, pixel, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * w * h, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    for (int position=0; position<w; ++position){r
//        std::cout << lColor[position + (int)(h * 50/ 100 * w)] << " ";
//    }
//    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfOpacity), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memlIntensity), "clReleaseMemObject");
    EC(clReleaseMemObject(memPixel), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    free(checker);
}