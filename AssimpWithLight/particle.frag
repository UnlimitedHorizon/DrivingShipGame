#version 330 core

in vec3 FragPos;
in vec4 vertColor;

// 光源属性结构体
struct LightAttr
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	// 衰减常数
	float linear;   // 衰减一次系数
	float quadratic; // 衰减二次系数
};

uniform LightAttr light;
uniform vec3 viewPos;

out vec4 color;

void main()
{
	// 环境光成分
	vec3	ambient = light.ambient * vec3(vertColor);

	vec3	result = ambient;
	color	= vec4(result , vertColor[3]);
}