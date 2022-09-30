/************************************************************
This code was written for the completion of the requirement:
GDPHYSX XX22 - Particle Motion Homework.

NAME: GALURA, MARC LAWRENCE C.
SECTION: XX22
DATE: SEPTEMBER 25, 2022
*************************************************************/

//Headers used for the program
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

//Defines the type for the output window screen
GLFWwindow* window;

//Variables for the ouput window size
float width = 800.0f;
float height = 800.0f;

//Variables for the position of the object
float xPos = 0.0f;
float yPos = 0.0f;
float zPos = 5.0f;

//Variable for time, gravity, and force
int gravityState;
int forceState;

//Ensures that GLFW registers the space key as one input instead of querying every frame
void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Allows the user to exit the program with the escape key
	if (key == GLFW_KEY_ESCAPE && action == 1)
		glfwSetWindowShouldClose(window, true);

	//Pressing E activates gravity
	if (key == GLFW_KEY_E && action == 1)
		gravityState = 1;

	//Pressing W applies force
	if (key == GLFW_KEY_W && action == 1)
		forceState = 1;

	//Presing Q resets the position and the forces
	if (key == GLFW_KEY_Q && action == 1)
	{
		xPos = 0.0f;
		yPos = 0.0f;
		zPos = 5.0f;

		gravityState = 0;
		forceState = 0;
	}
}

//Creates an Identity matrix and a Transformation matrix
glm::mat4 identity = glm::mat4(1.0f);
glm::mat4 transform;

//Creates the perspective projection matrix
glm::mat4 projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 100.0f);

glm::vec3 cameraPos = glm::vec3(0, 0, -50.0f);
glm::vec3 WorldUp = glm::vec3(0, 1, 0);

glm::mat4 camPositionMat = glm::translate(identity, (cameraPos * -1.0f));
glm::vec3 centerPos = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 F = glm::normalize(centerPos - cameraPos);
glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));
glm::vec3 U = glm::normalize(glm::cross(R, F));

glm::mat4 cameraOrientationMatrix = glm::mat4(glm::vec4(R, 0), glm::vec4(U, 0), glm::vec4((F * -1.0f), 0), glm::vec4(glm::vec3(0, 0, 0), 1));
glm::mat4 view = cameraOrientationMatrix * camPositionMat;


//Main function of the program
int main(void)
{
	//Determines if the glfw library has been initialized
	if (!glfwInit())
		return -1;

	//Creates a windowed mode window and its OpenGL context
	window = glfwCreateWindow(width, height, "HW1-GALURA_MARC", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Makes the current window's context current
	glfwMakeContextCurrent(window);
	gladLoadGL();

	//Relative path to the mesh and contains the attributes of the object
	std::string path = "3D/BombObj.obj";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;
	tinyobj::attrib_t attributes;

	//Loads the mesh of the 3D object selected
	bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());
	std::vector<GLuint> mesh_indices;

	//Iterates through all of the vertices and push the indices to the vector
	for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
	{
		mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
	}

	//Initialization of the array of vertex data and loops through all of the indices
	std::vector<GLfloat> fullVertexData;
	for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
	{
		//Assignment of index data for easier access
		tinyobj::index_t vData = shapes[0].mesh.indices[i];

		//Pushes the XYZ positions of the Vertex respectively
		fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
		fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
		fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);

		//Pushes the normals of the vertices respectively
		fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
		fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
		fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);

		//Pushes the UT values of the Tex Coords respectively
		fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
		fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);
	}

	//Vertically flips the image to prevent an upside-down image once loaded
	stbi_set_flip_vertically_on_load(true);

	//Variables for the details of the image
	int img_width, img_height, colorChannel;

	//Loads the image selected
	unsigned char* tex_bytes = stbi_load("3D/BombTex.png", &img_width, &img_height, &colorChannel, 0);

	//Generation of 1 texture onto the 3D model
	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Repeats and clamps the texture image on the object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);

	//Mipmaps used for the object textures
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex_bytes);

	//Allows the program to determine which side to render
	glEnable(GL_DEPTH_TEST);

	//Initializes the vertex and fragment shaders that will be used for the 3D object selected
	std::fstream vertstream("Shaders/Klee.vert");
	std::fstream fragstream("Shaders/Klee.frag");
	std::stringstream vertStrStream;
	std::stringstream fragStrStream;

	vertStrStream << vertstream.rdbuf();
	fragStrStream << fragstream.rdbuf();

	std::string vertStr = vertStrStream.str();
	std::string fragStr = fragStrStream.str();

	const char* vertSrc = vertStr.c_str();
	const char* fragSrc = fragStr.c_str();

	//Compiles the shaders and creates shader programs according to the vertex and fragment shaders used
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertSrc, NULL);
	glCompileShader(vertShader);

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSrc, NULL);
	glCompileShader(fragShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);

	//Initializes VAO and VBO buffers
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData
	(
		GL_ARRAY_BUFFER,
		sizeof(GLfloat) * fullVertexData.size(),
		fullVertexData.data(),
		GL_DYNAMIC_DRAW
	);

	//Creates an offset for the vertex array
	GLintptr uvPtr = 6 * sizeof(GLfloat);
	GLintptr normPtr = 3 * sizeof(GLfloat);

	//Obtains the position, normal, and UV data from the array (Index, Position, Data Type, Normalized State, Floats present)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)normPtr);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)uvPtr);

	//Enables the vertex attribute arrays and binds the buffers
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Retrieves the time
	float time = glfwGetTime();

	//Loops until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Rendering of the object
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//Manipulates the position, size, and rotation of the object
		transform = glm::translate(identity, glm::vec3(xPos, yPos, zPos));
		transform = glm::scale(transform, glm::vec3(10.0f, 10.0f, 10.0f));
		transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0, 1, 0));

		//Calls the function that allows for a singular input of keys
		glfwSetKeyCallback(window, input_callback);

		//Applies the force
		if (forceState == 1)
			xPos = xPos - (0.05 * time);
		
		//Applies the gravity
		if (gravityState == 1)
			yPos = yPos + (-0.05 * time);

		//Applies the uniform matrices from the vertex and fragment shaders while using the respective shader program
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 view2 = glm::lookAt(cameraPos, centerPos, WorldUp);

		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		GLuint tex0Loc = glGetUniformLocation(shaderProgram, "tex0");
		glUniform1i(tex0Loc, 0);
		glBindTexture(GL_TEXTURE_2D, texture);

		//Uses the shader program and draws the vertices of the object
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

		//Swaps the front and back buffers
		glfwSwapBuffers(window);

		//Poll for and process events
		glfwPollEvents();
	}
}

/***********************************************************************************************************************

This is to certify that this project is my own work, based on my personal efforts in studying and applying the concepts
learned. I have constructed the functions and their respective algorithms and corresponding code by myself. The program
was run, tested, and debugged by my own efforts.  I further certify that I have not copied in part or whole or otherwise
plagiarized the work of other students and/or persons.

										  Marc Lawrence C. Galura, DLSU ID# 12023817

***********************************************************************************************************************/
