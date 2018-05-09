#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objLoader.h"


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/**
* Set the size for all the structures necessary to store all the geometry information
* @fp File pointer
* @mesh Pointer to the mesh begin readed / created
**/
void setMeshSizes(FILE *fp, struct Mesh *mesh) {
	char line[128];

	// Read all the lines of the file counting all the vertices, normals, texture coordinates and faces on the file
	while (fgets(line, sizeof line, fp) != NULL)
	{
		switch (line[0]) {
			case 'v':
				switch (line[1]) 	{
				case 'n':
					mesh->numberOfNormals += 1;
					break;
				case 't':
					mesh->numberOfTexCoords += 1;
					break;
				default:
					mesh->numberOfVertices += 1;
					break;
				}
				break;
			case 'f':
				mesh->numberOfFaces += 1;
				break;
		}
	}

	// Reserve space to hold the vertices, normals, texCoords and faces of the mesh
	mesh->vertices = malloc(sizeof(*mesh->vertices) * mesh->numberOfVertices);
	mesh->normals = malloc(sizeof(*mesh->normals) * mesh->numberOfNormals);
	mesh->texCoords = malloc(sizeof(*mesh->texCoords) * mesh->numberOfTexCoords);
	mesh->faces = malloc(sizeof(*mesh->faces) * mesh->numberOfFaces);

	// Go to the start of the file
	fseek(fp, 0, SEEK_SET);
}

/**
* Store all the geometry information in the corresponding structures
* @fp File pointer
* @mesh Mesh pointer to the new mesh created and that's being populated
**/
void readMeshInformation(FILE *fp, struct Mesh *mesh) {
	char line[128];

	unsigned int currentVertex = 0;
	unsigned int currentNormal = 0;
	unsigned int currentTexCoord = 0;
	unsigned int currentFace = 0;

	struct Vect3 min;
	struct Vect3 max;

	while (fgets(line, sizeof line, fp) != NULL) {
		int i;
		switch (line[0]) {
			case 'v':
				switch (line[1]) {
				case 'n':
					// Reads a vertex normal
					sscanf(line, "vn %f %f %f/n", &mesh->normals[currentNormal].x, &mesh->normals[currentNormal].y, &mesh->normals[currentNormal].z);
					currentNormal++;
					break;
				case 't':
					// Reads a coord texture
					sscanf(line, "vt %f %f/n", &mesh->texCoords[currentTexCoord].x, &mesh->texCoords[currentTexCoord].y);
					currentTexCoord++;
					break;
				default:
					// Reads a vertex
					sscanf(line, "v %f %f %f/n", &mesh->vertices[currentVertex].x, &mesh->vertices[currentVertex].y, &mesh->vertices[currentVertex].z);
					
					//Finds the bounding box of the mesh
					if (currentVertex == 0) {
						min.x = max.x = mesh->vertices[0].x;
						min.y = max.y = mesh->vertices[0].y;
						min.z = max.z = mesh->vertices[0].z;
					}

					min.x = MIN(min.x, mesh->vertices[currentVertex].x);
					min.y = MIN(min.y, mesh->vertices[currentVertex].y);
					min.z = MIN(min.z, mesh->vertices[currentVertex].z);

					max.x = MAX(max.x, mesh->vertices[currentVertex].x);
					max.y = MAX(max.y, mesh->vertices[currentVertex].y);
					max.z = MAX(max.z, mesh->vertices[currentVertex].z);

					currentVertex++;
					break;
				}
				break;
			case 'f':
				// Reads a face
				sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d/n",
					&mesh->faces[currentFace].vertexIndex[0], &mesh->faces[currentFace].texCoordsIndex[0], &mesh->faces[currentFace].normalIndex[0],
					&mesh->faces[currentFace].vertexIndex[1], &mesh->faces[currentFace].texCoordsIndex[1], &mesh->faces[currentFace].normalIndex[1],
					&mesh->faces[currentFace].vertexIndex[2], &mesh->faces[currentFace].texCoordsIndex[2], &mesh->faces[currentFace].normalIndex[2]);

				for (i = 0; i < 3; i++) {
					mesh->faces[currentFace].vertexIndex[i]--;
					mesh->faces[currentFace].texCoordsIndex[i]--;
					mesh->faces[currentFace].normalIndex[i]--;
				}
				currentFace++;
				break;
		}
	}

	// Calculates the center of the boundig box
	mesh->meshCenter.x = (min.x + max.x) / 2.0f;
	mesh->meshCenter.y = (min.y + max.y) / 2.0f;
	mesh->meshCenter.z = (min.z + max.z) / 2.0f;
}

/**
* Reads a mesh (Geometry) from a given file
* @filePath route to the file
* @return   Mesh*
**/
struct Mesh* loadMesh(char* filePath) {

	FILE *fp;
	fp = fopen(filePath, "r");

	if (fp == NULL) {
		perror("Error Loading a Mesh: ");
		return NULL;
	}

	struct Mesh* newMesh = malloc(sizeof(*newMesh));

	newMesh->numberOfVertices = 0;
	newMesh->numberOfNormals = 0;
	newMesh->numberOfTexCoords = 0;
	newMesh->numberOfFaces = 0;

	setMeshSizes(fp, newMesh);

	readMeshInformation(fp, newMesh);

	fclose(fp);

	printf("Model Loaded %s\n -Vetices Number: %d\n -Triangles Number: %d\n\n", filePath, newMesh->numberOfVertices, newMesh->numberOfFaces);

	return newMesh;
}