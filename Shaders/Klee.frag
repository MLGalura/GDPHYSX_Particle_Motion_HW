//Standard for fragment and vertex shaders
#version 330 core
uniform sampler2D tex0;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

//The color of the vertex
out vec4 FragColor;

//Changes color in RGBA format (0-1)
void main()
{
	vec3 normal = normalize(normCoord);

    FragColor =  texture(tex0, texCoord);
}