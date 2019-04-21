#include "bmp.h"
#include "mtat.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

int _getPadding(int w, int h, int bpp);
int _blackorwhite(int cornerIndex, int temp2, int temp1);
bool _isedge(int cnt, int w);

typedef struct{
	BMPImage* img1;
	BMPImage* img2;
	int startIndex;
	int endIndex;
	int radius;
	int height;
	int width;
	int padding;
}ATimg;

int _getPadding(int w, int h, int bpp){
	int res = (4 - (w * bpp) % 4);
	if (res == 4) {
		res = 0;
	}
	else {
		res *= h;
	}
	return res;
}

BMPImage* binarize(const BMPImage* image, int radius, int num_threads, const char** a_error){
	if (image == NULL) {
		*a_error = "Cannot open image. ";
		return NULL;
	}
	//copying new bmp file
	BMPImage* bmp = malloc(sizeof(*bmp));
	int imageSize = image->header.image_size_bytes;
	bmp->data = malloc(sizeof(char)*imageSize);
	bmp->header = image->header;

	//preserve image to bmpcopy
	BMPImage* bmpcopy = malloc(sizeof(*bmpcopy));
	bmpcopy->data = malloc(sizeof(char)*imageSize);
	for(size_t i = 0; i < imageSize; i++){
		bmpcopy->data[i] = image->data[i];
	}
	bmpcopy->header = image->header;


	ATimg* pimg = malloc(sizeof(*pimg)*num_threads);

	int bpp = image->header.bits_per_pixel / 8;
	int padding = _getPadding(image->header.width_px,image->header.height_px,bpp);
	for (size_t i = 0; i < num_threads; i++) {
		(pimg[i]).radius = radius;
		(pimg[i]).img1 = (BMPImage*)bmpcopy;
		(pimg[i]).img2 = bmp;
		(pimg[i]).startIndex = i * (imageSize - padding) / bpp / num_threads;
		(pimg[i]).endIndex = ((i + 1) * (imageSize - padding) / bpp / num_threads) - 1;
		(pimg[i]).height = bmpcopy->header.height_px;
		(pimg[i]).width = bmpcopy->header.width_px;
		(pimg[i]).padding = padding;
	}

	pthread_t* yeetThread = malloc(sizeof(*yeetThread)*num_threads);
	for (size_t i = 0; i < num_threads; i++) {
		pthread_create(&(yeetThread[i]), NULL, worker, &(pimg[i]));
	}
	for (size_t j = 0; j < num_threads; j++) {
		pthread_join(yeetThread[j], NULL);
	}
	free(yeetThread);
	free(pimg);
	free_bmp(bmpcopy);

	return bmp;
}

void* worker(void* arg){
	ATimg* inp = (ATimg*)arg;
	int bpp = inp->img1->header.bits_per_pixel / 8;	//Do Not Change
	unsigned int cnt = inp->startIndex;
	int height = inp->height;
	int width = inp->width;
	int radius = inp->radius;
	int padding = _getPadding(width,height,bpp);
	int temp = 0;
	int temp1 = 0;
	int cornerIndex = 0;
	int temp2 = 0;
	unsigned char* old = inp->img1->data;
	unsigned char* new = inp->img2->data;
	int paddata = 0;
	int row = 0;
	int index = 0;

	for(cnt = inp->startIndex;cnt<=inp->endIndex;cnt++){
		temp1 = 0;
		temp2 = 0;
		for(int j = -radius; j <= radius; j++) {
			for(int i = -radius; i <= radius; i++) {
				if(i + (cnt % width) < width && i + (cnt % width) >= 0) {
					if(j + (cnt / width) < height && j + (cnt / width) >= 0) {
						temp = 0;
						for(int k = 0; k < bpp; k++){
							temp += old[k + (cnt + j * width + i) * bpp + padding * ((cnt + j * width + i) / width)];
						}
						temp1 += temp;
						temp2++;
					}
				}
			}
		}
		cornerIndex = 0;
		row = cnt / width;
		for(int i = 0; i < bpp; i++) {
			index = i + cnt * bpp + padding * row;
			cornerIndex += old[index];
		}

		int bw = _blackorwhite(cornerIndex, temp2, temp1);

		for(int i = 0; i < bpp; i++) {
			index = i + cnt * bpp + padding * row;
			new[index] = bw;
		}

		if(_isedge(cnt, width)) {
			for(int i = 0; i < padding; i++) {
				index = i + (cnt + 1) * bpp + padding * (cnt / width);
				new[index] = paddata;
			}
		}
	}
	return NULL;
}

int _blackorwhite(int cornerIndex, int temp2, int temp1){
	int res = 0;
	if (cornerIndex * temp2 > temp1) {
		res = 255;
	}
	return res;
}

bool _isedge(int cnt, int w){
	bool tf = false;
	if ((cnt + 1) % w == 0) {
		tf = true;
	}
	return tf;
}

/*With help from Franklin Shiao*/
