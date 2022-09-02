// Disable fopen() unsafe warning
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "bitmaplibrary.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>

// Load a .bmp file from a given path, then returns a pointer pointing to the data
// Data is returned with RGB colors (swap done from BGR)
unsigned char* loadBMP(const char* imagePath)
{
	// Data read from the header of the BMP file
	// BMP files begin with a 54 bytes header (14 bytes file header & 40 bytes info header)
	unsigned char header[54];
	unsigned int dataPos{}; // Position in the file where the image data begins
	unsigned int width{}, height{};
	unsigned int imageSize{}; // = width * height * 3
	unsigned int bitsPerPixel{}; // 24 bits or 32 bits with alpha channel
	unsigned char* data; // Actual RGB data

	FILE* file = fopen(imagePath, "rb");
	if (file == NULL)
	{
		printf("File not found\n");
		return 0;
	}

	// Read the first 54 header bytes
	if (fread(&header, 1, 54, file) != 54)
	{
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Checks if .bmp header is correct
	if (header[0] != 'B' || header[1] != 'M')
	{
		printf("Missing BM header\n");
		return 0;
	}

	// First, get pointer of header array at offset 0x0A (10),
	// then cast to int ptr to extract the 4 first bytes and dereference the address
	dataPos = *(int*)&(header[0x0A]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	imageSize = *(int*)&(header[0x22]);

	if (imageSize == 0) { imageSize = width * height * 3; }
	if (dataPos == 0) { dataPos = 54; }

	// Initialize data with known image size
	data = new unsigned char[imageSize]; // TODO: REMOVE PTR FROM MEMORY BEFORE RETURN
	// Read the corresponding image data
	fread(data, 1, imageSize, file);
	fclose(file);

	unsigned char tmp;

	for (int i = 0; i < imageSize; i += 3)
	{
		// Swapping bytes to have RGB color order instead of default BGR used in .bmp files
		tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

	return data;
}

void writeBMP(const int width, const int height, unsigned char* data, const char* imagePath)
{
	unsigned int bitsPerPixel = 24;
	unsigned int imageSize = width * height * 3;
	unsigned char header[54]; // To store header data
	unsigned char* content = new unsigned char[imageSize]; // To store pixel data

	// File header
	writeToCharPtr(&header[0x0], "BM", 2); // First 2 bytes of .bmp file are BM
	writeToCharPtr(&header[0x2], imageSize + 54, 4); // File size (header size + image size)
	writeToCharPtr(&header[0x6], 0, 2); // Reserved
	writeToCharPtr(&header[0x8], 0, 2); // Reserved
	writeToCharPtr(&header[0xA], 54, 4); // Data offset position

	// Info header
	writeToCharPtr(&header[0xE], 40, 4); // Header size
	writeToCharPtr(&header[0x12], width, 4); // Image width
	writeToCharPtr(&header[0x16], height, 4); // Image height
	writeToCharPtr(&header[0x1A], 1, 2); // Planes
	writeToCharPtr(&header[0x1C], 24, 2); // Bits per pixel
	writeToCharPtr(&header[0x1E], 0, 4); // Compression method
	writeToCharPtr(&header[0x22], imageSize, 4); // Image size
	writeToCharPtr(&header[0x26], 0, 4); // Print resolution horizontal
	writeToCharPtr(&header[0x2A], 0, 4); // Print resolution vertical
	writeToCharPtr(&header[0x2E], 0, 4); // Number of colors in palette
	writeToCharPtr(&header[0x32], 0, 4); // Important colors

	// Get content bytes from data pointer
	for (int i = 0; i < imageSize; i++)
	{
		content[i] = data[i];
	}

	// Create file, write to it, then close it
	std::ofstream stream(imagePath, std::ofstream::out);
	stream.write(reinterpret_cast<char*>(header), 54);
	stream.write(reinterpret_cast<char*>(data), imageSize);
	stream.close();

	delete[] content;
}

// Writes content of a char array (string) to a given char pointer
void writeToCharPtr(unsigned char* arr, const char* value, const int size)
{
	for (int i = 0; i < size; i++)
	{
		// *(arr + i) == arr[i]
		// *(value + i) == value[i]
		*(arr + i) = *(value + i);
	}
}

// Writes content of an integer to a given char pointer
void writeToCharPtr(unsigned char* arr, const int value, const int size)
{
	for (int i = 0; i < size; i++)
	{
		arr[i] = (unsigned char)(value >> (8 * i));
	}
}

int main(int argc, char* argv[])
{
	unsigned char* d = loadBMP("C:/Users/razor7877/Desktop/OpenGL/image.bmp");
	writeBMP(512, 576, d, "C:/Users/razor7877/Desktop/copy.bmp");

	return 0;
}