#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "bmp.h"
#include "mtat.h"

int main(int argc, char const *argv[]) {
	const char* error;
	FILE* fp = fopen("img2_384x510_gray.bmp", "r");
	BMPImage* img = read_bmp(fp, &error);
	fclose(fp);

	BMPImage* binImg = binarize(img, 2, 5, &error);

	fp = fopen("test.bmp", "w");
	write_bmp(fp, binImg, &error);

	fclose(fp);
	free_bmp(img);
	free_bmp(binImg);
	return EXIT_SUCCESS;
}
