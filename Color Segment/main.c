#include "Bits_And_Files_Functions.h"
#include "Segments_Functions.h"

void main()
{
	unsigned char choice;
	unsigned char imageFileName[MAX_NAME_FILE], imageFileNameCom[MAX_NAME_FILE], PGM_Name[MAX_NAME_FILE];
	grayImage *img = NULL;
	grayImage *newImg = NULL;
	int segmentsAmount;
	imgPosList* segments = NULL;
	unsigned char threshold, max_gray_level;
	int flag = FALSE, done = FALSE;

	while (done == FALSE)
	{
		printf("Please choose one of the options:\n\n");
		printf("1. Read an image file in PGM:\n");
		printf("2. Find all segments\n");
		printf("3. Color the segments\n");
		printf("4. Save the colored image in a compressed format\n");
		printf("5. Compress and save the original image in a compressed format\n");
		printf("6. Convert a compressed image to PGM format\n");
		printf("7. To end the program\n\n");
		scanf("%hhu", &choice);
		switch (choice)
		{
		case 1:
		{
			if (flag == FALSE)
			{
				printf("please enter the image file name:\n");
				scanf("%s", imageFileName);
				img = readPGM(imageFileName);
			}
			else
			{
				freeImage(img);
				printf("please enter another image file name:\n");
				scanf("%s", imageFileName);
				img = readPGM(imageFileName);
			}
			printf("PGM file successfully loaded into memory\n");
			flag = TRUE;
			break;
		}
		case 2:
		{
			printf("please enter your threshold:\n");
			scanf("%hhu", &threshold);
			if (flag == FALSE)
				printf("you didn't enter a image file name. please press 1 to enter name:\n");
			else
				if (segments == NULL)
					segmentsAmount = findAllSegments(img, threshold, &segments);
				else
				{
					freeImgPosList(segments, segmentsAmount);
					segmentsAmount = findAllSegments(img, threshold, &segments);
				}
			break;
			printf("All the segments in the picture were found\n");
		}
		case 3:
		{
			if (segments == NULL)
				printf("first, you need to find the segments. please press 2 to find the segments");
			else
			{
				if (newImg == NULL)
					newImg = colorSegments(segments, segmentsAmount);
				else
				{
					freeImage(newImg);
					newImg = colorSegments(segments, segmentsAmount);
				}
			}
			printf("Coloring the segment in the picture was successfully completed\n");
			break;
		}
		case 4:
		{
			printf("Please enter a binary file name to save the compressed colored image:\n");
			scanf("%s", imageFileNameCom);
			printf("Please enter the max gray level (0-127):\n");
			scanf("%hhu", &max_gray_level);
			savedCompressed(imageFileNameCom, newImg, max_gray_level);
			printf("Compressing the colored picture was successfully completed\n");
			break;
		}
		case 5:
		{
			printf("Please enter a binary file name to save the compressed image:\n");
			scanf("%s", imageFileNameCom);
			printf("please enter new max level:\n");
			scanf("%hhu", &max_gray_level);
			savedCompressed(imageFileNameCom, img, max_gray_level);
			printf("Compressing the picture into file was successfully completed\n");
			break;
		}
		case 6:
		{
			printf("Please enter a binary file name that contains a compressed image:\n");
			scanf("%s", imageFileNameCom);
			printf("Please enter a PGM file name to save the compressed image:\n");
			scanf("%s", PGM_Name);
			convertCompressedImageToPGM(imageFileNameCom, PGM_Name);
			break;
		}
		case 7:
		{
			done = TRUE;
			break;
		}
		default:
		{
			printf("you enter illigeal choice\n");
			break;
		}
		}
	}
	if (segments)
		freeImgPosList(segments, segmentsAmount);
	if (newImg)
		freeImage(newImg);
	if (img)
		freeImage(img);
}