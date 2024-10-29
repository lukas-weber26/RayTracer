visualizer: main.c
	cc main.c -g -o test -lm -fopenmp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl -lGLEW 

fast: main.c
	cc main.c -Ofast -o test -lm -fopenmp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -lm -ldl -lGLEW 
