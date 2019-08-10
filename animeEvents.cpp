//
// Created by Nozomi on 2019-07-29.
//

#include "animeEvents.h"
#include "variables.h"

#include <stdio.h>

void keyPressed(unsigned char key, int x, int y) {
   if (key == 13) {
       updateState = !updateState;
   }
}
