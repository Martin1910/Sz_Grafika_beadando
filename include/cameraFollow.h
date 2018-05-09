#ifndef CAMERA_FOLLOW
#define CAMERA_FOLLOW

#include "objLoader.h"

// Defines a boolean
typedef enum {
	false = 0,
	true = 1
} bool;

struct {
	struct Vect3 distance;
	struct Vect3 angle;
	bool applyModelRotation;
	struct Obj *target;
} camera;

void setTarget(struct Obj *obj);
void setTargetAndParameters(struct Obj *obj, struct Vect3 distance, struct Vect3 angle, bool applyModelRotation);
void updateCamera();

#endif