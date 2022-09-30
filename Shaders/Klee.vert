//Standard for fragment and vertex shaders
#version 330 core

//Calls OpenGL to assign the vector 3 to a position "aPos"
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;

out vec2 texCoord;
out vec3 normCoord;
out vec3 fragPos;

//Assignment of Transformation Matrix
uniform mat4 transform;

uniform mat4 projection;

uniform mat4 view; 

//Creates a single column matrix in the current position
void main()
{
	gl_Position = projection * view * transform * vec4(aPos, 1);

	texCoord = aTex;
	normCoord = mat3( transpose(inverse(transform)) ) * vertexNormal;
	fragPos = vec3(transform * vec4(aPos, 1));
}