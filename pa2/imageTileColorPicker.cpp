/*
*  File: imageTileColorPicker.cpp
*  Implements the image tile color picker for CPSC 221 PA2.
*
*/

#include "imageTileColorPicker.h"

ImageTileColorPicker::ImageTileColorPicker(PNG& otherimage) {
  // complete your implementation below
  img_other = otherimage;
  
}

HSLAPixel ImageTileColorPicker::operator()(PixelPoint p) {
  // complete your implementation below
  unsigned int width = img_other.width();  
  unsigned int height = img_other.height();
  return *img_other.getPixel(p.x % width, p.y % height);
}