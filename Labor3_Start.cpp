
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "LadeShader.h"

#include <stdlib.h>
#include "LadeBMP.h"

#define PI 3.1415926536

struct myVertexType //selbst definierter Datentyp für die Shaderprogramme
{
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;

	float u, v;

	myVertexType(myVertexType other, vec3 norm) {
		x = other.x;
		y = other.y;
		z = other.z;

		r = other.r;
		g = other.g;
		b = other.b;

		nx = norm[0];
		ny = norm[1];
		nz = norm[2];

		u = 0;
		v = 0;
	}
	myVertexType(myVertexType other, vec3 norm, vec2 uv)
	{
		x = other.x;
		y = other.y;
		z = other.z;

		r = other.r;
		g = other.g;
		b = other.b;

		nx = norm[0];
		ny = norm[1];
		nz = norm[2];

		u = uv[0];
		v = uv[1];
	}

	myVertexType(float x, float y, float z, float r, float g, float b)
	{
		this->x = x;
		this->y = y;
		this->z = z;

		this->r = r;
		this->g = g;
		this->b = b;

		nx = 0;
		ny = 0;
		nz = 0;
	}
	myVertexType() { x = y = z = r = g = b = nx = ny = nz = u = v = 0; }

};

myVertexType vertices[36]; //Array mit drei Vertexen

float ambfac = 0.3;
float diffusionFactor = 0.3;
int shininessFactor = 16;
float specularFactor = 0.6;

vec3 lightDir{ 0, 0, 1 };


//Key-Callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != 0)//nur beim Hinunterdrücken einer Taste
	{
		if (key == GLFW_KEY_A)//Wenn diese Taste "A" war
		{
			if (mods == 1)//Wenn Shift zusätzlich gehalten ist
				ambfac += .1f;//ambienten Faktor erhöhen
			else
				ambfac -= .1f;//ambienten Faktor senken
		}
		if (key == GLFW_KEY_D)//Wenn diese Taste "A" war
		{
			if (mods == 1)//Wenn Shift zusätzlich gehalten ist
				diffusionFactor += .1f;//ambienten Faktor erhöhen
			else
				diffusionFactor -= .1f;//ambienten Faktor senken
		}
		if (key == GLFW_KEY_S)//Wenn diese Taste "A" war
		{
			if (mods == 1)//Wenn Shift zusätzlich gehalten ist
				specularFactor += .1f;//ambienten Faktor erhöhen
			else
				specularFactor -= .1f;//ambienten Faktor senken
		}
		if (key == GLFW_KEY_G)//Wenn diese Taste "A" war
		{
			if (mods == 1)//Wenn Shift zusätzlich gehalten ist
				shininessFactor += 1.0f;//ambienten Faktor erhöhen
			else
				shininessFactor -= 1.0f;//ambienten Faktor senken
		}
	}
}

int main(void)
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, complete_shader_program;
	GLint  matrix_access, ambient, diffusion, specular, shininess, position_access, color_access, normal_access, light_dir_access, uv_access;

	myVertexType cubeEdges[8]
	{
		/*0*/myVertexType(+1.0f,+1.0f,+1.0f,1.0f,0.0f,0.0f),//Koordinate 1,1,1 mit roter Farbe
		/*1*/myVertexType(+1.0f,+1.0f,-1.0f,0.0f,1.0f,0.0f),//Koordinate 1,1,-1 mit grüner Farbe
		/*2*/myVertexType(+1.0f,-1.0f,-1.0f,0.0f,0.0f,1.0f),//blau
		/*3*/myVertexType(+1.0f,-1.0f,+1.0f,1.0f,0.0f,1.0f),//magenta
		/*4*/myVertexType(-1.0f,+1.0f,+1.0f,0.0f,1.0f,1.0f),//cyan
		/*5*/myVertexType(-1.0f,-1.0f,+1.0f,1.0f,1.0f,0.0f),//gelb
		/*6*/myVertexType(-1.0f,-1.0f,-1.0f,1.0f,1.0f,1.0f),//weiß
		/*7*/myVertexType(-1.0f,+1.0f,-1.0f,0.0f,0.0f,0.0f) //schwarz
	};

	cubeEdges[0] = myVertexType( myVertexType(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 0.0f), vec3{ 1, 0, 0 });
	//verkleinere Würfelkoordinaten
	for (int i = 0; i < 8; i++)
	{
		cubeEdges[i].x *= 0.5;
		cubeEdges[i].y *= 0.5;
		cubeEdges[i].z *= 0.5;
	}

	// Define UV coordinates for each face of the cube
	vec2 uv_left[4] = {
		{0.25, 0.5}, {0.5, 0.5},
		{0.25, 0.75}, {0.5, 0.75}
	};
	vec2 uv_front[4] = {
		{0.5, 0.5}, {0.75, 0.5},
		{0.5, 0.75}, {0.75, 0.75}
	};
	vec2 uv_right[4] = {
		{0.75, 0.5}, {1.0, 0.5},
		{0.75, 0.75}, {1.0, 0.75}
	};
	vec2 uv_back[4] = {
		{0.5, 0.25}, {0.75, 0.25},
		{0.5, 0.5}, {0.75, 0.5}
	};
	vec2 uv_top[4] = {
		{0.5, 0.25}, {0.75, 0.25},
		{0.5, 0.5}, {0.75, 0.5}
	};
	vec2 uv_bottom[4] = {
		{0.5, 0.75}, {0.75, 0.75},
		{0.5, 1.0}, {0.75, 1.0}
	};
	vec3 leftside  = { 1, 0, 0 };
	vec3 rightside = { -1, 0, 0 };
	vec3 front = { 0 ,0 ,1 };
	vec3 top = { 0 , 1 , 0 };
	vec3 bottom = { 0 , -1 , 0 };
	vec3 back = { 0 , 0, -1 };

	// Left side
	vertices[0] = myVertexType(cubeEdges[2], leftside, uv_left[0]);
	vertices[1] = myVertexType(cubeEdges[1], leftside, uv_left[1]);
	vertices[2] = myVertexType(cubeEdges[0], leftside, uv_left[2]);
	vertices[3] = myVertexType(cubeEdges[3], leftside, uv_left[3]);
	vertices[4] = myVertexType(cubeEdges[2], leftside, uv_left[0]);
	vertices[5] = myVertexType(cubeEdges[0], leftside, uv_left[2]);

	// Front side
	vertices[6] = myVertexType(cubeEdges[4], front, uv_front[0]);
	vertices[7] = myVertexType(cubeEdges[0], front, uv_front[1]);
	vertices[8] = myVertexType(cubeEdges[3], front, uv_front[2]);
	vertices[9] = myVertexType(cubeEdges[3], front, uv_front[2]);
	vertices[10] = myVertexType(cubeEdges[4], front, uv_front[0]);
	vertices[11] = myVertexType(cubeEdges[5], front, uv_front[3]);

	// Top side
	vertices[12] = myVertexType(cubeEdges[4], top, uv_top[0]);
	vertices[13] = myVertexType(cubeEdges[0], top, uv_top[1]);
	vertices[14] = myVertexType(cubeEdges[1], top, uv_top[2]);
	vertices[15] = myVertexType(cubeEdges[4], top, uv_top[0]);
	vertices[16] = myVertexType(cubeEdges[7], top, uv_top[3]);
	vertices[17] = myVertexType(cubeEdges[1], top, uv_top[2]);

	// Back side
	vertices[18] = myVertexType(cubeEdges[2], back, uv_back[0]);
	vertices[19] = myVertexType(cubeEdges[6], back, uv_back[1]);
	vertices[20] = myVertexType(cubeEdges[1], back, uv_back[2]);
	vertices[21] = myVertexType(cubeEdges[6], back, uv_back[1]);
	vertices[22] = myVertexType(cubeEdges[7], back, uv_back[3]);
	vertices[23] = myVertexType(cubeEdges[1], back, uv_back[2]);

	// Right side
	vertices[24] = myVertexType(cubeEdges[6], rightside, uv_right[0]);
	vertices[25] = myVertexType(cubeEdges[5], rightside, uv_right[1]);
	vertices[26] = myVertexType(cubeEdges[4], rightside, uv_right[2]);
	vertices[27] = myVertexType(cubeEdges[6], rightside, uv_right[0]);
	vertices[28] = myVertexType(cubeEdges[4], rightside, uv_right[2]);
	vertices[29] = myVertexType(cubeEdges[7], rightside, uv_right[3]);

	// Bottom side
	vertices[30] = myVertexType(cubeEdges[5], bottom, uv_bottom[0]);
	vertices[31] = myVertexType(cubeEdges[3], bottom, uv_bottom[1]);
	vertices[32] = myVertexType(cubeEdges[2], bottom, uv_bottom[2]);
	vertices[33] = myVertexType(cubeEdges[5], bottom, uv_bottom[0]);
	vertices[34] = myVertexType(cubeEdges[2], bottom, uv_bottom[2]);
	vertices[35] = myVertexType(cubeEdges[6], bottom, uv_bottom[3]);

	if (!glfwInit())//GLFW initialisieren
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(1280, 1024, "Happy Cube", NULL, NULL);//Fenster mit 1280x1024 Pixel erstellen
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);//Key-Callback setzten (Funktion oben)
	glfwMakeContextCurrent(window);//Verbindung mit dem Fenster
	gladLoadGL(glfwGetProcAddress);//Laden der Fuktionszeiger

	//Erzeugen des Vertex-Shaders
	char* vertex_shader_code = readTextFileIntoString("Shaders/VertexShaderStart.glsl");//Shader aus Datei laden (LadeShader.h)
	if (vertex_shader_code == 0) exit(EXIT_FAILURE);// Sicherstellen, dass die Datei erfolgreich geladen wurde!
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	char info[500];
	int num;
	glGetShaderInfoLog(vertex_shader, 500, &num, info);
	printf("%s", info);

	//Erzeugen des Fragment-Shaders
	char* fragment_shader_code = readTextFileIntoString("Shaders/FragmentShaderStart.glsl");//Shader aus Datei laden (LadeShader.h)
	if (fragment_shader_code == 0) exit(EXIT_FAILURE);// Sicherstellen, dass die Datei erfolgreich geladen wurde!
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderInfoLog(fragment_shader, 500, &num, info);
	printf("%s", info);

	//Linken des Shader-Progamms
	complete_shader_program = glCreateProgram();
	glAttachShader(complete_shader_program, vertex_shader);
	glAttachShader(complete_shader_program, fragment_shader);
	glLinkProgram(complete_shader_program);

	//Uniforme Positionen für den Zugriff während des Renderns
	matrix_access = glGetUniformLocation(complete_shader_program, "matrix");
	ambient = glGetUniformLocation(complete_shader_program, "ambientFactor");
	diffusion = glGetUniformLocation(complete_shader_program, "diffuseFactor");
	specular = glGetUniformLocation(complete_shader_program, "specularFactor");
	shininess = glGetUniformLocation(complete_shader_program, "shininessFactor");


	//Zugriff auf Position und Farbe innerhalb des Vertex-Buffers
	position_access = glGetAttribLocation(complete_shader_program, "position");
	color_access = glGetAttribLocation(complete_shader_program, "color");
	normal_access = glGetAttribLocation(complete_shader_program, "normals");
	uv_access = glGetAttribLocation(complete_shader_program, "texCoords");

	light_dir_access = glGetAttribLocation(complete_shader_program, "lightDir");

	// texture

	GLuint texture;
	int picture_width, picture_height;
	auto const pictureData = loadBMP24("textures/Cube.bmp", &picture_width, &picture_height);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, picture_width, picture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, pictureData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] pictureData;
	glGenerateMipmap(GL_TEXTURE_2D);
	GLuint textureId = glGetUniformLocation(complete_shader_program, "textureSampler");

	//Aufbau und Übermittlung des Vertex-Buffers
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Die Daten für Position und Farbe werden dem Shader mitgeteilt
	glEnableVertexAttribArray(position_access);
	glVertexAttribPointer(position_access, 3, GL_FLOAT, GL_FALSE,
		sizeof(myVertexType), (void*)0);

	glEnableVertexAttribArray(color_access);
	glVertexAttribPointer(color_access, 3, GL_FLOAT, GL_FALSE,
		sizeof(myVertexType), (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(normal_access);
	glVertexAttribPointer(normal_access, 3, GL_FLOAT, GL_FALSE,
		sizeof(myVertexType), (void*)(sizeof(float) * 6));

	glEnableVertexAttribArray(uv_access);
	glVertexAttribPointer(uv_access, 2, GL_FLOAT, GL_FALSE,
		sizeof(myVertexType), (void*)(sizeof(float) * 9));

	//Z-Achse positiv defineren
	glClearDepth(0);
	glDepthFunc(GL_GREATER);//größere Z-Werte überschreiben kleinere
	glEnable(GL_DEPTH_TEST);//Verdeckung der Dreiecke untereinander aktivieren

	while (!glfwWindowShouldClose(window))//Hauptscheife
	{
		mat4x4 m;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(complete_shader_program);//Ab jetzt dieses Shader-Programm nutzen

		//Rotationsmatrix erstellen
		mat4x4_identity(m);//Über die Linmath.h wird eine 4x4 Einheitsmatrix erzeugt
		mat4x4_rotate_Z(m, m, (float)glfwGetTime());//Auf die Matrix m wird eine Rotation um Z aufmultipliziert
		mat4x4_rotate_Y(m, m, (float)glfwGetTime());//Auf die Matrix m wird eine Rotation um Y aufmultipliziert
		mat4x4_rotate_X(m, m, (float)glfwGetTime());//Auf die Matrix m wird eine Rotation um Y aufmultipliziert

		// Textur aktivieren und binden
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureId, 0);

		// mat4x4_rotate_Y(m, m, 2 * PI * 10 / 360 * sin((float)glfwGetTime()));
		glUniformMatrix4fv(matrix_access, 1, GL_FALSE, (const GLfloat*)m);// Die Matrix wird in das Shader-Programm übertragen

		glUniform1f(ambient, ambfac);// Der ambiente Faktor wird in das Shader-Programm übertragen
		glUniform1f(diffusion, diffusionFactor);// Der ambiente Faktor wird in das Shader-Programm übertragen
		glUniform1f(specular, specularFactor);// Der ambiente Faktor wird in das Shader-Programm übertragen
		glUniform1i(shininess, shininessFactor);// Der ambiente Faktor wird in das Shader-Programm übertragen
		glUniform3f(light_dir_access, (GLfloat)lightDir[0], (GLfloat)lightDir[1], (GLfloat)lightDir[2]);

		glDrawArrays(GL_TRIANGLES, 0, 36);//Ein Dreieck wird gezeichnet

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


