/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////
HSLAPixel calculateAvg(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h);
void clearHelper(Node *sub);
Node* copyHelper(Node *sub);
void renderHelper(Node *tree, PNG& sub) const;
int sizeCounter(Node *root) const;
int countLeave(Node *sub) const;
void FlipInHorizonal(Node *sub, unsigned int width);
void FlipInVertical(Node *sub, unsigned int height);
void pruneHelper(Node *sub, double tolerance);
bool needPrune(Node* sub, HSLAPixel color, double tolerance);
#endif