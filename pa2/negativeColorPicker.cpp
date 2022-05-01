/*
*  File: negativeColorPicker.cpp
*  Implements the negative color picker for CPSC 221 PA2.
*
*/

#include "negativeColorPicker.h"

NegativeColorPicker::NegativeColorPicker(PNG& inputimg)
{
  // complete your implementation below
  img = inputimg;
}

HSLAPixel NegativeColorPicker::operator()(PixelPoint p)
{
  // complete your implementation below
  HSLAPixel target = *img.getPixel(p.x,p.y);
  target.l = 1 - target.l;
  if (target.h + 180 > 360){
    target.h -= 180;
  } else{
    target.h += 180;
  }  
  return target; 
}
