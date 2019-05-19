// [header]
// A very basic raytracer example.
// [/header]
// [compile]
// c++ -o raytracer -O3 -Wall raytracer.cpp
// [/compile]
// [ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// [/ignore]
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <cfenv>
#include <stdlib.h>
#include <math.h>
#include <fenv.h>
#include <time.h>
#include <OpenCL/opencl.h>

#include <GLUT/glut.h>
#include <openGL/gl.h>
#include <openGL/glu.h>

#include "vector.h"
#include "myMath.h"
#include "gpuSetting.h"
#include "variables.h"
#include "gradient.h"
#include "objectData.h"
#include "lightPropergation.h"
#include "casting.h"

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif


//std::vector <float> surfaces;

void makeDataset(void){
    printf("initilizing...\n");
    float zero[] = {0, 0, 0}, one[] = {1, 1, 1};
    for (unsigned int i=0; i < FIELDSZ; ++i){
        setVec3Float(&grad[i * 3], zero);
        setVec3Float(&fColor[i * 3], one);
        setVec3Float(&fMedium[i * 3], one);
        fOpacity[i] = 0;
        fRefractivity[i] = 1.;
        setVec3Float(&lColor[i * 3], zero);
        setVec3Float(&lDirection[i * 3], zero);
        lIntensity[i] = 0;
    }

    printf("setting objects...\n");
//    field, center, radius, color, reflection, opacity, refraction, emissionColor
//    inSphere(field,  Vec3f (roundInt(WIDTH / 2), roundInt(WIDTH * 2 / 3), roundInt(WIDTH * 2 / 3)), roundInt(WIDTH / 3), Vec3f (1, 0.98, 0.98), 0., 0.001, 1.5, Vec3f(0., 0., 0.));
//    inSphere(field,  Vec3f (roundInt(WIDTH / 3), roundInt(WIDTH * 5 / 6), roundInt(WIDTH / 6)), roundInt(WIDTH / 6), Vec3f (0.98, 0.98, 1), 0., 0.01, 1.5, Vec3f(0., 0., 0.));

//    leaned
    float q1[] = {(float)WIDTH * 4 / 5, (float)HEIGHT - 11, (float)DEPTH / 5}, q2[] = {(float)WIDTH / 5, (float)HEIGHT - 11, (float)DEPTH * 3 / 5}, q3[] = {(float)WIDTH * 4 / 5, (float)HEIGHT / 2, (float)DEPTH / 5}, q4[] = {(float)WIDTH * 4 / 5, (float)HEIGHT - 11, (float)DEPTH / 4}, q[] = {1, 0.997, 0.997};
    inRectangle(q1, q2, q3, q4, q, 0.3, 0.01, 1.8, fColor, fMedium, fOpacity, fRefractivity);

    // straight
//    float q1[] = {(float)WIDTH * 4 / 5, (float)HEIGHT - 11, (float)DEPTH / 5}, q2[] = {(float)WIDTH / 5, (float)HEIGHT - 11, (float)DEPTH / 5}, q3[] = {(float)WIDTH * 4 / 5, (float)HEIGHT / 2, (float)DEPTH / 5}, q4[] = {(float)WIDTH * 4 / 5, (float)HEIGHT - 11, (float)DEPTH / 4}, q[] = {1, 0.97, 0.97};
//    inRectangle(q1, q2, q3, q4, q, 0.3, 0.01, 1.8, fColor, fMedium, fOpacity, fRefractivity);

    float qq1[] = {0, (float)HEIGHT - 1, 0}, qq2[] = {0, (float)HEIGHT - 1, (float)DEPTH - 1}, qq3[] = {0, (float)HEIGHT - 10, 0}, qq4[] = {(float)WIDTH - 1, (float)HEIGHT - 1, 0}, qq[] = {1, 1, 1};
    inRectangle(qq1, qq2, qq3, qq4, qq, 0.3, 0.77, 1, fColor, fMedium, fOpacity, fRefractivity);
    float p1[] = {0, (float)HEIGHT - 1, 0}, p2[] = {(float)WIDTH - 1, (float)HEIGHT - 1 , 0}, p3[] = {0, (float)HEIGHT - 1, (float)DEPTH - 1}, p4[] = {(float)WIDTH - 1, (float)HEIGHT - 1, (float)DEPTH - 1}, c[] = {1., 1., 1.};
    inPlain(p1, p2, p3, p4, c, 0.3, 1.0, 1., fColor, fMedium, fOpacity, fRefractivity);
    float pp1[] = {0, 0, (float)DEPTH - 1}, pp2[] = {(float)WIDTH - 1, 0, (float)DEPTH - 1}, pp3[] = {0, (float)HEIGHT - 1, (float)DEPTH - 1}, pp4[] = {(float)WIDTH - 1, (float)HEIGHT - 1, (float)DEPTH - 1}, cc[] = {1., 1., 1.};
    inPlain(pp1, pp2, pp3, pp4, cc, 0.3, 1., 1., fColor, fMedium, fOpacity, fRefractivity);

    int meshes = 5;
    for (int row=0; row<meshes; ++row){
        for (int col=0; col<meshes; ++col){
            if ((row + col) % 2 == 0){
                float ppp1[] = {WIDTH / (float)meshes * (float)row, HEIGHT / (float)meshes * (float)col, (float)DEPTH - 1}, ppp2[] = {WIDTH / (float)meshes * (float)(row + 1) - 1, HEIGHT / (float)meshes * col, (float)DEPTH - 1}, ppp3[] = {WIDTH / (float)meshes * row, HEIGHT / (float)meshes * (col + 1) - 1, (float)DEPTH - 1}, ppp4[] = {WIDTH / (float)meshes * (row + 1) - 1, HEIGHT / (float)meshes * (col + 1) - 1, (float)DEPTH - 1}, ccc[] = {0.1, 0.1, 0.1};
                inPlain(ppp1, ppp2, ppp3, ppp4, ccc, 0.3, 1.0, 1., fColor, fMedium, fOpacity, fRefractivity);
            }
            if (col == meshes - 1){
                break;
            }
        }
    }
}

void myInit (char *progname)
{
    float aspect = (float) viewW / (float) viewH;
    glutInitWindowSize( viewW, viewH );
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow(progname);
    glClearColor (0.0, 0.0, 0.0, 1.0);
//    座標反転
    glPixelZoom(1.0f, -1.0f);
    glLoadIdentity();
}

void display( void )
{
//  座標調整
    glRasterPos2i(-1 , 1);
    glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "display" << std::endl;
    std::cout << "viewing size: " << viewW << " " << viewH << std::endl;

    float *image = new float[viewH * viewW * 3], *pixel = image;

    gpuRender(2 * WIDTH, pixel);
//    render(200, pixel);


    glDrawPixels(viewW, viewH, GL_RGB, GL_FLOAT, image);

    glFlush();
    std::cout << "display end" << std::endl;
    delete[] image;
}

void myReshape(int w, int h)
{
//    float aspect = (float) WIDTH / (float) HEIGHT;
    glViewport(0, 0, w, h);
    viewW = w;
    viewH = h;
//    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluPerspective(30.0, aspect, 1.0, 50.0);
//    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    viewH = HEIGHT;
    viewW = WIDTH;

    glutInit(&argc, argv);
    myInit(argv[0]);


    makeDataset();

    printf("calculating gradients...\n");
    gradRefraction(fRefractivity);

    printf("smoothing gradient...\n");
//    gradSmooth(3);
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();
//    gradGauss3SmoothWithCondition();

    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();
//    gradGauss3Smooth();

//    for (int z=0; z<DEPTH; ++z){
//        for (int x=0; x<WIDTH*4/5; ++x){
//            std::cout << (float)((int)(grad[(x + 60 * WIDTH + z * WIDTH * WIDTH) * 3] * 100)) << " ";
//        }
//        printf("\n");
//    }

//  field, light, direction, color, intensity, direction.z must be bigger than the others.
//    calcLightSource(field, fromLightSource, grad, Vec3f (1, 1, 1), Vec3f (1,1,1));

    float direction[] = {1, 1, 1}, color[] = {1, 1, 1};
    printf("calculating light directiton...\n");
    gpuCalcLightSource (direction, color);

    glutDisplayFunc(display);
    glutReshapeFunc (myReshape);
    glutMainLoop();

    delete[] grad;
    delete[] fColor;
    delete[] fMedium;
    delete[] fOpacity;
    delete[] fRefractivity;
    delete[] lColor;
    delete[] lDirection;
    delete[] lIntensity;

    return 0;
}