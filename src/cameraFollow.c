#include "cameraFollow.h"
#include <math.h>
#include <stdio.h>

#define PI 3.14159265

void setTarget(struct Obj *obj) {
	camera.target = obj;
}

void setTargetAndParameters(struct Obj *obj, struct Vect3 distance, struct Vect3 angle, bool applyModelRotation) {
	setTarget(obj);
	camera.distance = distance;
	camera.angle = angle;
	camera.applyModelRotation = applyModelRotation;
}

void updateCamera() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	struct Vect3 position = {
		camera.distance.x,
		camera.distance.y,
		camera.distance.z,
	};

	float radians = PI / 180.0;
	float x = position.x;
	float y = position.y;
	float z = position.z;

	// Rotate around y Axis
	float angle = camera.angle.y * radians;
	if(camera.applyModelRotation)
		angle += camera.target->rotation.y * radians;

	position.z = z * cos(angle) - x * sin(angle);
	position.x = z * sin(angle) + x * cos(angle);

	// Rotate around x Axis
	x = position.x;
	y = position.y;
	z = position.z;
	angle = camera.angle.x * radians;
	if(camera.applyModelRotation)
		angle += camera.target->rotation.x * radians;

	position.y = y * cos(angle) - z * sin(angle);
	position.z = y * sin(angle) + z * cos(angle);

	// Rotate around z Axis
	x = position.x;
	y = position.y;
	z = position.z;
	angle = camera.angle.x * radians;
	if(camera.applyModelRotation)
		angle += camera.target->rotation.x * radians;

	position.x = x * cos(angle) - y * sin(angle);
	position.y = x * sin(angle) + y * cos(angle);

	position.x += camera.target->position.x;
	position.y += camera.target->position.y;
	position.z += camera.target->position.z;

	gluLookAt(position.x, 
			position.y,
			position.z,
			camera.target->position.x, 
			camera.target->position.y, 
			camera.target->position.z,
			0, 1, 0);
}