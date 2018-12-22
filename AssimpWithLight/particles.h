#ifndef PARTICLES_H
#define PARTICLES_H
#include <GLEW/glew.h>
#include <glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <list>

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
	};
	~particle() {};
	bool animation(GLfloat delta)
	{
		life -= delta; fade -= dfade * delta;
		vx += ax * delta; vy += ay * delta; vz += az * delta;
		x += vx * delta; y += vy * delta; z += vz * delta;
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
		{
			while (it != particles.end() && !it->animation(delta))
				it = particles.erase(it);
		}
	}
};
#endif