#ifndef __SEGMENTS_FUNCTIONS_H
#define __SEGMENTS_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------Consts----------------------------------------------------------
#define FALSE -1
#define TRUE -2

//---------------------------------------------------------Structs Declarations----------------------------------------------------

typedef struct _grayImage
{
	unsigned short rows, cols;
	unsigned char **pixels;
}grayImage;

typedef unsigned short imgPos[2];

typedef struct _treeNode treeNode;
typedef struct _treeNodeListCell treeNodeListCell;
typedef struct _imgPosCell imgPostCell;

typedef struct _segment
{
	treeNode *root;
}Segment;

typedef struct _treeNode
{
	imgPos position;
	treeNodeListCell *next_possible_positions;
}treeNode;

typedef struct _treeNodeListCell
{
	treeNode *node;
	struct _treeNodeListCell *next;
}treeNodeListCell;

typedef struct _imgPosCell
{
	imgPos position;
	struct _imgPosCell *next, *prev;
}imgPosCell;

typedef struct _imgPosList {
	imgPosCell *head, *tail;
}imgPosList;

//-------------------------------------------------------Functions declarations--------------------------------------------------------
Segment findSingleSegment(grayImage *img, imgPos start, unsigned char threshold); //The function receives a greyImage pointer: img, imgPos: start and unsigned int: threshold. The function creates a Segment by scanning img, every connected pixel with grey level that is threshold distanece from start will be inserted to the Segment. the function returns that Segment.
int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments); //The function receives a grayImage pointer: img, an unsigned char: threshold and an imgPosList pointer address: segments. the function create an impPosList of all the segments in img in segments, and returns the size of segments (integer).
grayImage* colorSegments(imgPosList *segments, unsigned int size);  //The function receives imgPosList pointer: segments and an unsigned int: size. The function creates a grayImage which every segment in it will have the same gray level. the function returns the address of the new grayImage.
void freeImage(grayImage* image); //The function receives a grayImage pointer: image and free all allocated memory in image.
void checkMemoryAllocation(void *ptr); //The function receives a pointer: ptr, if ptr points at NULL, the function prints an appropriate messege and terminates the calling function.
unsigned char** calcNewGrayLevel(grayImage* image, unsigned char maxGray); //The function receives a greyImage pointer: image and an unsigned char: maxGray. the function allocate and returns an unsigned char matrix of the new gray levels of image according to maxGray.
void freeImgPosList(imgPosList* lst,int segmentsAmount);

#endif // !__SEGMENTS_FUNCTIONS_H