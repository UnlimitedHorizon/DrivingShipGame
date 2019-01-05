#version 330 core

in vec3 TextCoord;
uniform samplerCube  skybox;  // ¡ä¨®sampler2D???asamplerCube

out vec4 color;


void main()
{
	color = texture(skybox, TextCoord);
}