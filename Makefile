all:
	gcc -Iinclude/ src/bmpLoader.c src/cameraFollow.c src/main.c src/objLoader.c -lSOIL -lobj -lopengl32 -lglu32 -lglut32 -lm -o car.exe -Wall -Wextra -Wpedantic

linux:
	gcc -Iinclude/ src/bmpLoader.c src/cameraFollow.c src/main.c src/objLoader.c -lSOIL -lobj -lGL -lGLU -lglut -lm -o car -Wall -Wextra -Wpedantic
