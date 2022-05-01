// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <math.h> // provides fmin and fabs functions

/**************************
 * MISCELLANEOUS FUNCTIONS *
 **************************/

/*
 * This function is NOT part of the ImgList class,
 * but will be useful for one of the ImgList functions.
 * Returns the "difference" between two hue values.
 * PRE: hue1 is a double between [0,360).
 * PRE: hue2 is a double between [0,360).
 *
 * The hue difference is the absolute difference between two hues,
 * but takes into account differences spanning the 360 value.
 * e.g. Two pixels with hues 90 and 110 differ by 20, but
 *      two pixels with hues 5 and 355 differ by 10.
 */
double HueDiff(double hue1, double hue2)
{
  return fmin(fabs(hue1 - hue2), fabs(hue2 + 360 - hue1));
}

/*********************
 * CONSTRUCTORS, ETC. *
 *********************/

/*
 * Default constructor. Makes an empty list
 */
ImgList::ImgList()
{
  // set appropriate values for all member attributes here
  northwest = nullptr;
  southeast = nullptr;
}

/*
 * Creates a list from image data
 * PRE: img has dimensions of at least 1x1
 */
ImgList::ImgList(PNG &img)
{
  northwest = NULL;
  southeast = NULL;
  ImgNode *head = NULL;
  ImgNode *upper = NULL;

  // build the linked node structure and set the member attributes appropriately
  for (unsigned y = 0; y < img.height(); y++)
  {
    ImgNode *left = NULL;
    for (unsigned x = 0; x < img.width(); x++)
    {
      ImgNode *curr = new ImgNode();
      HSLAPixel *pixel = img.getPixel(x, y);
      curr->colour = *pixel;
      if (x == 0)
      {
        upper = head;
        head = curr;
      }
      // 上
      curr->north = upper;
      if (upper != NULL)
      {
        upper->south = curr;
        upper = upper->east;
      }
      // 左
      curr->west = left;
      if (left != NULL)
      {
        left->east = curr;
      }
      // 边
      if (x == 0 && y == 0)
      {
        northwest = curr;
      }
      else if (x == img.width() - 1 && y == img.height() - 1)
      {
        southeast = curr;
      }
      left = curr;
    }
  }
}

/*
 * Copy constructor.
 * Creates this this to become a separate copy of the data in otherlist
 */
ImgList::ImgList(const ImgList &otherlist)
{
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
 * Assignment operator. Enables statements such as list1 = list2;
 *   where list1 and list2 are both variables of ImgList type.
 * POST: the contents of this list will be a physically separate copy of rhs
 */
ImgList &ImgList::operator=(const ImgList &rhs)
{
  // Re-build any existing structure using rhs as a template

  if (this != &rhs)
  { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();

    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }

  return *this;
}

/*
 * Destructor.
 * Releases any heap memory associated with this list.
 */
ImgList::~ImgList()
{
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
 * ACCESSORS *
 ************/

/*
 * Returns the horizontal dimension of this list (counted in nodes)
 * Note that every row will contain the same number of nodes, whether or not
 *   the list has been carved.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionX() const
{
  // replace the following line with your implementation
  int count = 0;
  ImgNode *curr = northwest;
  while (curr != NULL)
  {
    count++;
    curr = curr->east;
  }
  return count;
}

/*
 * Returns the vertical dimension of the list (counted in nodes)
 * It is useful to know/assume that the grid will never have nodes removed
 *   from the first or last columns. The returned value will thus correspond
 *   to the height of the PNG image from which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   y dimension.
 */
unsigned int ImgList::GetDimensionY() const
{
  // replace the following line with your implementation
  int count = 0;
  ImgNode *curr = northwest;
  while (curr != NULL)
  {
    count++;
    curr = curr->south;
  }
  return count;
}

/*
 * Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
 * The returned value will thus correspond to the width of the PNG image from
 *   which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionFullX() const
{
  // replace the following line with your implementation
  int count = 0;
  ImgNode *curr = northwest;
  while (curr != NULL)
  {
    count = count + 1 + curr->skipright;
    curr = curr->east;
  }
  return count;
}

/*
 * Returns a pointer to the node which best satisfies the specified selection criteria.
 * The first and last nodes in the row cannot be returned.
 * PRE: rowstart points to a row with at least 3 physical nodes
 * PRE: selectionmode is an integer in the range [0,1]
 * PARAM: rowstart - pointer to the first node in a row
 * PARAM: selectionmode - criterion used for choosing the node to return
 *          0: minimum luminance across row, not including extreme left or right nodes
 *          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 * A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
 * That is, a hue value of exactly 360 should be converted to 0.
 * The hue difference is the absolute difference between two hues,
 * but be careful about differences spanning the 360 value.
 * e.g. Two pixels with hues 90 and 110 differ by 20, but
 *      two pixels with hues 5 and 355 differ by 10.
 */
ImgNode *ImgList::SelectNode(ImgNode *rowstart, int selectionmode)
{
  // add your implementation below
  ImgNode *curr = rowstart->east;
  ImgNode *target = curr;

  while (curr->east != NULL)
  {
    if (selectionmode == 0)
    {
      double targetLum = target->colour.l;
      double currLum = curr->colour.l;
      if (currLum < targetLum)
      {
        target = curr;
      }
    }
    else if (selectionmode == 1)
    {
      double targetTotal = HueDiff(target->colour.h, target->west->colour.h) + HueDiff(target->colour.h, target->east->colour.h);
      double currTotal = HueDiff(curr->colour.h, curr->west->colour.h) + HueDiff(curr->colour.h, curr->east->colour.h);
      if (currTotal < targetTotal)
      {
        target = curr;
      }
    } else if (selectionmode == 2){
      double targetTotal = HueDiff(target->colour.h, target->west->colour.h) + HueDiff(target->colour.h, target->east->colour.h);
      double currTotal = HueDiff(curr->colour.h, curr->west->colour.h) + HueDiff(curr->colour.h, curr->east->colour.h);
      if (currTotal < targetTotal)
      {
        target = curr;
      }
    }
    curr = curr->east;
  }

  return target;
}

HSLAPixel* Avg(HSLAPixel* left, HSLAPixel* right){
  HSLAPixel *pixel = new HSLAPixel;
  double smallH = fmin(left->h, right->h);
  double diff = HueDiff(right->h, left->h);
  double avgh;
  if (diff == 180){
    if (smallH < 90){
      avgh = (smallH == left->h)? left->h + 90 : left->h - 90;
    } else{
      avgh = (smallH == left->h)? left->h - 90 : left->h + 90;
    }
  } else if (diff < fabs((left->h)-(right->h))){
    avgh = (smallH == left->h)? left->h - diff/2 : left->h + diff/2;
  } else{
    avgh = (smallH == left->h)? left->h + diff/2 : left->h - diff/2;
  }
  avgh = (avgh >= 0)? avgh : avgh + 360;
  avgh = (avgh < 360)? avgh : avgh - 360;
  
  double avgl = (left->l + right->l) / 2;
  double avgs = (left->s + right->s) / 2;
  double avga = (left->a + right->a) / 2;
  pixel->h = avgh;
  pixel->s = avgs;
  pixel->l = avgl;
  pixel->a = avga;
  return pixel;
}


/*
 * Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
 * PRE: fillmode is an integer in the range of [0,2]
 * PARAM: fillgaps - whether or not to fill gaps caused by carving
 *          false: render one pixel per node, ignores fillmode
 *          true: render the full width of the original image,
 *                filling in missing nodes using fillmode
 * PARAM: fillmode - specifies how to fill gaps
 *          0: solid, uses the same colour as the node at the left of the gap
 *          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
 *             Note that "average" for hue will use the closer of the angular distances,
 *             e.g. average of 10 and 350 will be 0, instead of 180.
 *             Average of diametric hue values will use the smaller of the two averages
 *             e.g. average of 30 and 210 will be 120, and not 300
 *                  average of 170 and 350 will be 80, and not 260
 *          2: *** OPTIONAL - FOR BONUS ***
 *             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
 *             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
 *             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
 *             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
 *             Like fillmode 1, use the smaller difference interval for hue,
 *             and the smaller-valued average for diametric hues
 */
PNG ImgList::Render(bool fillgaps, int fillmode) const
{
  PNG outpng; // this will be returned later. Might be a good idea to resize it at some point.
  ImgNode *curr = northwest;
  ImgNode *head = northwest;
  if (!fillgaps)
  {
    unsigned int w1 = GetDimensionX();
    unsigned int h1 = GetDimensionY();
    outpng = PNG(w1, h1);

    for (unsigned y = 0; y < h1; y++)
    {
      for (unsigned x = 0; x < w1; x++)
      {
        HSLAPixel *pixel = outpng.getPixel(x, y);
        *pixel = curr->colour;
        if (curr->east != NULL)
        {
          curr = curr->east;
        }
        else
        {
          head = head->south;
          if (head != NULL)
          {
            curr = head;
          }
        }
      }
    }
  }
  else
  {
    unsigned int w2 = GetDimensionFullX();
    unsigned int h2 = GetDimensionY();
    outpng = PNG(w2, h2);
    for (unsigned y = 0; y < h2; y++)
    {
      curr = head;
      head = head->south;
      for (unsigned x = 0; x < w2; x++)
      {
        HSLAPixel *pixel = outpng.getPixel(x, y);
        *pixel = curr->colour;
        int skip = curr->skipright;
        switch (fillmode){
          case 0:
            while (skip > 0){
              x++;
              HSLAPixel *pixel0 = outpng.getPixel(x, y);
              *pixel0 = curr->colour;
              skip--;
            }
            break;
          case 1:
            while(skip > 0){
                x++;
                HSLAPixel *pixel1 = outpng.getPixel(x, y);
                HSLAPixel *fillPix = Avg(&(curr->colour), &(curr->east->colour));
                *pixel1 = *fillPix;
                delete fillPix;
                fillPix = nullptr;
                skip--;
            }
            break;
        }
        if (curr->east != NULL)
        {
        curr = curr->east;
        }
      }
    }
  }
  return outpng;
}

/************
 * MODIFIERS *
 ************/

/*
 * Removes exactly one node from each row in this list, according to specified criteria.
 * The first and last nodes in any row cannot be carved.
 * PRE: this list has at least 3 nodes in each row
 * PRE: selectionmode is an integer in the range [0,1]
 * PARAM: selectionmode - see the documentation for the SelectNode function.
 * POST: this list has had one node removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(int selectionmode)
{
  // add your implementation here
  ImgNode *head = northwest;
  while (head != NULL)
  {
    ImgNode *del = SelectNode(head, selectionmode);
    ImgNode *left = del->west;
    ImgNode *right = del->east;
    left->east = right;
    left->skipright += (1 + del->skipright);
    right->west = left;
    right->skipleft += (1 + del->skipleft);
    if (del->north != NULL && del->south != NULL)
    {
      del->north->skipdown += (1 + del->skipdown);
      del->south->skipup += (1 + del->skipup);
      del->north->south = del->south;
      del->south->north = del->north;
    }
    else if (del->north != NULL)
    {
      del->north->skipdown += (1 + del->skipdown);
      del->north->south = del->south;
    }
    else if (del->south != NULL)
    {
      del->south->skipup += (1 + del->skipup);
      del->south->north = del->north;
    }
    head = head->south;
    delete del;
  }
}

// note that a node on the boundary will never be selected for removal
/*
 * Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
 * based on specific selection criteria.
 * Note that this should remove one node from every row, repeated "rounds" times,
 * and NOT remove "rounds" nodes from one row before processing the next row.
 * PRE: selectionmode is an integer in the range [0,1]
 * PARAM: rounds - number of nodes to remove from each row
 *        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
 *        i.e. Ensure that the final list has at least two nodes in each row.
 * POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(unsigned int rounds, int selectionmode)
{
  // add your implementation here

  if (rounds >= GetDimensionFullX())
  {
    rounds = GetDimensionFullX() - 2;
  }
  while (0 < rounds)
  {
    Carve(selectionmode);
    rounds--;
  }
}

/*
 * Helper function deallocates all heap memory associated with this list,
 * puts this list into an "empty" state. Don't forget to set your member attributes!
 * POST: this list has no currently allocated nor leaking heap memory,
 *       member attributes have values consistent with an empty list.
 */
void ImgList::Clear()
{
  // add your implementation here
  ImgNode *head = NULL;
  ImgNode *curr = NULL;
  ImgNode *right = NULL;
  head = northwest;
  while (head != NULL)
  {
    curr = head;
    head = head->south;
    while (curr != NULL)
    {
      right = curr->east;
      if (right){
        right->west = nullptr;
      }
      if (curr->south){
        curr->south->north = nullptr;
      }
      delete curr;
      curr = right;
    }
    
  }
}

/* ************************
 *  * OPTIONAL - FOR BONUS *
 ** ************************
 * Helper function copies the contents of otherlist and sets this list's attributes appropriately
 * PRE: this list is empty
 * PARAM: otherlist - list whose contents will be copied
 * POST: this list has contents copied from by physically separate from otherlist
 */
void ImgList::Copy(const ImgList &otherlist)
{
  // add your implementation here
}

/*************************************************************************************************
 * IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
 *************************************************************************************************/
