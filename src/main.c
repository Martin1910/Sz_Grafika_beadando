#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include "objLoader.h"
#include "bmpLoader.h"
#include "cameraFollow.h"

#define PI 3.14159265

#define WINDOWS_WIDTH 1024
#define WINDOWS_HEIGHT 720

#define TIRES_TURNING_ANGLE 15
#define TIRES_ROTATION_SPEED (float)10.0 / (float)30.0
#define CAR_TURNING_ANGLE (float)3.0 / (float)30.0
#define CAR_SPEED (float)0.5 / (float)30.0

int lastTwoKeys [] = {-1, -1}; // Last Two keys Currently Pressed
int nextKeyIndex = 0; // Next key index to be stored

struct Vect3 cameraDistanceFromModel = {0, 7, 30};
struct Vect3 cameraAngle = {0, 180, 0};

const float radians = PI / 180.0;

int lastFrame; //TimeStamp to the last frame

//Lights definition
GLfloat light1_position[] = { 1, 1, 0, 0.0 }; //Directional Light (SUN)
GLfloat light1_ambient[] = { 0.5, 0.5, 0.5, 1 };
GLfloat light1_diffuse[] = { 1, 1, .8, 1.0 };

GLfloat light2_position[] = { 0, 2, -5.4, 1.0 }; //Front of the car
GLfloat light2_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light3_position[] = { 0, 2, 6, 1.0 }; //Back of the car
GLfloat light3_diffuse[] = { 1.0, .0, .0, 1.0 };

struct Mesh *carMesh, *tiresMesh, *roadMesh;
GLuint carTexture, tiresTexture, roadTexture;

struct Obj car = { { -6.188383	, 1.08769, -66.752327 }, { 0, 291.7, 0 }, { 1, 1, 1 }, &carMesh, &carTexture};
struct Obj tires[] = {
	{ { 2.28353, 0.02604, 3.0988 }, { 0, 0, 0 }, { 1, 1, 1 }, &tiresMesh, &tiresTexture}, // Left Front Tire
	{ { -2.28353, 0.02604, 3.0988 }, { 0, 180, 0 }, { 1, 1, 1 }, &tiresMesh, &tiresTexture},  // Right Front Tire
	{ { 2.28353, 0.02604, -3.8404 }, { 0, 0, 0 }, { 1, 1, 1 }, &tiresMesh, &tiresTexture}, // Left Back Tire
	{ { -2.28353, 0.02604, -3.8404 }, { 0, 180, 0 }, { 1, 1, 1 }, &tiresMesh, &tiresTexture}, // Right Back Tire
};

struct Obj scene[] = {
	{ { 0, 0.498643, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, &roadMesh, &roadTexture }, // Road
};

const unsigned int sceneObjectsCount = 1;

void applyTransformations(struct Obj *obj) {
	// Apply the object tranformations
	glTranslatef(obj->position.x, obj->position.y, obj->position.z);
	glScalef(obj->scale.x, obj->scale.y, obj->scale.z);
	glRotatef(obj->rotation.z, 0, 0, 1);
	glRotatef(obj->rotation.y, 0, 1, 0);
	glRotatef(obj->rotation.x, 1, 0, 0);
}

void drawObj(struct Obj *obj) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *(obj->textId));
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLES);
	// Iterates over each face of the object
	
	int i, j;
	
	for (i = 0; i < (*(obj->model))->numberOfFaces; i++) {
		// Draw the face
		for (j = 0; j < 3; j++) {
			unsigned int texCoord = (*(obj->model))->faces[i].texCoordsIndex[j];
			unsigned int normal = (*(obj->model))->faces[i].normalIndex[j];
			unsigned int vertex = (*(obj->model))->faces[i].vertexIndex[j];

			glTexCoord2f((*(obj->model))->texCoords[texCoord].x, (*(obj->model))->texCoords[texCoord].y);
			glNormal3f((*(obj->model))->normals[normal].x, (*(obj->model))->normals[normal].y, (*(obj->model))->normals[normal].z);
			glVertex3f((*(obj->model))->vertices[vertex].x, (*(obj->model))->vertices[vertex].y, (*(obj->model))->vertices[vertex].z);
		}
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void drawScene() {
	int i;
	for(i = 0; i < sceneObjectsCount; i++) {
		glPushMatrix();
			applyTransformations(&scene[i]);
			drawObj(&scene[i]);
		glPopMatrix();
	}
}

void drawTires() {
	int i;
	for(i = 0; i < 4; i++) {
		glPushMatrix();
			applyTransformations(&tires[i]);
			drawObj(&tires[i]);
		glPopMatrix();
	}
}

void drawCar() {
	//printf("Car Position: %f %f %f Car Rotation: %f %f %f\n", car.position.x, car.position.y, car.position.z, car.rotation.x, car.rotation.y, car.rotation.z);
	glPushMatrix();
		applyTransformations(&car);
		// glLightfv(GL_LIGHT1, GL_POSITION, light3_position);
		// glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
		drawObj(&car);
		drawTires();
	glPopMatrix();
}

void reshape( int w, int h ) {
    // Change the viewport
    glViewport( 0, 0, w, h );
    // Change the proyection matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    // Applies the perspective proyection
    gluPerspective( 48.5, (GLfloat) w/ (GLfloat) h, 0.1, 1000 );
    // Change the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}

void display() {
    // Clear the buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	updateCamera();

	//glLightfv(GL_LIGHT0, GL_POSITION, light1_position);

	drawCar();
	drawScene();

    // Swap the buffers
    glutSwapBuffers( );
}

void update() {

	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int elapse = currTime - lastFrame;
	lastFrame = currTime;
	float angle = 0;
	int i;
	for(i = 0; i < 2; i++)
	{
		if(lastTwoKeys[i] != -1) {
			switch(lastTwoKeys[i]){
				case GLUT_KEY_RIGHT:
					car.rotation.y -= CAR_TURNING_ANGLE * elapse;
					break;
				case GLUT_KEY_LEFT:
					car.rotation.y += CAR_TURNING_ANGLE * elapse;
					break;
				case GLUT_KEY_UP:
					angle = car.rotation.y * radians;
	    			car.position.x -= -sin(angle) * CAR_SPEED * elapse;
	    			car.position.z -= -cos(angle) * CAR_SPEED * elapse;
					for(i = 0; i < 4; i++) {
						if(i % 2 == 0)
							tires[i].rotation.x += TIRES_ROTATION_SPEED * elapse;
						else
							tires[i].rotation.x -= TIRES_ROTATION_SPEED * elapse;
					}
					break;
				case GLUT_KEY_DOWN:
					angle = car.rotation.y * radians;
	    			car.position.x += -sin(angle) * CAR_SPEED * elapse;
	    			car.position.z += -cos(angle) * CAR_SPEED * elapse;
					for(i = 0; i < 4; i++) {
						if(i % 2 == 0)
							tires[i].rotation.x -= TIRES_ROTATION_SPEED * elapse;
						else
							tires[i].rotation.x += TIRES_ROTATION_SPEED * elapse;
					}
					break;
			}
		}
	}

    // Indicates that you can redisplay again everything
    glutPostRedisplay();
}

void keyDown(int key, int x, int y) {
    nextKeyIndex = (nextKeyIndex + 1) % 2;
	int i;
    for(i = 0; i < 2; i++)
    	if(lastTwoKeys[i] == -1) {
    		nextKeyIndex = i;
    		break;
    	}

    lastTwoKeys[nextKeyIndex] = key;

    switch(key) {
    	case GLUT_KEY_RIGHT:
			tires[0].rotation.y -= TIRES_TURNING_ANGLE;
			tires[1].rotation.y -= TIRES_TURNING_ANGLE;
			break;
		case GLUT_KEY_LEFT:
			tires[0].rotation.y += TIRES_TURNING_ANGLE;
			tires[1].rotation.y += TIRES_TURNING_ANGLE;
			break;
    }
}

void keyUp(int key, int x, int y) {
	int i;
    for(i = 0; i < 2; i++) 
    	if(lastTwoKeys[i] == key){ 
    		lastTwoKeys[i] = -1;
    		break;
    	}

    switch(key) {
    	case GLUT_KEY_RIGHT:
			tires[0].rotation.y += TIRES_TURNING_ANGLE;
			tires[1].rotation.y += TIRES_TURNING_ANGLE;
			break;
		case GLUT_KEY_LEFT:
			tires[0].rotation.y -= TIRES_TURNING_ANGLE;
			tires[1].rotation.y -= TIRES_TURNING_ANGLE;
			break;
    }
}

void keyboardFunc(char key, int x, int y){

	if (key == 'q' || key == 'Q') {
		exit(0);
	}

}

void initGlutCallBacks() {
	// Handles the windows resize
	glutReshapeFunc(reshape);
    // Drawing function
    glutDisplayFunc(display);
	// Idle function
    glutIdleFunc(update);
    // Keyboard handler for keys pushed
    glutSpecialFunc(keyDown);
    // Keyboard handler for keys realeased
    glutSpecialUpFunc(keyUp);
    // Keyboard handler for key pressed
    glutKeyboardFunc(keyboardFunc); 
    // Allow keeping a key pressed without generating new events
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
}

void loadTextures() {
	roadTexture = loadBMP("assets/textures/Road.bmp");
	carTexture = loadBMP("assets/textures/Car.bmp");
	tiresTexture = loadBMP("assets/textures/Tire.bmp");
}

void loadMeshes() {
	roadMesh = loadMesh("assets/models/Road.obj");
	carMesh = loadMesh("assets/models/Car.obj");
	tiresMesh = loadMesh("assets/models/Tire.obj");
}

void loadAssets(){
	loadTextures();
	loadMeshes();
}

void init() {

	//Init window and display mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
    glutCreateWindow ("game");

    // Enable the depth test
    glEnable(GL_DEPTH_TEST);

    // Set clear color
    glClearColor(.5294, .8078, .9215, 0);

    // Set the shading Mode
    glShadeModel(GL_SMOOTH);
	
	//Set Lights
    glLightfv(GL_LIGHT0, GL_AMBIENT, light1_ambient);

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
   
	loadAssets();

	setTargetAndParameters(&car, cameraDistanceFromModel, cameraAngle, true);

	initGlutCallBacks();
}

int main(int argc, char const *argv[]) {
	// Initialize glut
	glutInit(&argc, (char**)argv);
	// Initialize the rest of the structures
	init();
	// Calls the main game Loop
	glutMainLoop();
	return 0;
}