#ifndef PARTICLES_H
#define PARTICLES_H
#include <GLEW/glew.h>
#include <glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <list>
#include "shader.h"

const float size = 0.002;
const float dx[8] = { -1, -1, -1, -1,  1,  1,  1,  1 };
const float dy[8] = { -1, -1,  1,  1, -1, -1,  1,  1 };
const float dz[8] = { -1,  1, -1,  1, -1,  1, -1,  1 };
const int which[12][3] = { {0, 1, 2}, {1, 2, 3}, {0, 1, 4}, {1, 4, 5}, {4, 5, 6}, {5, 6, 7} , {2, 3, 7}, {2, 6, 7}, {1, 3, 5}, {3, 5, 7}, {0, 2, 4}, {2, 4, 6} };

class particle
{
public:
	float life, fade, dfade, x, y, z, vx, vy, vz, ax, ay, az, r, g, b;
	particle(float _life = 0.0, float _fade = 0.0, float _dfade = 0.0,
		float _x = 0.0, float _y = 0.0, float _z = 0.0,
		float _vx = 0.0, float _vy = 0.0, float _vz = 0.0,
		float _ax = 0.0, float _ay = 0.0, float _az = 0.0,
		float _r = 0.0, float _g = 0.0, float _b = 0.0)
	{
		life = _life; fade = _fade; dfade = _dfade;
		x = _x; y = _y; z = _z;
		vx = _vx; vy = _vy; vz = _vz;
		ax = _ax; ay = _ay; az = _az;
		r = _r; g = _g; b = _b;
	}
	~particle() {}
	void animation(GLfloat delta)
	{
		life -= delta; fade -= dfade * delta;
		vx += ax * delta; vy += ay * delta; vz += az * delta;
		x += vx * delta; y += vy * delta; z += vz * delta;
	}
	bool active()
	{
		return (life > 1e-6 && fade > 1e-6);
	}
};

class particle_system
{
public:
	std::list<particle> particles;
	particle_system()
	{
		particles.clear();
	}
	~particle_system()
	{
		particles.clear();
	}
	void animation(GLfloat delta)
	{
		for (std::list<particle>::iterator it = particles.begin(); it != particles.end(); it++)
			it->animation(delta);
		for (std::list<particle>::iterator it = particles.begin(); it != particles.end();)
			if (!it->active())
				it = particles.erase(it);
			else
				it++;
	}
	void draw(const Shader &shader)
	{
		//std::cout << particles.size() << std::endl;
		GLfloat *vertices = new GLfloat[7 * 12 * 3 * particles.size()];
		//GLfloat *vertices = new GLfloat[3 * 12 * 3 * particles.size()];
		int now = 0;
		for (std::list<particle>::iterator it = particles.begin(); it != particles.end(); ++it)
			for (int i = 0; i < 12; ++i)
				for (int j = 0; j < 3; ++j)
				{
					vertices[now++] = it->x + size * dx[which[i][j]];
					vertices[now++] = it->y + size * dy[which[i][j]];
					vertices[now++] = it->z + size * dz[which[i][j]];
					vertices[now++] = it->r;
					vertices[now++] = it->g;
					vertices[now++] = it->b;
					vertices[now++] = it->fade;
				}
		GLuint VAOId, VBOId;
		glGenVertexArrays(1, &VAOId);
		glBindVertexArray(VAOId);
		glGenBuffers(1, &VBOId);
		glBindBuffer(GL_ARRAY_BUFFER, VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 7 * 12 * 3 * particles.size(), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			7 * sizeof(GL_FLOAT), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
			7 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAOId);
		shader.use();
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12 * particles.size());
		glBindVertexArray(0);
		glUseProgram(0);
		glDeleteVertexArrays(1, &VAOId);
		glDeleteBuffers(1, &VBOId);
		delete[]vertices;
	}
};
#endif