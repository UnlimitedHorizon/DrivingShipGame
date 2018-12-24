#version 330 core
in vec2 textCoord;

out vec4 color;

// Texture samplers
uniform sampler2D textureImage;

void main()
{
	color = texture(textureImage, textCoord);
}