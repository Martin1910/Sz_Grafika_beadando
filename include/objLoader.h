#ifndef OBJLOADER
#define OBJLOADER

#include <GL/glut.h>

// Defines a 2D vector
struct Vect2 {
	float x;
	float y;
};

// Defines a 3D vector
struct Vect3 {
	float x;
	float y;
	float z;
};

// Defines a Mesh Face
struct Face {
	unsigned int vertexIndex[3];
	unsigned int normalIndex[3];
	unsigned int texCoordsIndex[3];
};

// Defines a 3D object Mesh
struct Mesh {
	unsigned int numberOfVertices;
	unsigned int numberOfNormals;
	unsigned int numberOfTexCoords;
	unsigned int numberOfFaces;

	struct Vect3* vertices;
	struct Vect3* normals;
	struct Vect2* texCoords;
	struct Face* faces;
	struct Vect3 meshCenter;
};

// Defines a 3D object
struct Obj {
	struct Vect3 position;
	struct Vect3 rotation;
	struct Vect3 scale;
	struct Mesh **model;
	GLuint *textId;
};

/**
* Reads a mesh (Geometry) from a given file
* @filePath route to the file
* @return   Mesh*
**/
struct Mesh* loadMesh(char* filePath);

#endif