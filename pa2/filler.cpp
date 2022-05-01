/*
*  File:        filler.cpp
*  Description: Implementation of functions in the filler namespace.
*
*/

/*
*  Performs a flood fill using breadth first search.
*
*  PARAM:  config - FillerConfig struct to setup the fill
*  RETURN: animation object illustrating progression of flood fill algorithm
*/
animation filler::FillBFS(FillerConfig& config) {
  // complete your implementation below
  // You should replace the following line with a
  // correct call to fill.
  return Fill<Queue>(config);
}

/*
*  Performs a flood fill using depth first search.
*
*  PARAM:  config - FillerConfig struct to setup the fill
*  RETURN: animation object illustrating progression of flood fill algorithm
*/
animation filler::FillDFS(FillerConfig& config) {
  // complete your implementation below
  // You should replace the following line with a
  // correct call to fill.
  return Fill<Stack>(config);
}

/*
*  Run a flood fill on an image starting at the seed point
*
*  PARAM:  config - FillerConfig struct with data for flood fill of image
*  RETURN: animation object illustrating progression of flood fill algorithm
*/
template <template <class T> class OrderingStructure> animation filler::Fill(FillerConfig& config)
{
  /*
  * You need to implement this function!
  *
  * This is the basic description of a flood-fill algorithm: Every fill
  * algorithm requires an ordering structure, which is passed to this
  * function via its template parameter. For a breadth-first-search
  * fill, that structure is a Queue, for a depth-first-search, that
  * structure is a Stack. To begin the algorithm, you simply place the
  * given point in the ordering structure, marking it processed
  * (the way you mark it is a design decision you'll make yourself).
  * We have a choice to either change the color, if appropriate, when we
  * add the point to the OS, or when we take it off. In our test cases,
  * we have assumed that you will change the color when a point is added
  * to the structure.
  * Until the structure is empty, you do the following:
  *
  * 1.     Remove a point from the ordering structure, and then...
  *
  *        1.    add its unprocessed neighbors (up/down/left/right) whose color values are
  *              within (or equal to) tolerance distance from the center,
  *              to the ordering structure, and
  *              mark them as processed.
  *        2.    if it is an appropriate frame, send the current PNG to the
  *              animation (as described below).
  *
  * 2.     When implementing your breadth-first-search and
  *        depth-first-search fills, you will need to explore neighboring
  *        pixels (up/down/left/right) in some order.
  *
  *        While the order in which you examine neighbors does not matter
  *        for a proper fill, you must use the same order as we do for
  *        your animations to come out like ours! The order you should put
  *        neighboring pixels **ONTO** the queue or stack is based on the
  *        following priority condition:
  *        ** MINIMUM COLOR DISTANCE FROM THE CURRENT PIXEL **
  *        Ties are broken first by minimum y-coordinate, then by minimum x-coordinate.
  *        The HSLAPixel.dist() function will be useful, and you should
  *        take advantage of the functionality in your PriorityNeighbours class.
  * 
  *        If you process the neighbours in a different order, your fill may
  *        still work correctly, but your animations will be different
  *        from the grading scripts!
  *
  * 3.     For every k pixels filled, **starting at the kth pixel**, you
  *        must add a frame to the animation, where k = frameFreq.
  *
  *        For example, if frameFreq is 4, then after the 4th pixel has
  *        been filled you should add a frame to the animation, then again
  *        after the 8th pixel, etc.  You must only add frames for the
  *        number of pixels that have been filled, not the number that
  *        have been checked. So if frameFreq is set to 1, a pixel should
  *        be filled every frame.
  *
  * 4.     Finally, as you leave the function, send one last frame to the
  *        animation. This frame will be the final result of the fill, and
  *        it will be the one we test against.
  *
  */

  int framecount = 0; // increment after processing one pixel; used for producing animation frames (step 3 above)
  animation anim;
  OrderingStructure<PixelPoint> os;
  PNG *img = &config.img;
  unsigned int imgHeight = img->height();
  unsigned int imgWidth = img->width();

  // complete your implementation below
  // HINT: you will likely want to declare some kind of structure to track
  //       which pixels have already been visited
  vector<vector<PixRecord>> imgRecord(imgHeight,vector<PixRecord>(imgWidth));
  for(unsigned int y = 0; y < imgHeight; y++) {
    for(unsigned int x = 0; x < imgWidth; x++) {
      PixRecord currPix; 
      currPix.pix = img->getPixel(x,y);
      currPix.processed = false;
      imgRecord[y][x] = currPix;
    }
  }
  os.Add(config.seedpoint);
  unsigned int x = config.seedpoint.x;
  unsigned int y = config.seedpoint.y;
  imgRecord[y][x].processed = true;
  fillPix(os,config,imgRecord,img);
  framecount += 1;
  if (framecount == config.frameFreq) {
    anim.addFrame(*img);
    framecount = 0;
  }
  
  while(!os.IsEmpty()) {
    if(fillPix(os,config,imgRecord,img)) {
      framecount += 1;
      if (framecount == config.frameFreq) {
        anim.addFrame(*img);
        framecount = 0;
      }
    }
  }
    
  if(framecount != 0 ) {
    anim.addFrame(config.img);  
  }
  return anim;
}


bool filler::fillPix(OrderingStructure<PixelPoint> &os, filler::FillerConfig& config, vector<vector<filler::PixRecord>> &imgRecord, PNG *img) {
  unsigned int x = os.Peek().x;
  unsigned int y = os.Peek().y;
  unsigned int height = img->height();
  unsigned int width = img->width();
  PriorityNeighbours ph = PriorityNeighbours(os.Peek().color);
  if(needFill(config,*imgRecord[y][x].pix)) {
    *img->getPixel(x,y) = (*config.picker)(os.Remove());
    // if(x - 1 >= 0 && y -1 >= 0) {
    //   if(!imgRecord[y-1][x-1].processed){
    //     ph.Insert(PixelPoint(x-1,y-1,*imgRecord[y-1][x-1].pix));
    //     imgRecord[y-1][x-1].processed = true;
    //   }
    // }
    // if(x+1 < width && y+1<height) {
    //   if(!imgRecord[y+1][x+1].processed){
    //     ph.Insert(PixelPoint(x+1,y+1,*imgRecord[y+1][x+1].pix));
    //     imgRecord[y+1][x+1].processed = true;
    //   } 
    // }
    // if(x-1>0 && y+1<height) {
    //   if(!imgRecord[y+1][x-1].processed){
    //     ph.Insert(PixelPoint(x-1,y+1,*imgRecord[y+1][x-1].pix));
    //     imgRecord[y+1][x-1].processed = true;
    //   } 
    // }
    // if(x+1<width && y-1>=0) {
    //   if(!imgRecord[y-1][x+1].processed){
    //     ph.Insert(PixelPoint(x+1,y-1,*imgRecord[y-1][x+1].pix));
    //     imgRecord[y-1][x+1].processed = true;
    //   } 
    // }
    if(withInImg(x-1,y,img)) {
      if(!imgRecord[y][x - 1].processed) {
        ph.Insert(PixelPoint(x-1, y, *imgRecord[y][x-1].pix));
        imgRecord[y][x - 1].processed = true;
      }
    }
    if(withInImg(x+1,y,img)) {
      if(!imgRecord[y][x+1].processed) {
        ph.Insert(PixelPoint(x+1, y, *imgRecord[y][x + 1].pix));
        imgRecord[y][x+1].processed = true;
      }
    }
    if(withInImg(x,y-1,img)) {
      if(!imgRecord[y-1][x].processed) {
        ph.Insert(PixelPoint(x,y-1,*imgRecord[y-1][x].pix));
        imgRecord[y-1][x].processed = true;
      }
      
    }
    if(withInImg(x,y+1,img)) {
      if(!imgRecord[y+1][x].processed) {
        ph.Insert(PixelPoint(x,y+1,*imgRecord[y+1][x].pix));
        imgRecord[y+1][x].processed = true;
      }
    }
  

    while(!ph.IsEmpty()) {
      os.Add(ph.Remove());
    }  

    return true;
  } else {
      os.Remove();
      return false;
  }
  
}

bool filler::needFill(FillerConfig& config, HSLAPixel pix) {
  return pix.dist(config.seedpoint.color) <= config.tolerance;
}

bool filler::withInImg(unsigned int x, unsigned int y, PNG *img) {
  int w = (int)img->width();
  int h = (int)img->height();
  int x_i = (int) x;
  int y_i = (int) y;
  return (x_i >=0 && y_i >= 0 && x_i<w && y_i<h);
}



