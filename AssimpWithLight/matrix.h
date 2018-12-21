#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
class matrix4 {
public:
	float a[4][4];
	matrix4(float a11 = 0, float a12 = 0, float a13 = 0, float a14 = 0,
		float a21 = 0, float a22 = 0, float a23 = 0, float a24 = 0,
		float a31 = 0, float a32 = 0, float a33 = 0, float a34 = 0,
		float a41 = 0, float a42 = 0, float a43 = 0, float a44 = 0)
	{
		a[0][0] = a11; a[0][1] = a12; a[0][2] = a13; a[0][3] = a14;
		a[1][0] = a21; a[1][1] = a22; a[1][2] = a23; a[1][3] = a24;
		a[2][0] = a31; a[2][1] = a32; a[2][2] = a33; a[2][3] = a34;
		a[3][0] = a41; a[3][1] = a42; a[3][2] = a43; a[3][3] = a44;
	}
	~matrix4() {}
	void print()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				std::cout << a[i][j] << " ";
			std::cout << std::endl;
		}
	}
};
matrix4 operator *(const matrix4 &a, const matrix4 &b)
{
	matrix4 c;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
				c.a[i][j] += a.a[i][k] * b.a[k][j];
	return c;
}
#endif