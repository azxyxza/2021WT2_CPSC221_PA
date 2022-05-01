/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages
// #include "ptree-private.h"

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  // add your implementation below
  clearHelper(root);
}

/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  // add your implementation below
  root = copyHelper(other.root);
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  // replace the line below with your implementation
  
  HSLAPixel avgc = calculateAvg(im, ul, w, h);
  Node *parent = new Node(ul, w, h, avgc, NULL, NULL);
  
  if (w <= 1 && h <= 1){
    return parent;
  }else if (h > w){
    unsigned int height_A = h/2;
    unsigned int height_B = h - h/2;
    pair<unsigned int, unsigned int> upperleft_B = make_pair(ul.first, ul.second + height_A);
    Node *c_A = BuildNode(im, ul, w, height_A);
    Node *c_B = BuildNode(im, upperleft_B, w, height_B);
    parent->A = c_A;
    parent->B = c_B;
  } else {
    unsigned int width_A = w/2;
    unsigned int width_B = w - w/2;
    pair<unsigned int, unsigned int> upperleft_B = make_pair(ul.first + width_A, ul.second);
    Node *c_A = BuildNode(im, ul, width_A, h);
    Node *c_B = BuildNode(im, upperleft_B, width_B, h);
    parent->A = c_A;
    parent->B = c_B;
  } 
  return parent;
}


////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  // add your implementation below
  unsigned int w = im.width();
  unsigned int h = im.height();
  root = BuildNode(im, make_pair(0, 0), w, h);
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  // add your implementation below
  Copy(other);
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  // add your implementation below
  if (this != &other){
    Clear();
    Copy(other);
  }
  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  // add your implementation below
  Clear();
  root = nullptr;
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  unsigned int w = root->width;
  unsigned int h = root->height;
  PNG result = PNG(w, h);
  renderHelper(root, result);
  return result;
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  // add your implementation below
  pruneHelper(root, tolerance);
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  // replace the line below with your implementation
  return sizeCounter(root);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  // replace the line below with your implementation
  return countLeave(root);
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  // add your implementation below
  FlipInHorizonal(root, root->width);
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  // add your implementation below
  FlipInVertical(root, root->height);
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

HSLAPixel PTree::calculateAvg(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h){
  HSLAPixel p = HSLAPixel();
  double sumHX = 0.0;
  double sumHY = 0.0;
  double sumS = 0.0;
  double sumL = 0.0;
  double sumA = 0.0;
  int count = 0;

  for (unsigned int x = 0; x < w; x++){
    for (unsigned int y = 0; y < h; y++){
      HSLAPixel *curr = im.getPixel(ul.first + x, ul.second + y);
      sumHX += Deg2X(curr->h);
      sumHY += Deg2Y(curr->h);
      sumS += curr->s;
      sumL += curr->l;
      sumA += curr->a;
      count ++;
    }
  }
  p.h = XY2Deg(sumHX/count, sumHY/count);
  p.s = sumS/count;
  p.l = sumL/count;
  p.a = sumA/count;

  return p;
}


void PTree::clearHelper(Node *sub){
  if(sub){
    clearHelper(sub -> A);
    clearHelper(sub -> B);
    sub -> A = nullptr;
    sub -> B = nullptr;
    
    delete sub;
  } 
}


Node* PTree::copyHelper(Node *sub){
  if(sub){
    Node *n = new Node(sub->upperleft, 
                        sub->width, 
                        sub->height, 
                        sub->avg, 
                        NULL, 
                        NULL);
    n->A = copyHelper(sub->A);
    n->B = copyHelper(sub->B);
    return n;
  } else {
    return NULL;
  }
}


void PTree::renderHelper(Node* tree, PNG& sub) const{
  if (tree){
    if (tree->A == NULL && tree->B == NULL){
      for (unsigned int x = 0; x < tree->width; x++){
        for (unsigned int y = 0; y < tree->height; y++){
          *sub.getPixel(tree->upperleft.first + x, tree->upperleft.second + y) = tree->avg;
        }
      }
    } else{
      renderHelper(tree->A, sub);
      renderHelper(tree->B, sub);
    }
  }
}


int PTree::sizeCounter(Node *sub) const{
  if (sub){
    return 1 + sizeCounter(sub->A) + sizeCounter(sub->B);
  }
  return 0;
}


int PTree::countLeave(Node *sub) const{
  int count = 0;
  if (sub){
    if (sub->A == NULL && sub->B == NULL){
      count ++;
    } else{
      count += countLeave(sub->A)+countLeave(sub->B);
    }
  }
  return count;
}

void PTree::FlipInHorizonal(Node *sub, unsigned int width){
  if (sub){
    FlipInHorizonal(sub->A, width);
    FlipInHorizonal(sub->B, width);
    sub->upperleft.first = width - sub->upperleft.first - sub->width;
  }

}

void PTree::FlipInVertical(Node *sub, unsigned int height){
  if (sub){
    FlipInVertical(sub->A, height);
    FlipInVertical(sub->B, height);
    sub->upperleft.second = height - sub->upperleft.second - sub->height;
  }
}

void PTree::pruneHelper(Node *sub, double tolerance){
  if(sub){
    if (needPrune(sub->A, sub->avg, tolerance)
    && needPrune(sub->B, sub->avg, tolerance)){
      clearHelper(sub->A);
      clearHelper(sub->B);
      sub->A = NULL;
      sub->B = NULL;
    }
    pruneHelper(sub->A, tolerance);
    pruneHelper(sub->B, tolerance);
  }
}

bool PTree::needPrune(Node* sub, HSLAPixel color, double tolerance){
  if (sub){
    if (sub->avg.dist(color) < tolerance){
      return needPrune(sub->A, color, tolerance)
      && needPrune(sub->B, color, tolerance);
    } else{
      return false;
    }
  }
  return true;
}

