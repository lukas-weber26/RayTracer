visualizer: main.c
	cc main.c -g -o test -lm -fopenmp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl -lGLEW 

fast: main.c
	gcc main.c -Ofast -o test -lm -fopenmp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl -lGLEW 

op: main.c
	gcc main.c -O2 -o test -lm -fopenmp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl -lGLEW 

clang: main.c
	clang main.c -Ofast -o test -lm -fopenmp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl -lGLEW 
