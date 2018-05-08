#include"Bits_And_Files_Functions.h"

//--------------------------------------------------Functions Declarations-----------------------------------------------------
static void ShiftBitsLeft(unsigned char ch, short bitsNec);  //The function receives an unsigned char: ch and short: bitsNec and shift left the bits in ch 'bitNec' times.
static void checkOpeningFiles(void *ptr); //The function receives a pointer, and verify that it doesn't NULL. if it does- it terminates the calling function and prints an appropriate messege.
static long long calcNumOfBytes(unsigned short rows, unsigned short cols, unsigned char maxGray, short*bitNec); //The function receives two unsigned shorts: rows and cols, an unsigned char: maxGray and a short pointer: bitNec. the function calculates and updates the number of neccesary bits into bitNec. the function returnes the number of neccesary bytes after comprasion (long long).
static short findBitNec(unsigned char maxGray); //The function receives an unsigned char: maxGrey, calculates and returns the minimum bits neccesary (short).
static void freeUnsignedCharMartix(unsigned char**arr, int rows, int cols);//the function free the new metrix of pixels


//--------------------------------------------------Functions-----------------------------------------------------

grayImage* readPGM(char* fname)
{//the function read locals from PGM file to grayImage variable
	FILE* file;
	grayImage* img;//the returned value
	unsigned int ch = 0;
	short int i, j;//index variables
	int current;

	file = fopen(fname, "r"); //open the 'fname' file for reading
	checkOpeningFiles(file); //check if the file has opened seccessfully

	while (ch != '\n')
		fscanf(file, "%c", &ch);

	img = (grayImage*)malloc(sizeof(grayImage)); //allocate memory for img
	checkMemoryAllocation(img); //check allocation

	fscanf(file, "%hu %hu", &img->cols, &img->rows); //get cols and rows from the file
	img->pixels = (char**)malloc(img->rows * sizeof(char*)); //allocate memory for img pixels matrix
	checkMemoryAllocation(img->pixels); //check allocation

	fscanf(file, "%c", &ch); //gets the '\n' char
	ch = 0; //initialize ch
	fscanf(file,"%d",&ch);//gets the gray level
	for (i = 0; i < img->rows; i++)
	{
		img->pixels[i] = (char*)malloc(sizeof(char) * img->cols); //allocate memory for img pixels cols
		for (j = 0; j < img->cols; j++)
		{
			fscanf(file, "%d", &current); //it will pass over the '\n' and the spaces
			img->pixels[i][j] = (unsigned int)current; //update the pixels matrix
		}//for j
	}//for i

	fclose(file); //close fp

	return img;
}//readPGM

void savedCompressed(char* fileName, grayImage* image, unsigned char max_gray_level)
{//the function save the locals in the compressed shape by max_gray_level
	FILE* file;//file
	unsigned char** newImage; //matrix with the new gray levels
	int i, j; //index variables
	int indexRows = 0, indexCols = 0; //writing index
	short bitsNec; //number of necessary bits after compression
	long long bytesNec;//number of necessary bytes after compression
	unsigned char ch = 0;
	unsigned char mask;
	int flag = FALSE; //tells us if the commprassion is done
	short index;//index
	int mod; //tells us how many times we need to shift left char
	int counter = 0;

	file = fopen(fileName, "wb"); //open fileName to binary writing
	checkOpeningFiles(file); //check if fp has seccessfully opened
	newImage = calcNewGrayLevel(image, max_gray_level); //we create a matrix with the new updated gray levels
	bytesNec = calcNumOfBytes(image->rows, image->cols, max_gray_level, &bitsNec); //we calculate the number of bytes that are necessary for the compression
	index = 7;
	fwrite(&(image->rows), sizeof(unsigned short), 1, file); //writes in the file the num of rows
	fwrite(&(image->cols), sizeof(unsigned short), 1, file); //writes in the file the num of columns
	fwrite(&max_gray_level, sizeof(unsigned char), 1, file); //writes in the file the maximum gray level
	for (i = 0; i <= bytesNec && (flag == FALSE);)
	{
		for (j = bitsNec; j > 0; j--)
		{
			mask = BIT_I_SET(j - 1, unsigned char); //create mask
			ch = ch << 1; //shift left ch
			ch |= ((mask&newImage[indexRows][indexCols]) >> (j - 1)); //update ch
			counter++; //update counter
			if (counter == BYTE_SIZE) //if we have a whole byte
			{
				fwrite(&ch, sizeof(unsigned char), 1, file); //write that byte in the file
				counter = 0; //initialize counter
				i++; //promote i
			} //if
		} //for j

		indexCols++; //promote indexCols 

		if (indexCols == image->cols)
		{
			indexRows++; //promote indexRows
			indexCols = 0; //initialize indexCols
			if (indexRows == image->rows) //if it's also the last row
				flag = TRUE; //update flag to be TRUE
		} //if
	} //for i

	mod = bitsNec*image->rows*image->cols % BYTE_SIZE; //claculate mod
	ShiftBitsLeft(ch, mod);
	fwrite(&ch, sizeof(unsigned char), 1, file);  //insert the file the leftovers (ch)
	fclose(file); //close fp
	freeUnsignedCharMartix(newImage, image->rows, image->cols);
} //savedCompressed

void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name)
{//the function convert the compressed image to PGM file
	unsigned short rows, cols, colsIndex = 0;
	FILE* fp1, *fp2;
	unsigned char maxGray, tempNum = 255;
	short bitNec, count;//the counter help us to convert the char to his position in the relevant number (will be the power of 2)
	int i, j;//indexes
	long long byteSize; //receives the neccessary bytes after comprassion
	unsigned char ch, mask, temp, current = 0; //current will hold the current number that we want to print to pgm file

	fp1 = fopen(compressed_file_name, "rb"); //opens compressed_file_name for binary reading
	checkOpeningFiles(fp1); //checks if fp1 has opened seccessfully
	fp2 = fopen(pgm_file_name, "w"); //opens pgm_file_name for writing
	checkOpeningFiles(fp2); //checks if fp2 has opened seccessfully

							//get rows, cols and maxGray from compressed_file_name
	fread(&rows, sizeof(unsigned short), 1, fp1);
	fread(&cols, sizeof(unsigned short), 1, fp1);
	fread(&maxGray, sizeof(unsigned char), 1, fp1);

	bitNec = findBitNec(maxGray);
	byteSize = calcNumOfBytes(rows, cols, maxGray, &bitNec);
	count = bitNec - 1; //update count
						//write to file in PGM format
	fprintf(fp2, "P2\n");
	fprintf(fp2, "%hu %hu\n", cols, rows);
	fprintf(fp2, "%hhu\n", tempNum);
	for (i = 0; i < byteSize; i++)
	{
		fread(&ch, sizeof(unsigned char), 1, fp1); //get char from fp1
		for (j = 7; j >= 0; j--)
		{
			mask = BIT_I_SET(j, unsigned char); //create mask
			temp = mask&ch;
			if (temp != 0) //the bit wasn't 0
			{
				mask = BIT_I_SET(count, unsigned char); //create mask
				current = current | mask; //update current
			}
			count--;//update count
			if (count == -1)
			{
				current = (current * 255) / maxGray; //calculate current
				fprintf(fp2, "%d ", current); //print current to file
				current = 0; //initialize current
				count = bitNec - 1; //initialize count
				colsIndex++; //promote colsIndex
				if (colsIndex == cols) //if its the last column
				{
					colsIndex = 0; //initialize colsIndex
					fprintf(fp2, "\n");
				} //if
			} //if
		} //for j
	} //for i
	fclose(fp1);
	fclose(fp2);
} //convertCompressedImageToPGM

static void ShiftBitsLeft(unsigned char ch, short bitsNec)
{ //The function receives an unsigned char: ch and short: bitsNec and shift left the bits in ch 'bitNec' times.
	int i; //index variable
	for (i = 0; i < bitsNec; i++)
		ch = ch << 1; //shift left
}//ShiftBitsLeft

static void checkOpeningFiles(void *ptr)
{//the function check the success of opening file
	if (ptr == NULL)
	{
		fprintf(stderr, "opening file error!\n");
		exit(2);
	}
}//checkOpeningFiles

static long long calcNumOfBytes(unsigned short rows, unsigned short cols, unsigned char maxGray, short*bitNec)
{//the function calcaulate how many bytes we need for the compreesed image
	long long div, mod;
	*bitNec = findBitNec(maxGray); //calculate bitNex using the function findBitNec
	div = (*bitNec)*rows*cols / BYTE_SIZE; //calculate num of full bytes
	mod = (*bitNec)*rows*cols % BYTE_SIZE; //calculate num of part bytes

	if (mod > 0)
		return (div + 1);
	else
		return (div);

} //calcNumOfBits

unsigned char** calcNewGrayLevel(grayImage* image, unsigned char maxGray)
{//the function create new matrix of pixels by the maxGray
	unsigned char** newImg; //the returned image
	int i, j; //index variable

	newImg = (unsigned char**)malloc(sizeof(unsigned char*)*(image->rows));
	checkMemoryAllocation(newImg);

	for (i = 0; i < image->rows; i++)
	{
		newImg[i] = (unsigned char*)malloc(sizeof(unsigned char)*(image->cols));
		checkMemoryAllocation(newImg[i]);
		for (j = 0; j < image->cols; j++)
			newImg[i][j] = (image->pixels[i][j] * maxGray) / 255; //update newImg[i][j] after new gray level calculation
	} //for i

	return (newImg);
} //calcNewGrayLevel

void freeUnsignedCharMartix(unsigned char** arr, int rows, int cols)
{
	int i;
	for (i = 0; i<rows; i++)
		free(arr[i]);
	free(arr);
}

static short findBitNec(unsigned char maxGray)
{//the function find how many bits are necessary by the maxGray
	if (maxGray == 1)
		return 1;
	else if (maxGray < 3)
		return 2;
	else if (maxGray < 8)
		return 3;
	else if (maxGray < 16)
		return 4;
	else if (maxGray < 32)
		return 5;
	else if (maxGray < 64)
		return 6;
	else
		return 7;
}//findBitNec