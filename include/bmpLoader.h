#ifndef BMPLOADER
#define BMPLOADER

#if defined(_WIN32) || defined(WIN32)
	#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>

/**
Loads a BMP image and loads it into openGL
@param imagepath path to the image
@return index to the loaded texture
**/
GLuint loadBMP(char* imagepath);

#endif