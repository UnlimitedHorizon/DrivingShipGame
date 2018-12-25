#ifndef _MODEL_H_
#define _MODEL_H_

#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include "mesh.h"
#include "texture.h"
#include "matrix.h"
#include "particles.h"

/*
* 代表一个模型 模型可以包含一个或多个Mesh
*/
class Model
{
public:
	void draw(const Shader& shader)
	{
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
			it->draw(shader);
	}
	bool loadModel(const std::string& filePath)
	{
		Assimp::Importer importer;
		if (filePath.empty())
		{
			std::cerr << "Error:Model::loadModel, empty model file path." << std::endl;
			return false;
		}
		const aiScene* sceneObjPtr = importer.ReadFile(filePath, 
			aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!sceneObjPtr
			|| sceneObjPtr->mFlags == AI_SCENE_FLAGS_INCOMPLETE
			|| !sceneObjPtr->mRootNode)
		{
			std::cerr << "Error:Model::loadModel, description: " 
				<< importer.GetErrorString() << std::endl;
			return false;
		}
		this->modelFileDir = filePath.substr(0, filePath.find_last_of('/'));
		if (!this->processNode(sceneObjPtr->mRootNode, sceneObjPtr))
		{
			std::cerr << "Error:Model::loadModel, process node failed."<< std::endl;
			return false;
		}
		//propeller1 axis and box
		double min = 1e10;
		box1x0 = 1e10; box1x1 = -1e10;
		box1y0 = 1e10; box1y1 = -1e10;
		box1z0 = 1e10; box1z1 = -1e10;
		for (int i = 0; i < meshes[propeller1].vertData.size(); ++i)
		{
			if (meshes[propeller1].vertData[i].position.z < min)
			{
				min = meshes[propeller1].vertData[i].position.z;
				x1 = meshes[propeller1].vertData[i].position.x;
				y1 = meshes[propeller1].vertData[i].position.y;
			}
			if (meshes[propeller1].vertData[i].position.x < box1x0)
				box1x0 = meshes[propeller1].vertData[i].position.x;
			if (meshes[propeller1].vertData[i].position.x > box1x1)
				box1x1 = meshes[propeller1].vertData[i].position.x;
			if (meshes[propeller1].vertData[i].position.z < box1z0)
				box1z0 = meshes[propeller1].vertData[i].position.z;
			if (meshes[propeller1].vertData[i].position.z > box1z1)
				box1z1 = meshes[propeller1].vertData[i].position.z;
			if (meshes[propeller1].vertData[i].position.y < box1y0)
				box1y0 = meshes[propeller1].vertData[i].position.y;
			if (meshes[propeller1].vertData[i].position.y > box1y1)
				box1y1 = meshes[propeller1].vertData[i].position.y;
		}
		//propeller2 axis and box
		min = 1e10;
		box2x0 = 1e10; box2x1 = -1e10;
		box2y0 = 1e10; box2y1 = -1e10;
		box2z0 = 1e10; box2z1 = -1e10;
		for (int i = 0; i < meshes[propeller2].vertData.size(); ++i)
		{
			if (meshes[propeller2].vertData[i].position.z < min)
			{
				min = meshes[propeller2].vertData[i].position.z;
				x2 = meshes[propeller2].vertData[i].position.x;
				y2 = meshes[propeller2].vertData[i].position.y;
			}
			if (meshes[propeller2].vertData[i].position.x < box2x0)
				box2x0 = meshes[propeller2].vertData[i].position.x;
			if (meshes[propeller2].vertData[i].position.x > box2x1)
				box2x1 = meshes[propeller2].vertData[i].position.x;
			if (meshes[propeller2].vertData[i].position.z < box2z0)
				box2z0 = meshes[propeller2].vertData[i].position.z;
			if (meshes[propeller2].vertData[i].position.z > box2z1)
				box2z1 = meshes[propeller2].vertData[i].position.z;
			if (meshes[propeller2].vertData[i].position.y < box2y0)
				box2y0 = meshes[propeller2].vertData[i].position.y;
			if (meshes[propeller2].vertData[i].position.y > box2y1)
				box2y1 = meshes[propeller2].vertData[i].position.y;
		}
		//front gun 1 axis
		double max = -1e10, maxz = -1e10, minz = 1e10; min = 1e10;
		for (int i = 0; i < meshes[front_gun1].vertData.size(); ++i)
		{
			if (meshes[front_gun1].vertData[i].position.x < min)
				min = meshes[front_gun1].vertData[i].position.x;
			if (meshes[front_gun1].vertData[i].position.x > max)
				max = meshes[front_gun1].vertData[i].position.x;
			if (meshes[front_gun1].vertData[i].position.z < minz)
				minz = meshes[front_gun1].vertData[i].position.z;
			if (meshes[front_gun1].vertData[i].position.z > maxz)
				maxz = meshes[front_gun1].vertData[i].position.z;
		}
		x3 = (max - min) * 0.5 + min;
		z3 = (maxz - minz) / 7.0 * 2.0 + minz;
		//front gun 2 axis
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10;
		for (int i = 0; i < meshes[front_gun2].vertData.size(); ++i)
		{
			if (meshes[front_gun2].vertData[i].position.x < min)
				min = meshes[front_gun1].vertData[i].position.x;
			if (meshes[front_gun2].vertData[i].position.x > max)
				max = meshes[front_gun2].vertData[i].position.x;
			if (meshes[front_gun2].vertData[i].position.z < minz)
				minz = meshes[front_gun2].vertData[i].position.z;
			if (meshes[front_gun2].vertData[i].position.z > maxz)
				maxz = meshes[front_gun2].vertData[i].position.z;
		}
		x4 = (max - min) * 0.5 + min;
		z4 = (maxz - minz) / 7.0 * 2.0 + minz;
		//back gun 1 axis
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10;
		for (int i = 0; i < meshes[back_gun1].vertData.size(); ++i)
		{
			if (meshes[back_gun1].vertData[i].position.x < min)
				min = meshes[back_gun1].vertData[i].position.x;
			if (meshes[back_gun1].vertData[i].position.x > max)
				max = meshes[back_gun1].vertData[i].position.x;
			if (meshes[back_gun1].vertData[i].position.z < minz)
				minz = meshes[back_gun1].vertData[i].position.z;
			if (meshes[back_gun1].vertData[i].position.z > maxz)
				maxz = meshes[back_gun1].vertData[i].position.z;
		}
		x5 = (max - min) * 0.5 + min;
		z5 = (maxz - minz) / 7.0 * 5.0 + minz;
		//back gun 2 axis
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10;
		for (int i = 0; i < meshes[back_gun2].vertData.size(); ++i)
		{
			if (meshes[back_gun2].vertData[i].position.x < min)
				min = meshes[back_gun2].vertData[i].position.x;
			if (meshes[back_gun2].vertData[i].position.x > max)
				max = meshes[back_gun2].vertData[i].position.x;
			if (meshes[back_gun2].vertData[i].position.z < minz)
				minz = meshes[back_gun2].vertData[i].position.z;
			if (meshes[back_gun2].vertData[i].position.z > maxz)
				maxz = meshes[back_gun2].vertData[i].position.z;
		}
		x6 = (max - min) * 0.5 + min;
		z6 = (maxz - minz) / 7.0 * 5.0 + minz;
		//chimney position
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10;
		double maxy = -1e10, miny = 1e10;
		for (int i = 0; i < meshes[chimney].vertData.size(); ++i)
		{
			if (meshes[chimney].vertData[i].position.x < min)
				min = meshes[chimney].vertData[i].position.x;
			if (meshes[chimney].vertData[i].position.x > max)
				max = meshes[chimney].vertData[i].position.x;
			if (meshes[chimney].vertData[i].position.z < minz)
				minz = meshes[chimney].vertData[i].position.z;
			if (meshes[chimney].vertData[i].position.z > maxz)
				maxz = meshes[chimney].vertData[i].position.z;
			if (meshes[chimney].vertData[i].position.y < miny)
				miny = meshes[chimney].vertData[i].position.y;
			if (meshes[chimney].vertData[i].position.y > maxy)
				maxy = meshes[chimney].vertData[i].position.y;
		}
		x7 = (max - min) * 0.5 + min;
		z7 = (maxz - minz) * 0.25 + minz;
		y7 = (maxy - miny) * 4.0 / 7.0 + miny;
		//chimney2 position
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10; maxy = -1e10; miny = 1e10;
		for (int i = 0; i < meshes[chimney2].vertData.size(); ++i)
		{
			if (meshes[chimney2].vertData[i].position.x < min)
				min = meshes[chimney2].vertData[i].position.x;
			if (meshes[chimney2].vertData[i].position.x > max)
				max = meshes[chimney2].vertData[i].position.x;
			if (meshes[chimney2].vertData[i].position.z < minz)
				minz = meshes[chimney2].vertData[i].position.z;
			if (meshes[chimney2].vertData[i].position.z > maxz)
				maxz = meshes[chimney2].vertData[i].position.z;
			if (meshes[chimney2].vertData[i].position.y < miny)
				miny = meshes[chimney2].vertData[i].position.y;
			if (meshes[chimney2].vertData[i].position.y > maxy)
				maxy = meshes[chimney2].vertData[i].position.y;
		}
		x8 = (max - min) * 0.5 + min;
		z8 = (maxz - minz) * 16.0 / 24.0 + minz;
		y8 = (maxy - miny) * 29.0 / 47.0 + miny;
		//position of front end of boat
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10; maxy = -1e10; miny = 1e10;
		for (int i = 0; i < meshes[front].vertData.size(); ++i)
		{
			if (meshes[front].vertData[i].position.x < min)
				min = meshes[front].vertData[i].position.x;
			if (meshes[front].vertData[i].position.x > max)
				max = meshes[front].vertData[i].position.x;
			if (meshes[front].vertData[i].position.z < minz)
				minz = meshes[front].vertData[i].position.z;
			if (meshes[front].vertData[i].position.z > maxz)
				maxz = meshes[front].vertData[i].position.z;
			if (meshes[front].vertData[i].position.y < miny)
				miny = meshes[front].vertData[i].position.y;
			if (meshes[front].vertData[i].position.y > maxy)
				maxy = meshes[front].vertData[i].position.y;
		}
		x9 = (max - min) * 0.5 + min;
		z9 = -(maxz - minz) * 0.05 + maxz;
		y9 = (maxy - miny) * 0.2 + miny;
		//position of middle part of boat
		max = -1e10; maxz = -1e10; minz = 1e10; min = 1e10; maxy = -1e10; miny = 1e10;
		for (int i = 0; i < meshes[middle].vertData.size(); ++i)
		{
			if (meshes[middle].vertData[i].position.x < min)
				min = meshes[middle].vertData[i].position.x;
			if (meshes[middle].vertData[i].position.x > max)
				max = meshes[middle].vertData[i].position.x;
			if (meshes[middle].vertData[i].position.z < minz)
				minz = meshes[middle].vertData[i].position.z;
			if (meshes[middle].vertData[i].position.z > maxz)
				maxz = meshes[middle].vertData[i].position.z;
			if (meshes[middle].vertData[i].position.y < miny)
				miny = meshes[middle].vertData[i].position.y;
			if (meshes[middle].vertData[i].position.y > maxy)
				maxy = meshes[middle].vertData[i].position.y;
		}
		x10_0 = min;
		x10_1 = max;
		z10 = maxz;
		y10 = y9;
		srand(time(0));
		return true;
	}
	~Model()
	{
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			it->final();
		}
	}
	void animation(GLfloat delta)
	{
		float thita = 2 * 2 * acos(-1) * delta;
		float co = cos(thita);
		float si = sin(thita);
		//propeller1 animation
		matrix4 move1(1, 0, 0, -x1, 0, 1, 0, -y1, 0, 0, 1, 0, 0, 0, 0, 1);
		matrix4 move2(1, 0, 0, x1, 0, 1, 0, y1, 0, 0, 1, 0, 0, 0, 0, 1);
		matrix4 rotate(co, -si, 0, 0, si, co, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		matrix4 last = move2 * rotate * move1;
		for (int i = 0; i < meshes[propeller1].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[propeller1].vertData[i].position.x, 0, 0, 0,
				meshes[propeller1].vertData[i].position.y, 0, 0, 0,
				meshes[propeller1].vertData[i].position.z, 0, 0, 0,
				1, 0, 0, 0);
			tmp = last * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[propeller1].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[propeller1].final();
		meshes[propeller1].VAOId = 0;
		meshes[propeller1].VBOId = 0;
		meshes[propeller1].EBOId = 0;
		meshes[propeller1].setupMesh();
		//propeller2 animation
		move1 = matrix4(1, 0, 0, -x2, 0, 1, 0, -y2, 0, 0, 1, 0, 0, 0, 0, 1);
		move2 = matrix4(1, 0, 0, x2, 0, 1, 0, y2, 0, 0, 1, 0, 0, 0, 0, 1);
		rotate = matrix4(co, -si, 0, 0, si, co, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		last = move2 * rotate * move1;
		for (int i = 0; i < meshes[propeller2].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[propeller2].vertData[i].position[0], 0, 0, 0,
				meshes[propeller2].vertData[i].position[1], 0, 0, 0,
				meshes[propeller2].vertData[i].position[2], 0, 0, 0,
				1, 0, 0, 0);
			tmp = last * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[propeller2].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[propeller2].final();
		meshes[propeller2].VAOId = 0;
		meshes[propeller2].VBOId = 0;
		meshes[propeller2].EBOId = 0;
		meshes[propeller2].setupMesh();
		//front gun 1 animation
		thita = 0.05 * 2 * acos(-1) * delta;
		co = cos(thita);
		si = sin(thita);
		move1 = matrix4(1, 0, 0, -x3, 0, 1, 0, 0, 0, 0, 1, -z3, 0, 0, 0, 1);
		move2 = matrix4(1, 0, 0, x3, 0, 1, 0, 0, 0, 0, 1, z3, 0, 0, 0, 1);
		rotate = matrix4(co, 0, si, 0, 0, 1, 0, 0, -si, 0, co, 0, 0, 0, 0, 1);
		last = move2 * rotate * move1;
		for (int i = 0; i < meshes[front_gun1].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[front_gun1].vertData[i].position.x, 0, 0, 0,
				meshes[front_gun1].vertData[i].position.y, 0, 0, 0,
				meshes[front_gun1].vertData[i].position.z, 0, 0, 0,
				1, 0, 0, 0);
			tmp = last * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[front_gun1].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[front_gun1].final();
		meshes[front_gun1].VAOId = 0;
		meshes[front_gun1].VBOId = 0;
		meshes[front_gun1].EBOId = 0;
		meshes[front_gun1].setupMesh();
		//front gun 2 animation
		move1 = matrix4(1, 0, 0, -x4, 0, 1, 0, 0, 0, 0, 1, -z4, 0, 0, 0, 1);
		move2 = matrix4(1, 0, 0, x4, 0, 1, 0, 0, 0, 0, 1, z4, 0, 0, 0, 1);
		rotate = matrix4(co, 0, si, 0, 0, 1, 0, 0, -si, 0, co, 0, 0, 0, 0, 1);
		last = move2 * rotate * move1;
		for (int i = 0; i < meshes[front_gun2].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[front_gun2].vertData[i].position.x, 0, 0, 0,
				meshes[front_gun2].vertData[i].position.y, 0, 0, 0,
				meshes[front_gun2].vertData[i].position.z, 0, 0, 0,
				1, 0, 0, 0);
			tmp = last * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[front_gun2].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[front_gun2].final();
		meshes[front_gun2].VAOId = 0;
		meshes[front_gun2].VBOId = 0;
		meshes[front_gun2].EBOId = 0;
		meshes[front_gun2].setupMesh();
		//back gun 1 animation
		move1 = matrix4(1, 0, 0, -x5, 0, 1, 0, 0, 0, 0, 1, -z5, 0, 0, 0, 1);
		move2 = matrix4(1, 0, 0, x5, 0, 1, 0, 0, 0, 0, 1, z5, 0, 0, 0, 1);
		rotate = matrix4(co, 0, si, 0, 0, 1, 0, 0, -si, 0, co, 0, 0, 0, 0, 1);
		last = move2 * rotate * move1;
		for (int i = 0; i < meshes[back_gun1].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[back_gun1].vertData[i].position.x, 0, 0, 0,
				meshes[back_gun1].vertData[i].position.y, 0, 0, 0,
				meshes[back_gun1].vertData[i].position.z, 0, 0, 0,
				1, 0, 0, 0);
			tmp = last * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[back_gun1].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[back_gun1].final();
		meshes[back_gun1].VAOId = 0;
		meshes[back_gun1].VBOId = 0;
		meshes[back_gun1].EBOId = 0;
		meshes[back_gun1].setupMesh();
		//back gun 2 animation
		move1 = matrix4(1, 0, 0, -x6, 0, 1, 0, 0, 0, 0, 1, -z6, 0, 0, 0, 1);
		move2 = matrix4(1, 0, 0, x6, 0, 1, 0, 0, 0, 0, 1, z6, 0, 0, 0, 1);
		rotate = matrix4(co, 0, si, 0, 0, 1, 0, 0, -si, 0, co, 0, 0, 0, 0, 1);
		last = move2 * rotate * move1;
		for (int i = 0; i < meshes[back_gun2].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[back_gun2].vertData[i].position.x, 0, 0, 0,
				meshes[back_gun2].vertData[i].position.y, 0, 0, 0,
				meshes[back_gun2].vertData[i].position.z, 0, 0, 0,
				1, 0, 0, 0);
			tmp = last * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[back_gun2].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[back_gun2].final();
		meshes[back_gun2].VAOId = 0;
		meshes[back_gun2].VBOId = 0;
		meshes[back_gun2].EBOId = 0;
		meshes[back_gun2].setupMesh();
	}
private:
	/*
	* 递归处理模型的结点
	*/
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr)
	{
		if (!node || !sceneObjPtr)
		{
			return false;
		}
		std::cout << node->mName.data << std::endl;
		int flag = 0;
		if (node->mName == aiString("g Propeller_meshShape_0419"))
			flag = 1;
		if (node->mName == aiString("g Propeller_meshShape_0423"))
			flag = 2;
		if (node->mName == aiString("g gunShape_0001"))
			flag = 3;
		if (node->mName == aiString("g gunShape_0109"))
			flag = 4;
		if (node->mName == aiString("g gunShape_0389"))
			flag = 5;
		if (node->mName == aiString("g gunShape_0391"))
			flag = 6;
		if (node->mName == aiString("g MidFrontShape"))
			flag = 7;
		if (node->mName == aiString("g MidBack_DeckHouseShape"))
			flag = 8;
		if (node->mName == aiString("g BowShape"))
			flag = 9;
		if (node->mName == aiString("g MidBackShape"))
			flag = 10;
		// 先处理自身结点
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			// 注意node中的mesh是对sceneObject中mesh的索引
			const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]]; 
			if (meshPtr)
			{
				Mesh meshObj;
				if (this->processMesh(meshPtr, sceneObjPtr, meshObj))
				{
					this->meshes.push_back(meshObj);
					if (flag == 1)
						propeller1 = this->meshes.size() - 1;
					if (flag == 2)
						propeller2 = this->meshes.size() - 1;
					if (flag == 3)
						front_gun1 = this->meshes.size() - 1;
					if (flag == 4)
						front_gun2 = this->meshes.size() - 1;
					if (flag == 5)
						back_gun1 = this->meshes.size() - 1;
					if (flag == 6)
						back_gun2 = this->meshes.size() - 1;
					if (flag == 7)
						chimney = this->meshes.size() - 1;
					if (flag == 8)
						chimney2 = this->meshes.size() - 1;
					if (flag == 9)
						front = this->meshes.size() - 1;
					if (flag == 10)
						middle = this->meshes.size() - 1;
				}
			}
		}
		// 处理孩子结点
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			this->processNode(node->mChildren[i], sceneObjPtr);
		}
		return true;
	}
	bool processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr, Mesh& meshObj)
	{
		if (!meshPtr || !sceneObjPtr)
		{
			return false;
		}
		std::vector<Vertex> vertData;
		std::vector<Texture> textures;
		std::vector<GLuint> indices;
		// 从Mesh得到顶点数据、法向量、纹理数据
		for (size_t i = 0; i < meshPtr->mNumVertices; ++i)
		{
			Vertex vertex;
			// 获取顶点位置
			if (meshPtr->HasPositions())
			{
				vertex.position.x = meshPtr->mVertices[i].x;
				vertex.position.y = meshPtr->mVertices[i].y;
				vertex.position.z = meshPtr->mVertices[i].z;
			}
			// 获取纹理数据 目前只处理0号纹理
			if (meshPtr->HasTextureCoords(0))
			{
				vertex.texCoords.x = meshPtr->mTextureCoords[0][i].x;
				vertex.texCoords.y = meshPtr->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			// 获取法向量数据
			if (meshPtr->HasNormals())
			{
				vertex.normal.x = meshPtr->mNormals[i].x;
				vertex.normal.y = meshPtr->mNormals[i].y;
				vertex.normal.z = meshPtr->mNormals[i].z;
			}
			vertData.push_back(vertex);
		}
		// 获取索引数据
		for (size_t i = 0; i < meshPtr->mNumFaces; ++i)
		{
			aiFace face = meshPtr->mFaces[i];
			if (face.mNumIndices != 3)
			{
				std::cerr << "Error:Model::processMesh, mesh not transformed to triangle mesh." << std::endl;
				return false;
			}
			for (size_t j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		// 获取纹理数据
		if (meshPtr->mMaterialIndex >= 0)
		{
			const aiMaterial* materialPtr = sceneObjPtr->mMaterials[meshPtr->mMaterialIndex];
			// 获取diffuse类型
			std::vector<Texture> diffuseTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_DIFFUSE, diffuseTexture);
			textures.insert(textures.end(), diffuseTexture.begin(), diffuseTexture.end());
			// 获取specular类型
			std::vector<Texture> specularTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_SPECULAR, specularTexture);
			textures.insert(textures.end(), specularTexture.begin(), specularTexture.end());
		}
		meshObj.setData(vertData, textures, indices);
		return true;
	}
	/*
	* 获取一个材质中的纹理
	*/
	bool processMaterial(const aiMaterial* matPtr, const aiScene* sceneObjPtr, 
		const aiTextureType textureType, std::vector<Texture>& textures)
	{
		textures.clear();

		if (!matPtr 
			|| !sceneObjPtr )
		{
			return false;
		}
		if (matPtr->GetTextureCount(textureType) <= 0)
		{
			return true;
		}
		for (size_t i = 0; i < matPtr->GetTextureCount(textureType); ++i)
		{
			Texture text;
			aiString textPath;
			aiReturn retStatus = matPtr->GetTexture(textureType, i, &textPath);
			if (retStatus != aiReturn_SUCCESS 
				|| textPath.length == 0)
			{
				std::cerr << "Warning, load texture type=" << textureType
					<< "index= " << i << " failed with return value= "
					<< retStatus << std::endl;
				continue;
			}
			std::string absolutePath = /*this->modelFileDir + "/" +*/ textPath.C_Str();
			LoadedTextMapType::const_iterator it = this->loadedTextureMap.find(absolutePath);
			if (it == this->loadedTextureMap.end()) // 检查是否已经加载过了
			{
				GLuint textId = TextureHelper::load2DTexture(absolutePath.c_str());
				text.id = textId;
				text.path = absolutePath;
				text.type = textureType;
				textures.push_back(text);
				loadedTextureMap[absolutePath] = text;
			}
			else
			{
				textures.push_back(it->second);
			}
		}
		return true;
	}
private:
	std::vector<Mesh> meshes; // 保存Mesh
	std::string modelFileDir; // 保存模型文件的文件夹路径
	typedef std::map<std::string, Texture> LoadedTextMapType; // key = texture file path
	LoadedTextMapType loadedTextureMap; // 保存已经加载的纹理
public:
	int propeller1, propeller2;
	double x1, y1, z1, x2, y2, z2;
	double box1x0, box1x1, box1y0, box1y1, box1z0, box1z1;
	double box2x0, box2x1, box2y0, box2y1, box2z0, box2z1;
	int front_gun1, front_gun2;
	double x3, y3, z3, x4, y4, z4;
	int back_gun1, back_gun2;
	double x5, y5, z5, x6, y6, z6;
	int chimney, chimney2;
	double x7, y7, z7, x8, y8, z8;
	int front;
	double x9, y9, z9;
	int middle;
	double x10_0, x10_1, y10, z10;
};

#endif