#include <stdio.h>
#include <stdlib.h>

#include "bmpLoader.h"

/**
Loads a BMP image and loads it into openGL
@param imagepath path to the image
@return index to the loaded texture
**/
GLuint loadBMP(char* imagepath) {
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width;
	unsigned int height;
	unsigned int imageSize;
	unsigned char *data;

	FILE *file = fopen(imagepath, "rb");
	if (!file) {
		printf("Image file could not be opened\n (%s)", imagepath);
		return 0;
	}

	// Reads the header of the image
	if (fread(header, 1, 54, file) != 54) {
		printf("The image file %s is not a BMP file\n", imagepath);
		return 0;
	}

	// Check if the image is a BMP
	if (header[0] != 'B' || header[1] != 'M') {
		printf("The image file %s is not a BMP file\n", imagepath);
		return 0;
	}

	// Extracts the information from the header
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Recalculates the imageSize if it is incorrect in the file
	if (imageSize == 0)
		imageSize = width * height * 3;

	// Recalculates the pointer to the data if it is incorrect in the file
	if (dataPos == 0)
		dataPos = 54;

	// Allocates memory to store the image
	data = malloc(imageSize * sizeof(unsigned char));

	// Reads the image
	fread(data, 1, imageSize, file);

	fclose(file);

	glEnable(GL_TEXTURE_2D);
	GLuint textureID;
	// Creates a texture
	glGenTextures(1, &textureID);

	// Binds the texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// Set the parameters to texture coordinates
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Sets the parameters of magnification and minification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glDisable(GL_TEXTURE_2D);

	printf("Image Loaded %s\n", imagepath);

	return textureID;
}