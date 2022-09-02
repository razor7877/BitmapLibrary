#ifndef BITMAPLIBRARY_HPP
#define BITMAPLIBRARY_HPP

unsigned char* loadBMP(const char* imagePath);
void writeBMP(const int width, const int height, const unsigned char* data, const char* imagePath);
void writeToCharPtr(unsigned char* arr, const char* value, const int size);
void writeToCharPtr(unsigned char* arr, const int value, const int size);

#endif