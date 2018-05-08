#ifndef __BITS_AND_FILES_FUNCTIONS_H
#define __BITS_AND_FILES_FUNCTIONS_H
#include "Segments_Functions.h"


//-----------------------------------------------------------------Consts----------------------------------------------------------
#define BYTE_SIZE 8
#define BIT_I_SET(I,TYPE) ((TYPE)(1) << (I))
#define MAX_NAME_FILE 15

//-----------------------------------------------------------------Functions Declaration----------------------------------------------------------
grayImage* readPGM(char* fname); //The function recieves a string: fname, which contains a picture in PGM format. the function allocate memory for grayImage, inserts the data which saved in the file into that grayImage and returns the grayImage address.
void savedCompressed(char* fileName, grayImage* image, unsigned char max_gray_level); //The function receives a string: fileName, a grayImage pointer: image and an unsigned char: max_gray_level. the function writes into 'fileName' file the compressed data in image according to max_gray_level.
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name); //The function receives two strings: compressed_file_name and pgm_file_name and write in pgm_file_name the compressed image which saved in compressed_file_name in PGM format.

#endif