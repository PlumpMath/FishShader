//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

//GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//standard libs
#include <stdio.h>
#include <string>
#include <chrono>
#include <iostream>

// Shader sources
const GLchar* vertexSource =
	"#version 150\n"
	"in vec3 position;"
	"in vec3 normal;"
	"uniform mat4 proj;"
	"uniform mat4 view;"
	"uniform mat4 model;"
	"uniform float animationOffset;"
	"out vec3 color;"
	"void main()" 
	"{"
	"	vec3 lightV = normalize(vec3(1,-1,0));"
	"	vec3 mvNorm = vec3(view * model * vec4(normal, 0));"
	"	float diffuse = max(dot(mvNorm, lightV), 0.3);"
	"	vec3 startColor = vec3(1.0, 0.5, 0.0);"
	"	color = startColor * diffuse;"
	"	vec3 newPos = position + vec3(0, 0, 1 + (sin((animationOffset + position.x) * 4) / 16));"
	"	gl_Position = proj * view * model * vec4(newPos, 1.0);"
	"}";

const GLchar* fragmentSource =
    "#version 150\n"
    "in vec3 color;"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(color, 1.0);"
    "}";

int main(int argc, char *argv[])
{
	//start SDL with video module	
	SDL_Init(SDL_INIT_VIDEO);

	//set OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//create window
	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	//create OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//use glew to find experimental/modern OpenGl function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	//create VAO to save all this state we build up
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//init fish
	//(note: starts w/ empty normals)
	GLfloat vertices[] = {

		//0
		//front vertex
		-0.7, 0, 0, 0, 0, 0,

		//1 - 4
		//left side
		-0.4, -0.25, 0.1, 0, 0, 0,
		-0.4, 0.25, 0.1, 0, 0, 0,
		-0.1, 0.3, 0.2, 0, 0, 0,
		-0.1, -0.3, 0.2, 0, 0, 0,

		//5 - 8
		//right side
		-0.4, -0.25, -0.1, 0, 0, 0,
		-0.4, 0.25, -0.1, 0, 0, 0,
		-0.1, 0.3, -0.2, 0, 0, 0,
		-0.1, -0.3, -0.2, 0, 0, 0,

		//9 - 12
		//tail
		0.25, -0.25, 0, 0, 0, 0,
		0.25, 0.25, 0, 0, 0, 0,
		0.7, 0.5, 0, 0, 0, 0,
		0.7, -0.5, 0, 0, 0, 0

	};


	GLuint elements[] = {

		//left side
		0, 1, 2,
		1, 3, 2,
		1, 4, 3,
		4, 9, 3,
		9, 10, 3,

		//tail
		9, 11, 10,
		11, 12, 9,

		//right side
		0, 6, 5,
		5, 6, 7,
		5, 7, 8,
		7, 9, 8,
		7, 10, 9,

		//top
		2, 6, 0,
		2, 7, 6,
		2, 3, 7,
		3, 10, 7,

		//bottom
		0, 5, 1,
		1, 5, 8,
		1, 4, 8,
		9, 4, 8

	};

	//calculate face normals
	for (int t = 0; t < sizeof(elements) / sizeof(GLuint); t += 3) {

		//std::cout << t << std::endl;
		
		int t0 = elements[t];
		int t1 = elements[t+1];
		int t2 = elements[t+2];

		glm::vec3 vert0 = glm::vec3(vertices[ (t0 * 6) + 0 ], vertices[ (t0 * 6) + 1 ], vertices[ (t0 * 6) + 2 ]);
		glm::vec3 vert1 = glm::vec3(vertices[ (t1 * 6) + 0 ], vertices[ (t1 * 6) + 1 ], vertices[ (t1 * 6) + 2 ]);
		glm::vec3 vert2 = glm::vec3(vertices[ (t2 * 6) + 0 ], vertices[ (t2 * 6) + 1 ], vertices[ (t2 * 6) + 2 ]);

		glm::vec3 side0 = vert0 - vert2;
		glm::vec3 side1 = vert1 - vert2;

		glm::vec3 faceNorm = glm::cross(side0, side1);

		//std::cout << faceNorm.x << " "  << faceNorm.y  << " "  << faceNorm.z << std::endl;

		vertices[ (t0 * 6) + 0 + 3 ] += faceNorm.x;
		vertices[ (t0 * 6) + 1 + 3 ] += faceNorm.y;
		vertices[ (t0 * 6) + 2 + 3 ] += faceNorm.z;

		vertices[ (t1 * 6) + 0 + 3 ] += faceNorm.x;
		vertices[ (t1 * 6) + 1 + 3 ] += faceNorm.y;
		vertices[ (t1 * 6) + 2 + 3 ] += faceNorm.z;

		vertices[ (t2 * 6) + 0 + 3 ] += faceNorm.x;
		vertices[ (t2 * 6) + 1 + 3 ] += faceNorm.y;
		vertices[ (t2 * 6) + 2 + 3 ] += faceNorm.z;
	}

	//re-normalize vertex normals
	for (int v = 0; v < sizeof(vertices) / sizeof(GLuint); v += 6) {
		glm::vec3 norm = glm::normalize( glm::vec3(vertices[v + 3], vertices[v + 4], vertices[v + 5]) );

		vertices[v + 3] = norm.x;
		vertices[v + 4] = norm.y;
		vertices[v + 5] = norm.z;
	}

	//create vertex buffer
	GLuint vbo;
	glGenBuffers(1, &vbo);
	//make buffer the active object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//send vertex data to vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//create element buffer
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**) &vertexSource, NULL);
	glCompileShader(vertexShader);

	//fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**) &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//make shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	//link the program to make it "compile"
	glLinkProgram(shaderProgram);
	//actually use the shader program
	glUseProgram(shaderProgram);

	//tell position attribute how to get data from vertices
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
	
	//tell normal attribute how to get data from vertices
	GLint normAttrib = glGetAttribLocation(shaderProgram, "normal");
	glEnableVertexAttribArray(normAttrib);
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) (3*sizeof(float)));

	//get uniforms for the various transformation matrices
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");

	//get uniform for wiggling animation
	GLint uniAnim = glGetUniformLocation(shaderProgram, "animationOffset");

	//set up camera view
	glm::mat4 view = glm::lookAt(
	    glm::vec3(2.0f, 1.2f, 0.8f),
	    glm::vec3(0.0f, 0.0f, 0.0f),
	    glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	//set up field of view / projection matrix
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	//start time
	auto t_start = std::chrono::high_resolution_clock::now();

	//event loop
	SDL_Event windowEvent;
	while (true) {

		if (SDL_PollEvent(&windowEvent)) {
			if (windowEvent.type == SDL_QUIT) break;
		}

		// Clear the screen to black
        glClearColor(0.0f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //depth buffer needs to be cleared for depth testing

        //change transformation with time
		// Calculate transformation
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		//animate rotation
		glm::mat4 model;
		model = glm::rotate(
			model,
			-1.0f * 0.2f * time * glm::radians(180.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		//animate wiggling
		glUniform1f(uniAnim, time);

		//draw all this shit
		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0); //draw fish

		SDL_GL_SwapWindow(window);
	}

	//delete gl stuff
	glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glDeleteVertexArrays(1, &vao);

	//close window & delete context
	SDL_GL_DeleteContext(context);

	SDL_Quit();
	
	return 0;

}