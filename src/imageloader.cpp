#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "imageloader.h"

using namespace std;

Image::Image(char* ps, int w, int h, short b) : pixels(ps), width(w), height(h), bits(b) 
{
	cout<<"Constructor";		
}


Image::~Image() 
{
	delete[] pixels;
	cout<<"Destructor";
}


	//Converts a four-character array to an integer
	int toInt(const char* bytes) {
		return (int)(//((unsigned char)bytes[4] << 32)|
					 ((unsigned char)bytes[3] << 24) |
					 ((unsigned char)bytes[2] << 16) |
					 ((unsigned char)bytes[1] << 8) |
					 (unsigned char)bytes[0]);
	}

	//Converts a two-character array to a short
	short toShort(const char* bytes) 
	{
		return (short)(//((unsigned char)bytes[2] << 16)|
					   ((unsigned char)bytes[1] << 8) |
					   (unsigned char)bytes[0]);
	}

	//Reads the next four bytes as an integer
	int readInt(ifstream &input) 
	{
		char buffer[4];
		input.read(buffer,4);
		return toInt(buffer);
	}

	//Reads the next two bytes as a short
	short readShort(ifstream &input)
	{
		char buffer[2];
		input.read(buffer, 2);
		return toShort(buffer);
	}
	
	//auto_ptr, but for arrays
	template<class T>
	class auto_array
	{
		private:
			T* array;
			mutable bool isReleased;
		public:
			explicit auto_array(T* array_ = NULL) :
				array(array_), isReleased(false) {
			}
			
			auto_array(const auto_array<T> &aarray) {
				array = aarray.array;
				isReleased = aarray.isReleased;
				aarray.isReleased = true;
			}
			
			~auto_array() {
				if (!isReleased && array != NULL) {
					delete[] array;
				}
			}
			
			T* get() const {
				return array;
			}
			
			T &operator*() const 
			{
				return *array;
			}
			
			void operator=(const auto_array<T> &aarray) 
			{
				if (!isReleased && array != NULL) 
				{
					delete[] array;
				}
				array = aarray.array;
				isReleased = aarray.isReleased;
				aarray.isReleased = true;
			}
			
			T* operator->() const 
			{
				return array;
			}
			
			T* release() {
				isReleased = true;
				return array;
			}
			
			void reset(T* array_ = NULL) {
				if (!isReleased && array != NULL) {
					delete[] array;
				}
				array = array_;
			}
			
			T* operator+(int i) {
				return array + i;
			}
			
			T &operator[](int i) {
				return array[i];
			}
	};




	Image* read1BitImage(short bits, ifstream &input, int width, int height, int dataOffset) {
		//Read the data
		int bytesPerRow = (width/8 + (width/8) % 4);
		int size = bytesPerRow * height;
		auto_array<char> pixels(new char[size]);
		input.seekg(dataOffset, ios_base::beg);
		input.read(pixels.get(), size);

		//Get the data into the right format
		auto_array<char> pixels2(new char[width * height]);
		for(int y = 0,rev_y=height-1; y < height; y++,rev_y--) {
			for(int x = 0; x < width/8; x++) {
				int pos = (rev_y * width) + x * 8;
				for(int c = 0; c < 8; c++) {
					pixels2[pos + (7 - c)] = (pixels[bytesPerRow * y + x] >> c);
				}
			}
		}
		return new Image(pixels2.release(), width, height, bits);
	}

	Image* read4BitsImage(short bits, ifstream &input, int width, int height, int dataOffset) {
		//Read the data
		int bytesPerRow = (width/2 + (width/2)%4);
		int size = bytesPerRow * height;
		auto_array<char> pixels(new char[size]);
		input.seekg(dataOffset, ios_base::beg);
		input.read(pixels.get(), size);

		int bChar=0;
		//Get the data into the right format
		auto_array<char> pixels2(new char[ (width) * height]);
		for(int y = 0,rev_y=height-1; y < height; y++,rev_y--) {
			for(int x = 0; x < width/2; x++) 
			{
				int pos = (rev_y * width) + x * 2;

				for(int c = 0; c < 2; c++) 
				{
					pixels2[pos + (1 - c)] = (pixels[bytesPerRow * y + x] >> 4);
				}
			}
		}
		return new Image(pixels2.release(), width, height, bits);
	}

	Image* read8BitsImage(short bits, ifstream &input, int width, int height, int dataOffset)
	 {
		//Read the data
		int bytesPerRow = width;
		int size = bytesPerRow * height;
		auto_array<char> pixels(new char[size]);
		input.seekg(dataOffset, ios_base::beg);
		input.read(pixels.get(), size);

		//Get the data into the right format
		auto_array<char> pixels2(new char[width * height]);
		for(int y = 0; y < height; y++) 
		{
			for(int x = 0; x < width; x++)
			{
				pixels2[width * y + x] = pixels[bytesPerRow * y + x];
			}
		}
		return new Image(pixels2.release(), width, height, bits);
	}


	Image* read24BitsImage(short bits, ifstream &input, int width, int height, int dataOffset) 
	{
		//Read the data
		int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
		int size = bytesPerRow * height;
		auto_array<char> pixels(new char[size]);
		input.seekg(dataOffset, ios_base::beg);
		input.read(pixels.get(), size);

		//Get the data into the right format
		auto_array<char> pixels2(new char[width * height * 3]);
		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				for(int c = 0; c < 3; c++) {
					pixels2[3 * (width * y + x) + c] =
						pixels[bytesPerRow * y + 3 * x + (2 - c)];
				}
			}
		}
		return new Image(pixels2.release(), width, height, bits);
	}

	Image* read32BitsImage(short bits, ifstream &input, int width, int height, int dataOffset) 
	{
		//Read the data
		int bytesPerRow = ((width * 4 + 3) / 4) * 4 - (width * 4 % 4);
		int size = bytesPerRow * height;
		auto_array<char> pixels(new char[size]);
		input.seekg(dataOffset, ios_base::beg);
		input.read(pixels.get(), size);

		//Get the data into the right format
		auto_array<char> pixels2(new char[width * height * 4]);
		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				for(int c = 0; c < 4; c++) {
					pixels2[4 * (width * y + x) + c] =
						pixels[bytesPerRow * y + 4 * x + (2 - c)];
				}
			}
		}
		return new Image(pixels2.release(), width, height, bits);
	}


	Image* readBitMapPixels(short bits, ifstream &input, int width, int height, int dataOffset) 
	{
		Image* image;
		switch(bits){
		case 1:
			image = read1BitImage(bits, input, width, height, dataOffset);
			break;

		case 4:
			image = read4BitsImage(bits, input, width, height, dataOffset);
			break;

		case 8:
			image = read8BitsImage(bits, input, width, height, dataOffset);
			break;

		case 16:
		case 24:
			image = read24BitsImage(bits, input, width, height, dataOffset);
			break;
		case 32:
			image = read32BitsImage(bits, input, width, height, dataOffset);
			break;
		default:
			assert(!"Unknown bitmap format image ");
		}

		return image;
	}

Image* loadBMP(const char* filename) 
{
	ifstream input;
	input.open(filename, ifstream::binary);
	assert(!input.fail() || !"Could not find file");
	char buffer[2];
	input.read(buffer,2);
	assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	input.ignore(8);
	int dataOffset = readInt(input);
	
	//Read the header
	int headerSize = readInt(input);
	int width;
	int height;
	Image* image;

	switch(headerSize) 
	{
		case 108:
		case 40: // read Windows BMP image
			width = readInt(input);
			height = readInt(input);
			input.ignore(2);
			image = readBitMapPixels(readShort(input), input, width, height, dataOffset);
//			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
//			assert(readShort(input) == 0 || !"Image is compressed");
			break;

		case 12: // read OS/2 1.x BMP Image
			width = readShort(input);
			height = readShort(input);
			input.ignore(2);
			image = readBitMapPixels(readShort(input), input, width, height, dataOffset);
//			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			break;

		case 64:
			//OS/2 V2
			assert(!"Can't load OS/2 V2 bitmaps");
			break;

//		case 108:
//			//Windows V4
//			assert(!"Can't load Windows V4 bitmaps");
//			break;

		case 124:
			//Windows V5
			assert(!"Can't load Windows V5 bitmaps");
			break;

		default:
			assert(!"Unknown bitmap format");
	}
	
//	//Read the data
//	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
//	int size = bytesPerRow * height;
//	auto_array<char> pixels(new char[size]);
//	input.seekg(dataOffset, ios_base::beg);
//	input.read(pixels.get(), size);
//
//	//Get the data into the right format
//	auto_array<char> pixels2(new char[width * height * 3]);
//	for(int y = 0; y < height; y++) {
//		for(int x = 0; x < width; x++) {
//			for(int c = 0; c < 3; c++) {
//				pixels2[3 * (width * y + x) + c] =
//					pixels[bytesPerRow * y + 3 * x + (2 - c)];
//			}
//		}
//	}
//
//	input.close();
//	return new Image(pixels2.release(), width, height);
	return (image);
}

