#include"Segments_Functions.h"

//--------------------------------------------------Functions Declarations-----------------------------------------------------

static unsigned char** allocateColorMat(unsigned short rows, unsigned short cols); //The function receives two unsigned shorts: rows and cols, allocate memory of unsigned char matrix in that size (rows*cols) and returns that matrix.
static treeNodeListCell* allocateCell(int pos_rows, int pos_cols);  //The function receives two integers: pos_rows, pos_cols, allocates treeNodeListCell, allocates treeNode, and updates its position. The function returns the new allocated treeNodeListCell address.
static treeNode* allocateTreeNode(int row, int col); //The function receives two integers: row and cols, allocates TreeNode, inserts rows and cols into the position of the new node and returns that node.
static imgPosCell* allocateImgPosCell(imgPosCell *prev, imgPos start); //The function receives an imgPosCell pointer: prev and an imgPos: start. the function allocate memory for a new imgPosCell, updates its position to be start, and its previous to be prev. the function returns that new imgPosCell.
static int** createImgPosMat(int rows, int cols); //The function receives two integers: rows and cols and allocates memory for an integers matrix with row and cols size initialized with zeros. the function returns that matrix.
static treeNodeListCell* findSingleSegmentAux(char min, char max, int row, int col, grayImage *img, int** arr); //The function receives two chars: min and max, two integers: row and col, a greyImage pointer: img and an integers matrix: arr. The function creates a Segment by scanning img, every connected pixel with grey level that is threshold distanece from start will be inserted to the Segment. the function returns the head of the next possible positions of root in that Segment.
static treeNodeListCell* AddNeighborsToList(unsigned char min, unsigned char max, int row, int col, grayImage* img, int** arr); //The function receives two unsigned chars: min and max, two integers: row and col, a grayImage pointer: img, and an integers matrix: arr. the function checks the neigbors of the char in the matches row and col in img, (checks if its value is between min and max) and returns the head of the list it created.
static imgPosList createTheSegmentsList(Segment seg);//The function receives a Segment: seg, create an imgPosList from seg and returns it.
static void createTheSegmentsListAux(imgPosList* lst, imgPosCell* current, imgPosCell *prev, treeNode* root); //The function receives an imgPosList adress: lst, two imgPosCell addresses: current and prev and a treeNode address: root. the function creates an imgPosList in lst which starts from current position value (updates prev to be the previous imgPosListCell).
static void copyimgPosListArrays(imgPosList *seg, imgPosList *temp, int logicSize); //The function receives two imgPosList addresses: seg and temp and an integer: logicSize. the function copies seg to temp.
static void findRowsAndCols(imgPosList* segments, unsigned int size, unsigned short* rows, unsigned short* cols); //The function receives an imgPosList pointer: segments, an unsigned int: size and two unsigned short addresses rows and cols. the function runs over segments, and update rows and cols value to be the actual rows and cols that appear in segments.
static void freeBoolMatrix(int**arr, int rows, int cols); //The function receives an integers matrix: arr, and two integers: rows and cols and free all allocated memory in arr.
static void freeSegment(Segment seg); //the function receives a Segment: seg and free all allocated memory in seg.
static void freeSegmentAux(treeNode* root); //the function receives a Segment: seg and free all allocated memory in seg.

Segment findSingleSegment(grayImage *img, imgPos start, unsigned char threshold)
{
	Segment tree;//the return value
	int** arr; //bollian matrix
	unsigned char min, max; //the min and max value that the segments can get

	tree.root = allocateTreeNode(start[0], start[1]);
	min = (img->pixels[start[0]][start[1]]) - threshold;
	if ((int)min>(int)(img->pixels[start[0]][start[1]]))//if the minimum is smaller than 0
		min = 0;

	max = (img->pixels[start[0]][start[1]]) + threshold;
	if ((int)max < (int)img->pixels[start[0]][start[1]])//if the maximum is bigger than 255
		max = 255;

	tree.root = allocateTreeNode(start[0], start[1]);
	arr = createImgPosMat(img->rows, img->cols);

	findSingleSegmentAux(min, max, start[0], start[1], img, arr);
	freeBoolMatrix(arr, img->rows, img->cols);

	return tree;
}//findSingleSegment

int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments)
{
	int size = 0;//the return size
	imgPosCell* prev = NULL;
	Segment seg;
	imgPosList* segmentList, *temp;
	int i, j;//index variables
	int phySize = 1; //the phisical size 
	int logicSize = 0; //the logic size
	int index = 0;
	unsigned char min, max; //variables that hold the limits
	int** arr;//the bollian matrix

	arr = createImgPosMat(img->rows, img->cols);

	segmentList = (imgPosList*)malloc(sizeof(imgPosList));
	checkMemoryAllocation(segmentList);

	for (i = 0; i<img->rows; i++)
	{
		for (j = 0; j<img->cols; j++)
		{
			if (arr[i][j] == 0)
			{
				size++;
				arr[i][j] = 1;
				min = (img->pixels[i][j]) - threshold;
				if ((int)min>(int)(img->pixels[i][j]))//if the minimum is smaller than 0
					min = 0;
				max = (img->pixels[i][j]) + threshold;
				if ((int)max < (int)img->pixels[i][j])//if the maximum is bigger than 255
					max = 255;
				seg.root = allocateTreeNode(i, j);
				seg.root->next_possible_positions = findSingleSegmentAux(min, max, i, j, img, arr);
				if (logicSize < phySize)
				{
					segmentList[index] = createTheSegmentsList(seg);//the function will copy the locals to the list

					index++;
					logicSize++;
				}//if
				else
				{
					phySize *= 2;
					temp = (imgPosList *)malloc(phySize * sizeof(imgPosList));
					checkMemoryAllocation(temp);
					copyimgPosListArrays(segmentList, temp, logicSize);
					free(segmentList);
					segmentList = temp;
					segmentList[index] = createTheSegmentsList(seg);//the function will copy the locals to the list
					index++;
					logicSize++;
				}//else
				freeSegment(seg);
			}//if
		}
	}
	temp = (imgPosList *)malloc(logicSize * sizeof(imgPosList));
	checkMemoryAllocation(temp);

	copyimgPosListArrays(segmentList, temp, logicSize);
	free(segmentList);

	segmentList = temp;
	*segments = segmentList;

	freeBoolMatrix(arr, img->rows, img->cols);

	return size;
}//findAllSegments

grayImage* colorSegments(imgPosList *segments, unsigned int size)
{//the function create new grayImage that every segment has the same value in the image
	grayImage* img;//the returned value
	imgPostCell* current; //travelling imgPosCell pointer 
	unsigned short rows = 0, cols = 0;
	unsigned int i;
	int grayLevel;

	findRowsAndCols(segments, size, &rows, &cols); //update rows and cols using the function findRowsAndCols
	img = (grayImage*)malloc(sizeof(grayImage)); //allocate memory for img
	checkMemoryAllocation(img); //check allocation
	img->rows = rows; //update img rows
	img->cols = cols; //update img cols
	img->pixels = allocateColorMat(rows, cols); //allocate memory for the pixels matrix

	if (size == 1)
	{
		grayLevel = 255; //update grayLevel
		current = segments[0].head; //set current at segments[0].head
		while (current) //while current isn't NULL
		{
			img->pixels[current->position[0]][current->position[1]] = grayLevel; //update img
			current = current->next; //promote current
		}//while
	}//if
	else
	{ //size > 1
		for (i = 0; i < size; i++)
		{
			grayLevel = (i * 255) / (size - 1); //calculate grayLevel
			current = segments[i].head; //set current at segments[i].head
			while (current) //while current isn't NULL
			{
				img->pixels[current->position[0]][current->position[1]] = (unsigned char)grayLevel; //update img
				current = current->next; //promote current
			}//while
		}//for
	}//else

	return img;
}//colorSegments

void freeImage(grayImage* image)
{//the function free the image from the menu
	int i;//index
	for (i = 0; i < image->rows; i++)
		free(image->pixels[i]); //free the pixels rows
	free(image->pixels); // free image->pixels
	free(image); //free image
}//freeImage

void checkMemoryAllocation(void *ptr)
{//the function check the success of memory allocations
	if (ptr == NULL)
	{
		fprintf(stderr, "memory allocation error!\n");
		exit(1);
	}
}

static treeNodeListCell* findSingleSegmentAux(char min, char max, int row, int col, grayImage *img, int** arr)
{
	treeNodeListCell* start;//the return value
	treeNodeListCell* lst = AddNeighborsToList(min, max, row, col, img, arr);
	if (lst == NULL)
		return NULL;
	else
	{
		start = lst; //initialize start
		while (lst != NULL)
		{
			lst->node->next_possible_positions = findSingleSegmentAux(min, max, lst->node->position[0], lst->node->position[1], img, arr);
			lst = lst->next; //promote lst
		}//while
	}//else

	return start;
}//findSingleSegmentAux

static treeNode* allocateTreeNode(int row, int col)
{
	treeNode* res;//the return value

	res = (treeNode*)malloc(sizeof(treeNode)); //allocate memory for res
	checkMemoryAllocation(res); //check allocation

	res->position[0] = row;
	res->position[1] = col;

	res->next_possible_positions = NULL;
	return res;
}//allocateTreeNode

static void copyimgPosListArrays(imgPosList *seg, imgPosList *temp, int logicSize)
{
	int i;//index
	for (i = 0; i<logicSize; i++)
		temp[i] = seg[i];
}//copyimgPosListArrays

static imgPosList createTheSegmentsList(Segment seg)
{
	imgPosCell* prev = NULL; //imgPosCell pointer
	imgPosList lst; //The returned value

	lst.head = NULL; //update lst.head to be NULL
	lst.tail = NULL; //update lst.tail to be NULL

	createTheSegmentsListAux(&lst, lst.head, prev, seg.root); //create list

	return lst;
}//createTheSegmentsList

static void createTheSegmentsListAux(imgPosList* lst, imgPosCell* current, imgPosCell *prev, treeNode* root)
{
	imgPosCell* newNode; //imgPosCell pointer
	treeNodeListCell* currentTree;

	if (root == NULL) //if the tree is empty
		return;
	else
	{
		currentTree = root->next_possible_positions; //set currentTree at root->next_possible_positions (beginning of the list)
		newNode = allocateImgPosCell(prev, root->position); //create new imgPosCell and update its previous and position

		if (lst->head == NULL) //if lst is empty
			lst->head = newNode;
		else //lst isn't empty
			lst->tail->next = newNode;

		lst->tail = newNode;
		current = lst->tail;

		while (currentTree != NULL)
		{
			createTheSegmentsListAux(lst, current->next, current, currentTree->node);
			currentTree = currentTree->next;
		} //while
	} //else
}//createTheSegmentsListAux

static int** createImgPosMat(int rows, int cols)
{
	int** arr;//the return value
	int i;//index
	arr = (int**)malloc(rows * sizeof(int*)); //memory allocation
	checkMemoryAllocation(arr); //verify allocation
	for (i = 0; i < rows; i++)
	{
		arr[i] = (int*)calloc(cols, sizeof(int)); //memory allocation
		checkMemoryAllocation(arr[i]); //verify allocation
	}
	return arr;
}//createImgPosMat

static treeNodeListCell* AddNeighborsToList(unsigned char min, unsigned char max, int row, int col, grayImage* img, int** arr)
{
	treeNodeListCell* lst = NULL; //the current neighbor treeNodeListCell
	treeNodeListCell* res = NULL; //the returned value- keep the address of the head of lst

	if (row - 1 >= 0 && arr[row - 1][col] == 0 && img->pixels[row - 1][col] >= min &&img->pixels[row - 1][col] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row - 1, col);  //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row - 1, col);
		arr[row - 1][col] = 1;
	}
	if (row - 1 >= 0 && col - 1 >= 0 && arr[row - 1][col - 1] == 0 && img->pixels[row - 1][col - 1] >= min &&img->pixels[row - 1][col - 1] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row - 1, col - 1); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row - 1, col - 1);
		arr[row - 1][col - 1] = 1;
	}
	if (col - 1 >= 0 && arr[row][col - 1] == 0 && img->pixels[row][col - 1] >= min &&img->pixels[row][col - 1] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row, col - 1); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row, col - 1);
		arr[row][col - 1] = 1;
	}
	if (row + 1 < img->rows && col - 1 >= 0 && arr[row + 1][col - 1] == 0 && img->pixels[row + 1][col - 1] >= min &&img->pixels[row + 1][col - 1] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row + 1, col - 1); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row + 1, col - 1);
		arr[row + 1][col - 1] = 1;
	}
	if (row + 1 < img->rows && arr[row + 1][col] == 0 && img->pixels[row + 1][col] >= min &&img->pixels[row + 1][col] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row + 1, col); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row + 1, col);
		arr[row + 1][col] = 1;
	}
	if (row + 1 < img->rows && col + 1 < img->cols && arr[row + 1][col + 1] == 0 && img->pixels[row + 1][col + 1] >= min &&img->pixels[row + 1][col + 1] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row + 1, col + 1); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row + 1, col + 1);
		arr[row + 1][col + 1] = 1;
	}
	if (col + 1 < img->cols && arr[row][col + 1] == 0 && img->pixels[row][col + 1] >= min &&img->pixels[row][col + 1] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row, col + 1); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row, col + 1);
		arr[row][col + 1] = 1;
	}
	if (row - 1 >= 0 && col + 1 < img->cols && arr[row - 1][col + 1] == 0 && img->pixels[row - 1][col + 1] >= min &&img->pixels[row - 1][col + 1] <= max)
	{
		if (lst) //if lst isn't empty
		{
			lst->next = allocateCell(row - 1, col + 1); //update lst->next using allocateCell
			lst = lst->next;
		}
		else
			res = lst = allocateCell(row - 1, col + 1);
		arr[row - 1][col + 1] = 1;
	}
	return res; //return the head of lst we created
}//AddNeighborsToList

static imgPosCell* allocateImgPosCell(imgPosCell *prev, imgPos start)
{//the function alloacate imgPosCell
	imgPosCell* current;//the return value

	current = (imgPosCell*)malloc(sizeof(imgPosCell)); //allocate memory of imgPosCell
	checkMemoryAllocation(current); //check if allocation succeed

	current->position[0] = start[0]; //insert position data
	current->position[1] = start[1]; //insert position data
	current->prev = prev; //update current->prev
	current->next = NULL; //update current->next

	return current;
}//allocateImgPosCell

static treeNodeListCell* allocateCell(int pos_rows, int pos_cols)
{//the function allocate the cell of the list
	treeNodeListCell* cell;//the return value
	cell = (treeNodeListCell*)malloc(sizeof(treeNodeListCell)); //memory allocation
	checkMemoryAllocation(cell); //verify allocation
	cell->node = allocateTreeNode(pos_rows, pos_cols);
	cell->next = NULL;
	return cell;
}//allocateCell

static unsigned char** allocateColorMat(unsigned short rows, unsigned short cols)
{//the function allocate the color martix
	unsigned char** pixels; //the returned value
	int i; //index variable
	pixels = (unsigned char**)malloc(rows * sizeof(unsigned char*)); //memory allocation for pixels rows
	checkMemoryAllocation(pixels); //check allocation

	for (i = 0; i < rows; i++)
	{
		pixels[i] = (unsigned char*)malloc(cols * sizeof(unsigned char)); //memory allocation for pixels cols
		checkMemoryAllocation(pixels[i]); //check allocation
	} //for

	return pixels;
}//allocateColorMat


static void findRowsAndCols(imgPosList* segments, unsigned int size, unsigned short* rows, unsigned short* cols)
{//the function find how many rows and cols we have in the image
	unsigned int i; //index variable
	imgPosCell* current; //traveling imgPosCell pointer

	for (i = 0; i < size; i++)
	{
		current = (segments + i)->head; //set current to be segment[i] head
		while (current) //while current isn't NULL
		{
			if (current->position[0] >(*rows)) //if we found a position in segments with row index bigger than *rows
				*rows = current->position[0]; //update *rows to be that row index value

			if (current->position[1] > (*cols)) //if we found a position in segments with col index bigger than *cols
				*cols = current->position[1]; //update *cols to be that col index value.

			current = current->next; //promote current
		}
	}
	//update *rows and *cols (plus 1 because the row and col position starts from 0) 
	*rows = *rows + 1;
	*cols = *cols + 1;
}//findRowsAndCols

static void freeSegment(Segment seg)
{//the function free the segment
	if (seg.root)
	{
		freeSegmentAux(seg.root);
		free(seg.root);
	}//if
}//freeSegment

static void freeSegmentAux(treeNode* root)
{//the function help to free the segment
	treeNodeListCell* lst;
	treeNodeListCell* toDelete;
	if (root->next_possible_positions == NULL) //if there is nothing to free
		return;
	else
	{
		lst = root->next_possible_positions; //set lst at root->next_possible_positions
		while (lst) //while lst isn't NULL
		{
			freeSegmentAux(lst->node);
			toDelete = lst; //update toDelete to point at lst
			lst = lst->next; //promote lst
			free(toDelete->node); //free toDelete
		}

		free(root->next_possible_positions);
	}
}//freeSegmentAux

void freeImgPosList(imgPosList* lst, int segmentsAmount)
{
	int i;
	imgPosCell* current, *prev;
	for (i = 0; i<segmentsAmount; i++)
	{
		current = lst[i].head;
		while (current)
		{
			prev = current;
			current = current->next;
			free(prev);
		}
	}
	free(lst);
}

static void freeUnsignedCharMartix(unsigned char**arr, int rows, int cols)
{
	int i;//index

	for (i = 0; i< rows; i++)
		free(arr[i]);
	free(arr);
}//freeBoolMatrix

static void freeBoolMatrix(int**arr, int rows, int cols)
{
	int i;//index

	for (i = 0; i< rows; i++)
		free(arr[i]);
	free(arr);
}//freeBoolMatrix