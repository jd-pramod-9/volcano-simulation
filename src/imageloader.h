#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

//#ifndef IMAGE_LOADER_H_INCLUDED
//#define IMAGE_LOADER_H_INCLUDED

//Represents an image
class Image {
	public:
		Image(char* ps, int w, int h, short bits);
		~Image();
		
		/* An array of the form (R1, G1, B1, R2, G2, B2, ...)   (Color components range from 0 to 255.)*/
		char* pixels;
		int width;
		int height;
		short bits;
};

//Reads a bitmap image from file.
Image* loadBMP(const char* filename);
//Image* readBitMapPixels(short bits, ifstream &input, int width, int height, int dataOffset);
//#endif
