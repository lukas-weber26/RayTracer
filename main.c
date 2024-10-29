#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define sphere_count 3
#define light_count 2

void window_size_callback(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
}

void handle_close(GLFWwindow * window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE); //changed 1 to gl_true
	}
}

GLFWwindow * opengl_init(unsigned int * VAO, unsigned int * program, unsigned int * texture) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(1920, 1080, "Canvas", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_size_callback);
	glfwPollEvents();

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0,0, 1920, 1080);
	
	float verts [] = {
		-1.0, -1.0,0,0,
		1.0,-1.0,1.0,0,
		-1.0,1.0,0,1.0,
		1.0,-1.0,1.0,0,
		1.0,1.0,1.0,1.0,
		-1.0,1.0,0,1.0
	};

	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	int success;
	const char * vertex_shader_source = "#version 330 core\n"
	"layout (location = 0) in vec2 aPos;\n"
	"layout (location = 1) in vec2 aTexCoord;\n"
	"out vec2 TexCoord;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
	"TexCoord = aTexCoord;"
	"}\0";

	const char * fragment_shader_source = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D ourTexture;\n"
	"void main()\n"
	"{\n"
	"FragColor = texture(ourTexture,TexCoord);\n"
	"}\0";

	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	assert(success);

	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	assert(success);

	*program = glCreateProgram();
	glAttachShader(*program, vertex_shader);
	glAttachShader(*program, fragment_shader);
	glLinkProgram(*program);
	glGetProgramiv(*program, GL_LINK_STATUS, &success);
	assert(success);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glUseProgram(*program);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void * )0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void * )(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//texture stuff
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return window;
}

typedef struct rgb_color {
	unsigned char  r; 
	unsigned char  g; 
	unsigned char  b; 
} rgb_color;

typedef struct canvas_coord {
	int x;	
	int y;	
} canvas_coord ; 

typedef struct screen_coord{
	int x;	
	int y;	
} screen_coord; 

typedef struct viewport_coord{
	double x;	
	double y;	
	double z;	
} viewport_coord; 

typedef struct sphere {
	viewport_coord center;	
	double rad;	
	rgb_color color;
	int specular;
} sphere; 

typedef enum {POINT, DIRECTIONAL} light_type;

typedef struct light {
	double x; 
	double y; 
	double z; 
	double intensity; 
	light_type type;
} light;

light light_create(double x, double y, double z, light_type type, double intensity) {
	light new_light	= {x,y,z,intensity,type};
	return new_light;
} 

viewport_coord viewport_coord_normalize(viewport_coord a) {
	double magnitude = sqrt(a.x*a.x + a.y*a.y+a.z*a.z);
	viewport_coord resulting_coord = {a.x/magnitude, a.y/magnitude, a.z/magnitude};
	return resulting_coord;
}

viewport_coord viewport_coord_subtract(viewport_coord a, viewport_coord b) {
	viewport_coord resulting_coord = {a.x-b.x, a.y-b.y, a.z - b.z};
	return resulting_coord;
}

viewport_coord viewport_coord_add(viewport_coord a, viewport_coord b) {
	viewport_coord resulting_coord = {a.x+b.x, a.y+b.y, a.z + b.z};
	return resulting_coord;
}

viewport_coord viewport_coord_scale(viewport_coord a, double b) {
	viewport_coord resulting_coord = {a.x*b, a.y*b, a.z*b};
	return resulting_coord;
}

double viewport_coord_dot(viewport_coord a, viewport_coord b) {
	double result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}

viewport_coord viewport_coord_get(double x, double y, double z) {
	viewport_coord new = {x,y,z};
	return new;
}

sphere create_sphere(double x, double y, double z, double rad, unsigned char r, unsigned char g, unsigned char b, int specular) {
	viewport_coord sphere_coord = {x,y,z};
	rgb_color sphere_color = {r,g,b};
	sphere final_sphere = {sphere_coord, rad, sphere_color, specular};
	return final_sphere;
}

viewport_coord create_viewport_coord(double x, double y, double z){
	viewport_coord position = {x,y,z};
	return position;
}

void print_canvas_coord (canvas_coord coord, int print_newline) {
	printf("x,y:%d,%d", coord.x, coord.y);
	if (print_newline) {
		printf("\n");
	}
}

void print_screen_coord (screen_coord coord, int print_newline) {
	printf("x,y:%d,%d", coord.x, coord.y);
	if (print_newline) {
		printf("\n");
	}
}

void print_viewport_coord (viewport_coord coord, int print_newline) {
	printf("x,y,z:%f,%f,%f", coord.x, coord.y, coord.z);
	if (print_newline) {
		printf("\n");
	}
}

typedef struct scene_information {
	int screen_width; 
	int screen_height; 
	int canvas_width; 
	int canvas_height; 
	double viewport_width; 
	double viewprot_height; 
	double min_t;
	double max_t;
	viewport_coord camera_position;
	sphere spheres[sphere_count];
	int n_spheres;
	rgb_color background_color;		
	int n_lights;
	light lights[light_count];		
	double diffuse; 
} scene_information;

rgb_color get_color(unsigned char r, unsigned char g, unsigned char b) {
	rgb_color color = {r,g,b};
	return color;
}

screen_coord canvas_to_screen(canvas_coord canvas, scene_information scene) {
	assert(scene.canvas_width == scene.screen_width);
	assert(scene.canvas_height == scene.screen_height);
	screen_coord screen = {canvas.x+scene.screen_width/2, canvas.y+scene.screen_height/2}; 
	return screen;
} 

viewport_coord canvas_to_viewport(canvas_coord canvas, scene_information scene) {
	viewport_coord viewport = {((double) canvas.x)*(scene.viewport_width/((double)scene.canvas_width)), ((double) canvas.y)*(scene.viewprot_height/((double)scene.canvas_height)), 1.0};
	return viewport;
} 

viewport_coord get_viewport_coord(double x, double y, double z) {
	viewport_coord viewport = {x,y,z};
	return viewport;
} 


void draw_canvas_on_screen(unsigned char * screen, canvas_coord canvas, rgb_color color, scene_information scene) {
	screen_coord sc = canvas_to_screen(canvas, scene);

	screen[(sc.x+(scene.screen_width*sc.y))*3] = color.r;
	screen[(sc.x+(scene.screen_width*sc.y))*3 + 1] = color.g;
	screen[(sc.y+(scene.screen_width*sc.y))*3 + 2] = color.b;
}

#define inf 1000000.0

viewport_coord intersect_ray_sphere(viewport_coord origin, viewport_coord direction, sphere sphere) {
	double r = sphere.rad;
	viewport_coord CO = viewport_coord_subtract(origin, sphere.center);

	double a = viewport_coord_dot(direction, direction);
	double b = 2*viewport_coord_dot(CO, direction);
	double c = viewport_coord_dot(CO, CO) - r*r;
	
	double discriminant = b*b - 4*a*c;
	if (discriminant < 0) {
		return get_viewport_coord(inf*10 ,inf*10, 0);
	}

	double t1 = (-b + sqrt(discriminant))/(2*a);
	double t2 = (-b - sqrt(discriminant))/(2*a);
	
	return get_viewport_coord(t1,t2, 0);

}

rgb_color scale_color (rgb_color input_color, double scale) {
	rgb_color output_color;
	output_color.r = (unsigned char) (((double) input_color.r) * scale);
	output_color.g = (unsigned char) (((double) input_color.g) * scale);
	output_color.b = (unsigned char) (((double) input_color.b) * scale);
	return output_color;
}

viewport_coord light_to_viewport_coord (light input_light) {
	viewport_coord output_coord = {input_light.x, input_light.y, input_light.z};
	return output_coord;
}

double viewport_coord_length (viewport_coord a) {
	return sqrt(a.x*a.x + a.y*a.y+a.z*a.z);
}

//viewport_coord viewport_position
void calculate_closest_intersection(scene_information scene, viewport_coord origin, viewport_coord direction, double t_min, double t_max,double * closest_t, int * closest_sphere) {
	for(int i = 0; i<scene.n_spheres; i++) {
		viewport_coord t1t2 = intersect_ray_sphere(origin, direction, scene.spheres[i]);
		double t1 = t1t2.x;
		double t2 = t1t2.y;
		if (t1 < t_max && t_min < t1 && t1 < *closest_t) {
			*closest_t = t1;
			*closest_sphere = i;
		}	
		if (t2 < t_max && t_min < t2 && t2 < *closest_t) {
			*closest_t = t2;
			*closest_sphere = i;
		}	
	}
}

rgb_color compute_color(scene_information scene, viewport_coord viewport_position, int closest_sphere, double closest_t) {
		rgb_color return_color = scene.spheres[closest_sphere].color;

		double light_intensity = scene.diffuse;
		viewport_coord light_vector; 
		viewport_coord point =  viewport_coord_add(viewport_coord_scale(viewport_coord_subtract(viewport_position,scene.camera_position),closest_t),scene.camera_position);
		viewport_coord normal = viewport_coord_normalize(viewport_coord_subtract(point, scene.spheres[closest_sphere].center));

		for (int i = 0; i < scene.n_lights; i ++) {
			if (scene.lights[i].type == POINT) {
				light_vector = viewport_coord_subtract(light_to_viewport_coord(scene.lights[i]), point);
			} else if (scene.lights[i].type == DIRECTIONAL) {
				light_vector = viewport_coord_get(scene.lights[i].x, scene.lights[i].y,scene.lights[i].z);
			} else {
				printf("Bad light.");
				exit(0);
			} 
			
			//shadow check here
			int shadow_sphere = -1;
			double shadow_t = scene.max_t;	
			calculate_closest_intersection(scene, point, light_vector, 0.001, scene.max_t, &shadow_t, &shadow_sphere);	
			if (shadow_sphere != -1) {
				continue;
			}

			//diffuse
			double n_dot_l = viewport_coord_dot(normal, light_vector);
			if (n_dot_l > 0) {
				light_intensity += scene.lights[i].intensity * n_dot_l / (viewport_coord_length(normal) * viewport_coord_length(light_vector));
			}

			//specular
			viewport_coord R = viewport_coord_subtract(viewport_coord_scale(normal, 2 * viewport_coord_dot(normal, light_vector)), light_vector);	
			viewport_coord V = viewport_coord_subtract(scene.camera_position, point); //this subract is a bit quiestionable, may be able to just scale by -1
			double r_dot_v = viewport_coord_dot(R,V); 			
			if (r_dot_v > 0) {
				light_intensity += scene.lights[i].intensity * pow(r_dot_v/(viewport_coord_length(R) * viewport_coord_length(V)), scene.spheres[closest_sphere].specular);
			}	
	
		}

		if (light_intensity > 1.0) {
			light_intensity = 1.0;
		}

		return scale_color(return_color, light_intensity);
}

rgb_color raytrace(scene_information scene, viewport_coord viewport_position) {
	double closest_t = inf;	
	int closest_sphere = -1;

	calculate_closest_intersection(scene, scene.camera_position, viewport_coord_subtract(viewport_position, scene.camera_position), scene.min_t, scene.max_t, &closest_t, &closest_sphere);
	
	if (closest_sphere == -1) {
		return scene.background_color;		
	} else {
		return compute_color(scene, viewport_position, closest_sphere, closest_t);
	}
}

void handle_canvas_point(unsigned char * screen, scene_information scene, canvas_coord canvas_position) {
	viewport_coord coord = canvas_to_viewport(canvas_position, scene);
	rgb_color color = raytrace(scene, coord);
	draw_canvas_on_screen(screen, canvas_position, color, scene);
}

void draw_on_canvas(unsigned char * screen, scene_information scene) {
	#pragma omp parallel for 
	for (int x = -scene.canvas_width/2; x < scene.canvas_width/2; x ++) {
		for (int y = -scene.canvas_height/2; y < scene.canvas_height/2; y ++) {
		
			canvas_coord coord = {x,y}; 
			handle_canvas_point(screen, scene, coord);

		}
	}
}

int main() {	

	scene_information scene;
	scene.screen_width = 1920; 
	scene.screen_height = 1080; 
	scene.canvas_width = 1920; 
	scene.canvas_height = 1080; 
	scene.viewport_width = 1.0; 
	scene.viewprot_height = 0.6; 
	scene.camera_position = create_viewport_coord(0.0,0.0,0.0);
	scene.max_t = inf;
	scene.min_t = 1.0;
	scene.n_lights = light_count;
	scene.lights[0] = light_create(0.0, 1.0, 1.0, POINT,0.4);
	scene.lights[1] = light_create(0.0, -0.3, 1.0, DIRECTIONAL ,0.4);
	scene.n_spheres = sphere_count;
	scene.spheres[0] = create_sphere(0.0, -0.1, 3.0, 0.3, 120, 200, 0, 1000);
	scene.spheres[1] = create_sphere(-0.3, 0.0, 1.2, 0.2, 230, 40, 40, 500);
	scene.spheres[2] = create_sphere(0.4, 0.1, 2.0, 0.1, 20, 80, 240, 1000);
	scene.background_color = get_color(0,0,0);
	scene.diffuse = 0.2;

	unsigned char * screen = calloc(scene.screen_width*scene.screen_height*3, sizeof(unsigned char));
	draw_on_canvas(screen, scene);

	unsigned int VAO;	
	unsigned int program; 
	unsigned int texture;

	GLFWwindow  * window = opengl_init(&VAO, &program, &texture);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene.screen_width, scene.screen_height, 0, GL_RGB ,GL_UNSIGNED_BYTE, screen);
	glGenerateMipmap(GL_TEXTURE_2D);

	int i = 0;

	while(!glfwWindowShouldClose(window)) {
		handle_close(window);
		glClearColor(0.3, 0.6, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();

		unsigned char * screen = calloc(scene.screen_width*scene.screen_height*3, sizeof(unsigned char));
		draw_on_canvas(screen, scene);

		i ++;
		if (i > 60) {
			exit(0);
		}
	}

	glfwTerminate();
	free(screen);
}
