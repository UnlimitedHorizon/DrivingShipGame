// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <glut.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
// 引入SOIL库
#include <SOIL/SOIL.h>
// 引入GLM库
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>
// 包含着色器加载库
#include "shader.h"
// 包含相机控制辅助类
#include "camera.h"
// 包含纹理加载类
#include "texture.h"
// 加载模型的类
#include "model.h"
// 水面效果类
#include "fluid.h"

// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// 鼠标移动回调函数原型声明
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// 鼠标滚轮回调函数原型声明
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 场景中移动
void do_movement();

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// 用于相机交互参数
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // 按键情况记录
GLfloat deltaTime = 0.0f; // 当前帧和上一帧的时间差
GLfloat lastFrame = 0.0f; // 上一帧时间
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 lampPos(1.2f, 1.2f, 1.2f);
Model *the_model;

void particles_animation(GLfloat delta, const Model &obj, particle_system p[])
{
	//smoke1 animation
	glm::mat4 base = obj.movement * obj.rotation;
	particle_system *smoke1 = &p[0];
	smoke1->animation(delta);
	for (int i = 0; i < fmin(delta * 1000, 1000); ++i)
	{
		glm::vec4 pos(obj.x7, obj.y7, obj.z7, 1.0);
		glm::vec4 v((rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 0.3 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, 0, 0, 0.5, 0.5, 0.5);
		smoke1->particles.push_back(tmp);
	}
	particle_system *smoke2 = &p[1];
	smoke2->animation(delta);
	for (int i = 0; i < fmin(delta * 1000, 1000); ++i)
	{
		glm::vec4 pos(obj.x8, obj.y8, obj.z8, 1.0);
		glm::vec4 v((rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 0.3 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, 0, 0, 0.5, 0.5, 0.5);
		smoke2->particles.push_back(tmp);
	}
	particle_system *spray = &p[2];
	spray->animation(delta);
	if (obj.move_speed > 0) for (int i = 0; i < fmin(delta * 1000, 1000); ++i)
	{
		float x = rand() / (RAND_MAX + 0.0) * (obj.box1x1 - obj.box1x0) + obj.box1x0;
		float y = rand() / (RAND_MAX + 0.0) * (obj.box1y1 - obj.box1y0) + obj.box1y0;
		float z = rand() / (RAND_MAX + 0.0) * (obj.box1z1 - obj.box1z0) + obj.box1z0;
		glm::vec4 pos(x, y, z, 1.0);
		glm::vec4 v(0.05, (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.1, -0.7 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, -0.1, 0, 0.7, 0.7, 1.0);
		spray->particles.push_back(tmp);
	}
	particle_system *spray2 = &p[3];
	spray2->animation(delta);
	if (obj.move_speed > 0) for (int i = 0; i < fmin(delta * 1000, 1000); ++i)
	{
		float x = rand() / (RAND_MAX + 0.0) * (obj.box2x1 - obj.box2x0) + obj.box2x0;
		float y = rand() / (RAND_MAX + 0.0) * (obj.box2y1 - obj.box2y0) + obj.box2y0;
		float z = rand() / (RAND_MAX + 0.0) * (obj.box2z1 - obj.box2z0) + obj.box2z0;
		glm::vec4 pos(x, y, z, 1.0);
		glm::vec4 v(0.05, (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.1, -0.7 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, -0.1, 0, 0.7, 0.7, 1.0);
		spray2->particles.push_back(tmp);
	}
	particle_system *spray3 = &p[4];
	spray3->animation(delta);
	if (obj.move_speed > 0) for (int i = 0; i < fmin(delta * 1000, 1000); ++i)
	{
		glm::vec4 pos(obj.x9, obj.y9, obj.z9, 1.0);
		glm::vec4 v(-0.15 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 0.02 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.1, -0.6 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, -0.1, 0, 0.7, 0.7, 1.0);
		spray3->particles.push_back(tmp);

		pos = glm::vec4(obj.x9, obj.y9, obj.z9, 1.0);
		v = glm::vec4(0.15 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 0.02 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.1, -0.6 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp1(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, -0.1, 0, 0.7, 0.7, 1.0);
		spray3->particles.push_back(tmp1);

		pos = glm::vec4(obj.x10_0, obj.y10, obj.z10, 1.0);
		v = glm::vec4(-0.15 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 0.02 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.1, -0.6 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp2(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, -0.1, 0, 0.7, 0.7, 1.0);
		spray3->particles.push_back(tmp2);

		pos = glm::vec4(obj.x10_1, obj.y10, obj.z10, 1.0);
		v = glm::vec4(0.15 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 0.02 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.1, -0.6 + (rand() - RAND_MAX / 2.0) / (RAND_MAX / 2.0) * 0.05, 1.0);
		pos = base * pos;
		v = obj.rotation * v;
		particle tmp3(1.0, 1.0, 1.0, pos.x, pos.y, pos.z,
			v.x, v.y, v.z,
			0, -0.1, 0, 0.7, 0.7, 1.0);
		spray3->particles.push_back(tmp3);
	}
}

int main(int argc, char** argv)
{
	if (!glfwInit())	// 初始化glfw库
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// 开启OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of loading model with AssImp with light on", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);

	// 注册窗口键盘事件回调函数
	glfwSetKeyCallback(window, key_callback);
	// 注册鼠标事件回调函数
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// 注册鼠标滚轮事件回调函数
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// 鼠标捕获 停留在程序内
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化GLEW 获取OpenGL函数
	glewExperimental = GL_TRUE; // 让glew获取所有拓展函数
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) 
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	//Section1 加载模型数据
	Model objModel;
	if (!objModel.loadModel("Kutuzov1952.obj"))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	the_model = &objModel;
	particle_system particle_systems[20];
	// Section2 准备着色器程序
	Shader shader("model.vertex", "model.frag");
	Shader particle_shader("particle.vertex", "particle.frag");
	Shader fluidShader("texture.vs", "texture.fs");

	srand(1);
	int rowSize = 100, columnSize = 100;    // 横纵总网格点数
	int refreshCenterX = 0, refreshCenterZ = 0;
	int refreshRowSize = 10, refreshColumnSize = 10;    // 动态刷新部分横纵网格点数
	int step = 1;    // 网格边长
	GLfloat refreshTime = 0.08f;  // 刷新时间
	Fluid f(rowSize, columnSize, step, refreshTime, 1, 0.2, 0);
	f.setRefreshRange(
		(rowSize - refreshRowSize) / 2 + refreshCenterX / step,
		(rowSize + refreshRowSize) / 2 + refreshCenterX / step,
		(columnSize - refreshColumnSize) / 2 + refreshCenterZ / step,
		(columnSize + refreshColumnSize) / 2 + refreshCenterZ / step);
	glm::mat4 modelFluid;
	float transX = -rowSize * step / 2;
	float transY = -0.8f;
	float transZ = -columnSize * step / 2;
	modelFluid = glm::translate(modelFluid, glm::vec3(transX, transY, transZ)); // 调整位置
	float scaleX = 1.0f;
	float scaleY = 0.07f;
	float scaleZ = 1.0f;
	modelFluid = glm::scale(modelFluid, glm::vec3(scaleX, scaleY, scaleZ)); // 调整比例

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	lastFrame = (GLfloat)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		do_movement(); // 根据用户操作情况 更新相机属性
		objModel.animation(deltaTime);
		particles_animation(deltaTime, objModel, particle_systems);

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		// 设置光源属性 点光源
		GLint lightAmbientLoc = glGetUniformLocation(shader.programId, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(shader.programId, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(shader.programId, "light.specular");
		GLint lightPosLoc = glGetUniformLocation(shader.programId, "light.position");
		GLint attConstant = glGetUniformLocation(shader.programId, "light.constant");
		GLint attLinear = glGetUniformLocation(shader.programId, "light.linear");
		GLint attQuadratic = glGetUniformLocation(shader.programId, "light.quadratic");
		glUniform3f(lightAmbientLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightDiffuseLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
		// 设置衰减系数
		glUniform1f(attConstant, 1.0f);
		glUniform1f(attLinear, 0.09f);
		glUniform1f(attQuadratic, 0.032f);
		// 设置观察者位置
		GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos");
		glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // 投影矩阵
		glm::mat4 view = camera.getViewMatrix(); // 视变换矩阵
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model = objModel.movement * objModel.rotation;
		//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f)); // 适当缩放模型
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // 适当下调位置
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		// 这里填写场景绘制代码
		objModel.draw(shader);

		glBindVertexArray(0);
		glUseProgram(0);


		particle_shader.use();
		// 设置光源属性 点光源
		GLint lightAmbientLoc1 = glGetUniformLocation(particle_shader.programId, "light.ambient");
		GLint lightDiffuseLoc1 = glGetUniformLocation(particle_shader.programId, "light.diffuse");
		GLint lightSpecularLoc1 = glGetUniformLocation(particle_shader.programId, "light.specular");
		GLint lightPosLoc1 = glGetUniformLocation(particle_shader.programId, "light.position");
		GLint attConstant1 = glGetUniformLocation(particle_shader.programId, "light.constant");
		GLint attLinear1 = glGetUniformLocation(particle_shader.programId, "light.linear");
		GLint attQuadratic1 = glGetUniformLocation(particle_shader.programId, "light.quadratic");
		glUniform3f(lightAmbientLoc1, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightDiffuseLoc1, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightSpecularLoc1, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc1, lampPos.x, lampPos.y, lampPos.z);
		// 设置衰减系数
		glUniform1f(attConstant1, 1.0f);
		glUniform1f(attLinear1, 0.09f);
		glUniform1f(attQuadratic1, 0.032f);
		// 设置观察者位置
		GLint viewPosLoc1 = glGetUniformLocation(particle_shader.programId, "viewPos");
		glUniform3f(viewPosLoc1, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projection1 = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // 投影矩阵
		glm::mat4 view1 = camera.getViewMatrix(); // 视变换矩阵
		glUniformMatrix4fv(glGetUniformLocation(particle_shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection1));
		glUniformMatrix4fv(glGetUniformLocation(particle_shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view1));
		glm::mat4 model1;
		//model1 = glm::scale(model1, glm::vec3(1.5f, 1.5f, 1.5f)); // 适当缩小模型
		model1 = glm::translate(model1, glm::vec3(0.0f, -0.5f, 0.0f)); // 适当下调位置
		glUniformMatrix4fv(glGetUniformLocation(particle_shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model1));
		// 这里填写场景绘制代码
		for (int i = 0; i < 5; ++i)
			particle_systems[i].draw(particle_shader); // 绘制物体

		glBindVertexArray(0);
		glUseProgram(0);


		fluidShader.use();
		// 设置观察者位置
		GLint viewPosLocFluid = glGetUniformLocation(fluidShader.programId, "viewPos");
		glUniform3f(viewPosLocFluid, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projectionFluid = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // 投影矩阵
		glm::mat4 viewFluid = camera.getViewMatrix(); // 视变换矩阵
		glUniformMatrix4fv(glGetUniformLocation(fluidShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projectionFluid));
		glUniformMatrix4fv(glGetUniformLocation(fluidShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(viewFluid));
		glUniformMatrix4fv(glGetUniformLocation(fluidShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(modelFluid));
		// 这里填写场景绘制代码
		refreshCenterX = (int)(objModel.movement[3][2]);
		refreshCenterZ = (int)(objModel.movement[3][0]);
		f.setRefreshRange(
			(rowSize - refreshRowSize) / 2 + refreshCenterX / step,
			(rowSize + refreshRowSize) / 2 + refreshCenterX / step,
			(columnSize - refreshColumnSize) / 2 + refreshCenterZ / step,
			(columnSize + refreshColumnSize) / 2 + refreshCenterZ / step);

		f.animation(deltaTime);  // 水面刷新
		glDisable(GL_CULL_FACE);
		f.draw(fluidShader);
		glEnable(GL_CULL_FACE);

		glBindVertexArray(0);
		glUseProgram(0);


		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS && !keyPressedStatus[key])
		{
			if (key == GLFW_KEY_W && the_model->move_speed < 4)
				the_model->move_speed += 1;
			if (key == GLFW_KEY_S && the_model->move_speed > -2)
				the_model->move_speed -= 1;
			if (key == GLFW_KEY_A && the_model->rotation_speed < 2)
				the_model->rotation_speed += 1;
			if (key == GLFW_KEY_D && the_model->rotation_speed > -2)
				the_model->rotation_speed -= 1;
			keyPressedStatus[key] = true;
		}
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // 首次鼠标移动
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false; 
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}
// 由相机辅助类处理鼠标滚轮控制
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}
// 由相机辅助类处理键盘控制
void do_movement()
{
	/*if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);*/
	glm::vec4 pos(camera.position.x, camera.position.y, camera.position.z, 1.0);
	glm::mat4 unit(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
	pos = glm::translate(unit, glm::vec3(the_model->dir[0], the_model->dir[1], the_model->dir[2])
		* (the_model->speed_unit * the_model->move_speed)) * pos;
	camera.position.x = pos.x;
	camera.position.y = pos.y;
	camera.position.z = pos.z;
}