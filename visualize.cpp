//
// Created by Nozomi on 2019-07-25.
//

#include "visualize.h"

void cvArrow(CvArr* img, CvPoint pt1, CvPoint pt2, CvScalar color, int thickness, int lineType, int shift)
{
    cvLine(img,pt1,pt2,color,thickness,lineType,shift);
    float vx = (float)(pt2.x - pt1.x);
    float vy = (float)(pt2.y - pt1.y);
    float v = sqrt( vx*vx + vy*vy );
    float ux = vx / v;
    float uy = vy / v;
    //矢印の幅の部分
    float w=5,h=10;
    CvPoint ptl,ptr;
    ptl.x = (int)((float)pt2.x - uy*w - ux*h);
    ptl.y = (int)((float)pt2.y + ux*w - uy*h);
    ptr.x = (int)((float)pt2.x + uy*w - ux*h);
    ptr.y = (int)((float)pt2.y - ux*w - uy*h);
    //矢印の先端を描画する
    cvLine(img,pt2,ptl,color,thickness,lineType,shift);
    cvLine(img,pt2,ptr,color,thickness,lineType,shift);
}
