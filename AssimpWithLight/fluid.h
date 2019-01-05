#pragma once
#include <glm/glm.hpp>

#include <vector>
#include <math.h>

#include "Shader.h"
#include "texture.h"

// 表示一个顶点属性
struct VertexFluid {
	glm::vec3 position;
	glm::vec2 texCoords;
};

class Fluid {
public:

	// rowSize_: x网格点数, columnSize_: z网格点数, step: 网格点间距
	// refreshTime_: 刷新时间间隔
	// t: 公式系数-时间, c: 公式系数-波速, mu: 公式系数-阻力系数
	Fluid(int rowSize_, int columnSize_, float step_, GLfloat refreshTime_, float t, float c, float mu):
		rowSize(rowSize_), columnSize(columnSize_), step(step_), refreshTime(refreshTime_),
		display(true), currentRender(false), accumulateTime(0.0f)
	{
		// 计算迭代公式系数
		float f1 = c * c * t * t / (step * step);
		float f2 = 1.0f / (mu * t + 2);
		k1 = (4.0f - 8.0f * f1) * f2;
		k2 = (mu * t - 2) * f2;
		k3 = 2.0f * f1 * f2;

		// 加载纹理
		texture = TextureHelper::load2DTexture("water.bmp");
		
		int count = rowSize * columnSize;  // 网格点个数
		// 载入顶点数据
		vertData1.reserve(count);
		vertData2.reserve(count);
		float texStepX = 1.0f / rowSize;
		float textStepY = 1.0f / columnSize;
		for (int i = 0; i < columnSize; i++) {
			float z = step * i;
			for (int j = 0; j < rowSize; j++) {
				float x = step * j;
				float y = 0.0f;
				// 边界置0.0f 其余点随机初始化0.0f或1.0f
				if (i != 0 && j != 0 && i != columnSize - 1 && j != columnSize - 1)
					y = (0 == rand() % 2) ? 1.0f : 0.0f;
				float textCoorX = j * texStepX;
				float textCoorY = i * textStepY;

				VertexFluid v;
				v.position.x = x;
				v.position.y = y;
				v.position.z = z;
				v.texCoords.x = textCoorX;
				v.texCoords.y = textCoorY;
				vertData1.push_back(v);
				vertData2.push_back(v);
			}
		}
		// 载入三角形块索引信息
		indices.reserve(2 * ((rowSize - 1)*(columnSize - 1)));
		// 上三角
		for (int i = 0; i < rowSize - 1; i++) {
			for (int j = 0; j < columnSize - 1; j++) {
				int index = i * rowSize + j;
				indices.push_back(index);
				indices.push_back(index + 1);
				indices.push_back(index + rowSize);
			}
		}
		// 下三角
		for (int i = 1; i < rowSize; i++) {
			for (int j = 1; j < columnSize; j++) {
				int index = i * rowSize + j;
				indices.push_back(index);
				indices.push_back(index - 1);
				indices.push_back(index - rowSize);
			}
		}

		this->setupFluid();
	}
	~Fluid() {
		this->final();
	}

	// 获取指定坐标的水面高度
	float getHeight(float x, float z) {
		float transX = -rowSize * step / 2;
		float transZ = -columnSize * step / 2;
		x -= transX;
		z -= transZ;

		int x_min = floor(x / step);
		int x_max = ceil(x / step);
		x /= step;
		int z_min = floor(z / step);
		int z_max = ceil(z / step);
		z /= step;
		if (x_min < 0 || x_max >= rowSize || z_min < 0 || z_max >= columnSize) {
			return 0;
		}
		int index11 = z_min * rowSize + x_min;
		int index12 = z_max * rowSize + x_min;
		int index21 = z_min * rowSize + x_max;
		int index22 = z_max * rowSize + x_max;

		// 当前使用的点集数据
		std::vector<VertexFluid> &vd = currentRender ? vertData2 : vertData1;
		
		// 12(x_min, z_max), 22(x_max, z_max)
		// 11(x_min, z_min), 21(x_max, z_min)
		if (x_min == x_max) {
			return linear_mapping(z_min, z_max, vd[index11].position.y, vd[index12].position.y, z);
		}
		if (z_min == z_max) {
			return linear_mapping(x_min, x_max, vd[index11].position.y, vd[index21].position.y, x);
		}
		if (x == x_min) {
			return linear_mapping(z_min, z_max, vd[index11].position.y, vd[index12].position.y, z);
		}
		// 两种三角片元分别为右下/左上 因此分界线 k = 1
		auto p1 = vd[index11].position;
		auto p2 = vd[index22].position;
		auto p3 = ((z - z_min) / (x - x_min) > 1) ? vd[index12].position : vd[index21].position;
		// y = -(ax + cz + d)/b
		float a = (p2.y - p1.y)*(p3.z - p1.z) - (p2.z - p1.z)*(p3.y - p1.y);
		float b = (p2.z - p1.z)*(p3.x - p1.x) - (p2.x - p1.x)*(p3.z - p1.z);
		float c = (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
		float d = 0 - (a * p1.x + b * p1.y + c * p1.z);

		return -(a * x + c * z + d) / b;
	}

	// 切换显示状态
	void switchDisplay() {
		display = !display;
	}
	// 设置显示状态
	void setDisplay(bool display_) {
		display = display_;
	}

	// 设定水面刷新范围  范围参数为数组下标写法  闭区间
	bool setRefreshRange(int startX, int endX, int startZ, int endZ) {
		startX = startX < 1 ? 1 : startX;
		endX = endX > rowSize - 1 ? rowSize - 1 : endX;
		startZ = startZ < 1 ? 1 : startZ;
		endZ = endZ > columnSize - 1 ? columnSize - 1 : endZ;
		if (startX >= rowSize || endX < 0
			|| startZ >= columnSize || endZ < 0) {
			return false;
		}
		refreshRangeStartColumn = startX;
		refreshRangeEndColumn = endX;
		refreshRangeStartRow = startZ;
		refreshRangeEndRow = endZ;
		return true;
	}

	void animation(GLfloat deltaTime) {
		accumulateTime += deltaTime;
		if (accumulateTime > refreshTime) {
			this->refresh();
			accumulateTime = 0;
		}
	}
	// 强制刷新形状
	void refresh(void) {
		// 调用流体表面方程
		currentRender = !currentRender;
		std::vector<VertexFluid> &vd1 = currentRender ? vertData1 : vertData2;
		std::vector<VertexFluid> &vd2 = currentRender ? vertData2 : vertData1;
		for (int i = refreshRangeStartColumn; i <= refreshRangeEndColumn; i++) {
			for (int j = refreshRangeStartRow; j <= refreshRangeEndRow; j++) {
				int index = i * rowSize + j;
				float temp = vertData1[index].position.y;
				vd2[index].position.y =
					k1 * vd1[index].position.y +
					k2 * vd2[index].position.y +
					k3 * (vd1[index + 1].position.y + vd1[index - 1].position.y +
						vd1[index + rowSize].position.y + vd1[index - rowSize].position.y);
			}
		}
		//for (int i = 1; i < columnSize - 1; i++) {
		//	for (int j = 1; j < rowSize - 1; j++) {
		//		int index = i * rowSize + j;
		//		float temp = vertData1[index].position.y;
		//		vd2[index].position.y =
		//			k1 * vd1[index].position.y +
		//			k2 * vd2[index].position.y +
		//			k3 * (vd1[index + 1].position.y + vd1[index - 1].position.y +
		//				vd1[index + rowSize].position.y + vd1[index - rowSize].position.y);
		//	}
		//}
		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFluid) * vd2.size(),
			&vd2[0], GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// 绘制
	void draw(const Shader& shader) const {
		if (display) {
			shader.use();
			glBindVertexArray(this->VAOId);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(shader.programId, "textureImage"), 0);

			glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glUseProgram(0);
		}
	}

private:
	// 建立VAO, VBO, EBO等缓冲区
	void setupFluid(void) {
		glGenVertexArrays(1, &this->VAOId);
		glGenBuffers(1, &this->VBOId);
		glGenBuffers(1, &this->EBOId);

		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFluid) * this->vertData1.size(),
			&this->vertData1[0], GL_STREAM_DRAW);
		// 顶点位置属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(VertexFluid), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// 顶点纹理坐标
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(VertexFluid), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		// 索引数据
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->indices.size(),
			&this->indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void final(void) const {
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
		glDeleteBuffers(1, &this->EBOId);
	}

	/**
	 * @brief 线性映射 给定 a->x, b->y, 求目标原象点c对应象点
	 * 		  [(c-b)x - (c-a)y]/(a-b)
	 *		  若 a == b 则固定返回 x
	 * @param original_a 原象点a
	 * @param original_b 原象点b
	 * @param aim_x 象点x
	 * @param aim_y 象点y
	 * @param original 目标原象点
	 * @return 目标象点
	 */
	float linear_mapping(float original_a, float original_b,
			float aim_x, float aim_y, float original) {
		if (original_a - original_b < 0.000001) {
			return aim_x;
		}
		return (
			((original - original_b) * aim_x
				- (original - original_a) * aim_y)
			/ (original_a - original_b));
	}

private:
	int rowSize;    // x网格数
	int columnSize;    // z网格数
	int step;	// w网格边长
	int refreshRangeStartColumn;
	int refreshRangeEndColumn;
	int refreshRangeStartRow;
	int refreshRangeEndRow;

	// 因迭代公式需要记录上一次点的位置 因此使用两组顶点数据交替赋值
	bool currentRender;    // 记录当前使用的顶点缓冲
	std::vector<VertexFluid> vertData1;    // 顶点缓冲1
	std::vector<VertexFluid> vertData2;    // 顶点缓冲2
	
	std::vector<GLuint> indices;    // 索引

	float k1, k2, k3;    // 多项式系数

	bool display;	// 控制是否显示
	GLfloat refreshTime;
	GLfloat accumulateTime;

	GLuint VAOId, VBOId, EBOId;
	GLuint texture;    // 纹理
};