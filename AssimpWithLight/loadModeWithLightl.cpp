// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <glut.h>
// ����GLFW��
#include <GLFW/glfw3.h>
// ����SOIL��
#include <SOIL/SOIL.h>
// ����GLM��
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>
// ������ɫ�����ؿ�
#include "shader.h"
// ����������Ƹ�����
#include "camera.h"
// �������������
#include "texture.h"
// ����ģ�͵���
#include "model.h"
// ˮ��Ч����
#include "fluid.h"

// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// ����ƶ��ص�����ԭ������
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// �����ֻص�����ԭ������
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// �������ƶ�
void do_movement();

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(0.0f, 0.3f, -3.0f), glm::vec3(0.0, 1.0, 0.0), 180.0);
glm::vec3 lampPos(1.2f, 1.2f, 1.2f);
Model *the_model;

bool waveDisplay = true;

void particles_animation(GLfloat delta, const Model &obj, particle_system p[])
{
	//smoke1 animation
	const float rho = 2.0;
	glm::mat4 base = obj.movement * obj.rotation;
	particle_system *smoke1 = &p[0];
	smoke1->animation(delta);
	for (int i = 0; i < fmin(delta * 1000 * rho, 1000); ++i)
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
	for (int i = 0; i < fmin(delta * 1000 * rho, 1000); ++i)
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
	if (obj.move_speed > 0) for (int i = 0; i < fmin(delta * 1000 * rho, 1000); ++i)
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
	if (obj.move_speed > 0) for (int i = 0; i < fmin(delta * 1000 * rho, 1000); ++i)
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
	if (obj.move_speed > 0) for (int i = 0; i < fmin(delta * 1000 * rho, 1000); ++i)
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
	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of loading model with AssImp with light on", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	glfwSetKeyCallback(window, key_callback);
	// ע������¼��ص�����
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// ע���������¼��ص�����
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// ��겶�� ͣ���ڳ�����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��ʼ��GLEW ��ȡOpenGL����
	glewExperimental = GL_TRUE; // ��glew��ȡ������չ����
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) 
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	//Section1 ����ģ������
	Model objModel;
	if (!objModel.loadModel("Kutuzov1952.obj", 1))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	the_model = &objModel;
	particle_system particle_systems[20];

	Model island;
	if (!island.loadModel("Island.obj", 0))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	//Section2 ������������
	// ָ����Χ�еĶ������� λ��
	GLfloat skyboxVertices[] = {
		// ����
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, -1.0f, -1.0f,	// B
		1.0f, -1.0f, -1.0f,		// C
		1.0f, -1.0f, -1.0f,		// C
		1.0f, 1.0f, -1.0f,		// D
		-1.0f, 1.0f, -1.0f,		// A

		// �����
		-1.0f, -1.0f, 1.0f,		// E
		-1.0f, -1.0f, -1.0f,	// B
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, 1.0f, 1.0f,		// F
		-1.0f, -1.0f, 1.0f,		// E

		// �Ҳ���
		1.0f, -1.0f, -1.0f,		// C
		1.0f, -1.0f, 1.0f,		// G
		1.0f, 1.0f, 1.0f,		// H
		1.0f, 1.0f, 1.0f,		// H
		1.0f, 1.0f, -1.0f,		// D
		1.0f, -1.0f, -1.0f,		// C

		// ����
		-1.0f, -1.0f, 1.0f,  // E
		-1.0f, 1.0f, 1.0f,  // F
		1.0f, 1.0f, 1.0f,  // H
		1.0f, 1.0f, 1.0f,  // H
		1.0f, -1.0f, 1.0f,  // G
		-1.0f, -1.0f, 1.0f,  // E

		// ����
		-1.0f, 1.0f, -1.0f,  // A
		1.0f, 1.0f, -1.0f,  // D
		1.0f, 1.0f, 1.0f,  // H
		1.0f, 1.0f, 1.0f,  // H
		-1.0f, 1.0f, 1.0f,  // F
		-1.0f, 1.0f, -1.0f,  // A

		// ����
		-1.0f, -1.0f, -1.0f,  // B
		-1.0f, -1.0f, 1.0f,   // E
		1.0f, -1.0f, 1.0f,    // G
		1.0f, -1.0f, 1.0f,    // G
		1.0f, -1.0f, -1.0f,   // C
		-1.0f, -1.0f, -1.0f,  // B
	};

	// Section3 ׼���������
	GLuint skyBoxVAOId, skyBoxVBOId;
	glGenVertexArrays(1, &skyBoxVAOId);
	glGenBuffers(1, &skyBoxVBOId);
	glBindVertexArray(skyBoxVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Section4 ��������
	//GLuint cubeTextId = TextureHelper::load2DTexture("../resources/textures/container.jpg");
	std::vector<const char*> faces;
	faces.push_back("../resources/skyboxes/sky/sky_rt.jpg");
	faces.push_back("../resources/skyboxes/sky/sky_lf.jpg");
	faces.push_back("../resources/skyboxes/sky/sky_up.jpg");
	faces.push_back("../resources/skyboxes/sky/sky_dn.jpg");
	faces.push_back("../resources/skyboxes/sky/sky_bk.jpg");
	faces.push_back("../resources/skyboxes/sky/sky_ft.jpg");
	GLuint skyBoxTextId = TextureHelper::loadCubeMapTexture(faces);

	// Section2 ׼����ɫ������
	Shader shader("model.vertex", "model.frag");
	Shader particle_shader("particle.vertex", "particle.frag");
	Shader fluidShader("texture.vs", "texture.fs");
	Shader skyBoxShader("skybox.vertex", "skybox.frag");


	srand(1);
	int rowSize = 100, columnSize = 100;    // �������������
	int refreshCenterX = 0, refreshCenterZ = 0;
	int refreshRowSize = 10, refreshColumnSize = 10;    // ��̬ˢ�²��ֺ����������
	int step = 1;    // ����߳�
	GLfloat refreshTime = 0.08f;  // ˢ��ʱ��
	Fluid f(rowSize, columnSize, step, refreshTime, 1, 0.2, 0);
	f.setRefreshRange(
		(rowSize - refreshRowSize) / 2 + refreshCenterX / step,
		(rowSize + refreshRowSize) / 2 + refreshCenterX / step,
		(columnSize - refreshColumnSize) / 2 + refreshCenterZ / step,
		(columnSize + refreshColumnSize) / 2 + refreshCenterZ / step);
	glm::mat4 modelFluid;
	float transX = -rowSize * step / 2;
	float transY = -0.55f;
	float transZ = -columnSize * step / 2;
	modelFluid = glm::translate(modelFluid, glm::vec3(transX, transY, transZ)); // ����λ��
	float scaleX = 1.0f;
	float scaleY = 0.05f;
	float scaleZ = 1.0f;
	modelFluid = glm::scale(modelFluid, glm::vec3(scaleX, scaleY, scaleZ)); // ��������

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LESS);

	lastFrame = (GLfloat)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������
		objModel.animation(deltaTime);
		particles_animation(deltaTime, objModel, particle_systems);

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// ����skyBox
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL); // ��Ȳ������� С�ڵ���
		skyBoxShader.use();
		glm::mat4 projection0 = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 view0 = glm::mat4(glm::mat3(camera.getViewMatrix())); // �ӱ任���� �Ƴ�translate����
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection0));
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view0));

		glBindVertexArray(skyBoxVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextId); // ע��󶨵�CUBE_MAP
		glUniform1i(glGetUniformLocation(skyBoxShader.programId, "skybox"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//objModel.draw(shader); // ��������
		glBindVertexArray(0);
		glUseProgram(0);
		glDepthFunc(GL_LESS);

		shader.use();
		glDepthMask(GL_TRUE);
		// ���ù�Դ���� ���Դ
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
		// ����˥��ϵ��
		glUniform1f(attConstant, 1.0f);
		glUniform1f(attLinear, 0.00f);
		glUniform1f(attQuadratic, 0.0f);
		// ���ù۲���λ��
		GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos");
		glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model = objModel.movement * objModel.rotation;
		//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f)); // �ʵ�����ģ��
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // �ʵ��µ�λ��
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		// ������д�������ƴ���
		objModel.draw(shader); // ��������
		glBindVertexArray(0);
		glUseProgram(0);
		
		shader.use();
		glDepthMask(GL_TRUE);
		// ���ù�Դ���� ���Դ
		GLint lightAmbientLoc2 = glGetUniformLocation(shader.programId, "light.ambient");
		GLint lightDiffuseLoc2 = glGetUniformLocation(shader.programId, "light.diffuse");
		GLint lightSpecularLoc2 = glGetUniformLocation(shader.programId, "light.specular");
		GLint lightPosLoc2 = glGetUniformLocation(shader.programId, "light.position");
		GLint attConstant2 = glGetUniformLocation(shader.programId, "light.constant");
		GLint attLinear2 = glGetUniformLocation(shader.programId, "light.linear");
		GLint attQuadratic2 = glGetUniformLocation(shader.programId, "light.quadratic");
		glUniform3f(lightAmbientLoc2, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightDiffuseLoc2, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightSpecularLoc2, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc2, lampPos.x, lampPos.y, lampPos.z);
		// ����˥��ϵ��
		glUniform1f(attConstant2, 1.0f);
		glUniform1f(attLinear2, 0.00f);
		glUniform1f(attQuadratic2, 0.0f);
		// ���ù۲���λ��
		GLint viewPosLoc2 = glGetUniformLocation(shader.programId, "viewPos");
		glUniform3f(viewPosLoc2, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projection2 = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 view2 = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection2));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view2));
		glm::mat4 model2;
		model2 = glm::scale(model2, glm::vec3(0.01f, 0.01f, 0.01f)); // �ʵ�����ģ��
		model2 = glm::translate(model2, glm::vec3(200.0f, -56.0f, 200.0f)); // �ʵ��µ�λ��
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model2));
		// ������д�������ƴ���
		island.draw(shader); // ��������
		glBindVertexArray(0);
		glUseProgram(0);

		particle_shader.use();
		// ���ù�Դ���� ���Դ
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
		// ����˥��ϵ��
		glUniform1f(attConstant1, 1.0f);
		glUniform1f(attLinear1, 0.0f);
		glUniform1f(attQuadratic1, 0.0f);
		// ���ù۲���λ��
		GLint viewPosLoc1 = glGetUniformLocation(particle_shader.programId, "viewPos");
		glUniform3f(viewPosLoc1, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projection1 = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 view1 = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(particle_shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection1));
		glUniformMatrix4fv(glGetUniformLocation(particle_shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view1));
		glm::mat4 model1;
		//model1 = glm::scale(model1, glm::vec3(1.5f, 1.5f, 1.5f)); // �ʵ���Сģ��
		model1 = glm::translate(model1, glm::vec3(0.0f, -0.5f, 0.0f)); // �ʵ��µ�λ��
		glUniformMatrix4fv(glGetUniformLocation(particle_shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model1));
		// ������д�������ƴ���
		for (int i = 0; i < 5; ++i)
			particle_systems[i].draw(particle_shader); // ��������

		glBindVertexArray(0);
		glUseProgram(0);



		fluidShader.use();
		// ���ù۲���λ��
		GLint viewPosLocFluid = glGetUniformLocation(fluidShader.programId, "viewPos");
		glUniform3f(viewPosLocFluid, camera.position.x, camera.position.y, camera.position.z);
		glm::mat4 projectionFluid = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 viewFluid = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(fluidShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projectionFluid));
		glUniformMatrix4fv(glGetUniformLocation(fluidShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(viewFluid));
		glUniformMatrix4fv(glGetUniformLocation(fluidShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(modelFluid));
		// ������д�������ƴ���
		refreshCenterX = (int)(objModel.movement[3][2]);
		refreshCenterZ = (int)(objModel.movement[3][0]);
		f.setRefreshRange(
			(rowSize - refreshRowSize) / 2 + refreshCenterX / step,
			(rowSize + refreshRowSize) / 2 + refreshCenterX / step,
			(columnSize - refreshColumnSize) / 2 + refreshCenterZ / step,
			(columnSize + refreshColumnSize) / 2 + refreshCenterZ / step);

		f.animation(deltaTime);  // ˮ��ˢ��
		if (waveDisplay) {
			glDisable(GL_CULL_FACE);
			f.draw(fluidShader);
			glEnable(GL_CULL_FACE);
		}

		glBindVertexArray(0);
		glUseProgram(0);

		
		
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
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
			if (key == GLFW_KEY_Q)
				waveDisplay = !waveDisplay;

			keyPressedStatus[key] = true;
		}
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // �״�����ƶ�
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false; 
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(-xoffset, yoffset);
}
// ����������ദ�������ֿ���
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}
// ����������ദ����̿���
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
	glm::vec4 pos(0, 0.3, -3.0, 1.0);
	glm::mat4 unit(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
	pos = (the_model->movement) * pos;
	camera.position.x = pos.x;
	camera.position.y = pos.y;
	camera.position.z = pos.z;
	//camera.yawAngle -= the_model->omega_unit * the_model->rotation_speed * deltaTime / 2 / acos(-1) * 360;
}