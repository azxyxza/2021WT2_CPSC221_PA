/*
*  File: censorColorPicker.cpp
*  Implements the censor color picker for CPSC 221 PA2.
*
*/

#include "censorColorPicker.h"

#include <math.h> // gives access to sqrt function

/*
*  Useful function for computing the Euclidean distance between two PixelPoints
*/
double PointDistance(PixelPoint a, PixelPoint b) {
  unsigned int diff_x, diff_y;
  if (a.x > b.x)
    diff_x = a.x - b.x;
  else
    diff_x = b.x - a.x;
  if (a.y > b.y)
    diff_y = a.y - b.y;
  else
    diff_y = b.y - a.y;
  return sqrt(diff_x * diff_x + diff_y * diff_y);
}

HSLAPixel CensorColorPicker::AvgHSLA(unsigned int x, unsigned int y, PNG& inputimage, unsigned int b_width)
{
  HSLAPixel avgP = HSLAPixel();
  double sumH = 0.0, sumL = 0.0, sumS = 0.0, sumA = 0.0; 
  double avgH = 0.0, avgS = 0.0, avgL = 0.0, avgA = 0.0;
  for (unsigned int i = 0; i < (x + b_width); i++){
    for (unsigned int j = 0; j < (y + b_width); j++){
      sumH += inputimage.getPixel(i, j)->h;
      sumS += inputimage.getPixel(i, j)->s;
      sumL += inputimage.getPixel(i, j)->l;
      sumA += inputimage.getPixel(i, j)->a;
    }
  }
  avgH = sumH/(b_width*b_width);
  avgS = sumS/(b_width*b_width);
  avgL = sumL/(b_width*b_width);
  avgA = sumA/(b_width*b_width);
  avgP = HSLAPixel(avgH, avgS, avgL, avgA);
  return avgP;
}

PNG CensorColorPicker::blockfy(PNG& inputimage, unsigned int b_width)
{
  PNG blocky_img = PNG(inputimage.width(), inputimage.height());
  for (unsigned int x = 0; x < (blocky_img.width() - b_width); x += b_width){
    for (unsigned int y = 0; x < (blocky_img.height() - b_width); y += b_width){
      // single module have the same avg HSLA
      for (unsigned int i = 0; i < (x + b_width); i++){
        for (unsigned int j = 0; j < (y + b_width); j++){
          *blocky_img.getPixel(i,j) = AvgHSLA(x, y, blocky_img, b_width);
        }
      }
    }
  }
  return blocky_img;
}

CensorColorPicker::CensorColorPicker(unsigned int b_width, PixelPoint ctr, unsigned int rad, PNG& inputimage)
{
  // complete your implementation below
  blockwidth = b_width;     // width of a mosaic block in pixels.
  center = ctr;             // middle/center point of mosaic area
  radius = rad;             // maximum pixel distance away from center where the mosaic color will be applied
  img = inputimage;         // Original image used in flood fill algorithm
  blockyimg = blockfy(img, blockwidth);
}

HSLAPixel CensorColorPicker::operator()(PixelPoint p)
{ 
  // complete your implementation below
  if(PointDistance(p,center) <= radius) {
    return *blockyimg.getPixel(p.x, p.y);
  } else {
    return *img.getPixel(p.x,p.y);
  }
}
