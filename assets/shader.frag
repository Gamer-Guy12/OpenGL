#version 430 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D texture0;

void main()
{
	FragColor = texture(texture0, texCoord) * vertexColor;
}
