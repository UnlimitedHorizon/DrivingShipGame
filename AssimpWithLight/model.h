#ifndef _MODEL_H_
#define _MODEL_H_

#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <math.h>
#include "mesh.h"
#include "texture.h"
#include "matrix.h"

/*
* 代表一个模型 模型可以包含一个或多个Mesh
*/
class Model
{
public:
	void draw(const Shader& shader) const
	{
		int tmp = -1;
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			tmp++;
			/*if (tmp != propeller1 && tmp != propeller2)
				continue;*/
			it->draw(shader);
		}
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
		double max = -1e10;
		for (int i = 0; i < meshes[propeller1].vertData.size(); ++i)
		{
			std::cout << meshes[propeller1].vertData[i].position[0] << " " <<
				meshes[propeller1].vertData[i].position[1] << " " <<
				meshes[propeller1].vertData[i].position[2] << " " << std::endl;
		}
		for (int i = 0; i < meshes[propeller1].vertData.size(); ++i)
			if (meshes[propeller1].vertData[i].position[1] > max)
			{
				max = meshes[propeller1].vertData[i].position[1];
				x1 = meshes[propeller1].vertData[i].position[0];
				z1 = meshes[propeller1].vertData[i].position[2];
			}
		max = -1e10;
		for (int i = 0; i < meshes[propeller2].vertData.size(); ++i)
			if (meshes[propeller2].vertData[i].position[1] > max)
			{
				max = meshes[propeller2].vertData[i].position[1];
				x2 = meshes[propeller2].vertData[i].position[0];
				z2 = meshes[propeller2].vertData[i].position[2];
			}
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
		//return;
		float thita = 2 * 2 * acos(-1) * delta;
		//float thita = 0;
		float co = cos(thita);
		float si = sin(thita);
		double min = 1e10;
		for (int i = 0; i < meshes[propeller1].vertData.size(); ++i)
			if (meshes[propeller1].vertData[i].position.z < min)
			{
				min = meshes[propeller1].vertData[i].position.z;
				x1 = meshes[propeller1].vertData[i].position.x;
				y1 = meshes[propeller1].vertData[i].position.y;
			}
		min = 1e10;
		for (int i = 0; i < meshes[propeller2].vertData.size(); ++i)
			if (meshes[propeller2].vertData[i].position.z < min)
			{
				min = meshes[propeller2].vertData[i].position.z;
				x2 = meshes[propeller2].vertData[i].position.x;
				y2 = meshes[propeller2].vertData[i].position.y;
			}
		for (int i = 0; i < meshes[propeller1].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[propeller1].vertData[i].position.x, 0, 0, 0,
				meshes[propeller1].vertData[i].position.y, 0, 0, 0,
				meshes[propeller1].vertData[i].position.z, 0, 0, 0,
				1, 0, 0, 0);
			//tmp.print();
			//matrix4 move1(1, 0, 0, -x1, 0, 1, 0, 0, 0, 0, 1, -z1, 0, 0, 0, 1);
			//matrix4 move2(1, 0, 0, x1, 0, 1, 0, 0, 0, 0, 1, z1, 0, 0, 0, 1);
			//matrix4 rotate(co, 0, si, 0, 0, 1, 0, 0, -si, 0, co, 0, 0, 0, 0, 1);
			matrix4 move1(1, 0, 0, -x1, 0, 1, 0, -y1, 0, 0, 1, 0, 0, 0, 0, 1);
			matrix4 move2(1, 0, 0, x1, 0, 1, 0, y1, 0, 0, 1, 0, 0, 0, 0, 1);
			matrix4 rotate(co, -si, 0, 0, si, co, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			tmp = move2 * rotate * move1 * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[propeller1].vertData[i].position[j] = tmp.a[j][0];
		}
		for (int i = 0; i < meshes[propeller2].vertData.size(); ++i)
		{
			matrix4 tmp(meshes[propeller2].vertData[i].position[0], 0, 0, 0,
				meshes[propeller2].vertData[i].position[1], 0, 0, 0,
				meshes[propeller2].vertData[i].position[2], 0, 0, 0,
				1, 0, 0, 0);
			//matrix4 move1(1, 0, 0, -x2, 0, 1, 0, 0, 0, 0, 1, -z2, 0, 0, 0, 1);
			//matrix4 move2(1, 0, 0, x2, 0, 1, 0, 0, 0, 0, 1, z2, 0, 0, 0, 1);
			//matrix4 rotate(co, 0, si, 0, 0, 1, 0, 0, -si, 0, co, 0, 0, 0, 0, 1);
			matrix4 move1(1, 0, 0, -x2, 0, 1, 0, -y2, 0, 0, 1, 0, 0, 0, 0, 1);
			matrix4 move2(1, 0, 0, x2, 0, 1, 0, y2, 0, 0, 1, 0, 0, 0, 0, 1);
			matrix4 rotate(co, -si, 0, 0, si, co, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			tmp = move2 * rotate * move1 * tmp;
			for (int j = 0; j < 3; ++j)
				meshes[propeller2].vertData[i].position[j] = tmp.a[j][0];
		}
		meshes[propeller1].final();
		meshes[propeller1].VAOId = 0;
		meshes[propeller1].VBOId = 0;
		meshes[propeller1].EBOId = 0;
		meshes[propeller1].setupMesh();
		meshes[propeller2].final();
		meshes[propeller2].VAOId = 0;
		meshes[propeller2].VBOId = 0;
		meshes[propeller2].EBOId = 0;
		meshes[propeller2].setupMesh();
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
	int propeller1, propeller2;
	double x1, y1, z1, x2, y2, z2;
};

#endif