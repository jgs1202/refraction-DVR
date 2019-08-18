//
// Created by Nozomi on 2019-07-29.
//

#include "animeEvents.h"
#include "variables.h"

#include <GLUT/glut.h>
//#include <openGL/gl.h>
//#include <openGL/glu.h>
#include <stdio.h>

void keyPressed(unsigned char key, int x, int y) {
    printf("push\n");
   if (key == 13) {
       updateState = !updateState;
   } else if (key == 32) {
       shiftX = 0.;
       shiftY = 0.;
   }
    glutPostRedisplay();
}

void myMouse( int button, int state, int x, int y )
{
    if (state == GLUT_DOWN) {
        switch(button) {
            case GLUT_LEFT_BUTTON:
                mButton = button;
                break;
            case GLUT_RIGHT_BUTTON:
                mButton = button;
                break;
        }
        xBegin = x;
        yBegin = y;
        dragState = true;
    } else {
        dragState = false;
    }
}
void myMotion( int x, int y )
{
    int xDisp, yDisp;
    xDisp = x - xBegin;
    yDisp = y - yBegin;
    switch (mButton) {
        case GLUT_LEFT_BUTTON:
            shiftX += (float)xDisp / 100;
            shiftY -= (float)yDisp / 100;
            break;
        case GLUT_RIGHT_BUTTON:
            break;
    }
    xBegin = x;
    yBegin = y;
    glutPostRedisplay();
}