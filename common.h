#ifndef I_COMMON_H
#define I_COMMON_H

#include "math.h"
#include "stdlib.h"
/*定义像素*/
typedef unsigned int Pixel;
/*四舍五入*/
inline float Rand( float a_Range ) { return ((float)rand() / RAND_MAX) * a_Range; }

namespace Raytracer {
/*定义向量点乘 点乘最后算出来的是一个数*/
#define DOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
/*向量规格化 */
#define NORMALIZE(A)	{float l=1/sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
/*向量长度定义*/
#define LENGTH(A)		(sqrtf(A.x*A.x+A.y*A.y+A.z*A.z))
/*向量长度的平方*/
#define SQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
/*两个向量之间的距离*/
#define SQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))
/*这个是干嘛用的*/
#define EPSILON			0.0001f					/*定义微元*/
#define TRACEDEPTH		10						/*定义跟踪次数*/

#define PI				3.141592653589793238462f

class vector3
{
public:
	/*缺省构造函数*/
	vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {};
	/*用户构造函数*/
	vector3( float a_X, float a_Y, float a_Z ) : x( a_X ), y( a_Y ), z( a_Z ) {};
	/*定位向量*/
	void Set( float a_X, float a_Y, float a_Z ) { x = a_X; y = a_Y; z = a_Z; }
	/*向量规格化*/
	void Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; }
	/*向量长度*/
	float Length() { return (float)sqrt( x * x + y * y + z * z ); }
	/*向量长度的平方*/
	float SqrLength() { return x * x + y * y + z * z; }
	/*向量点积*/
	float Dot( vector3 a_V ) { return x * a_V.x + y * a_V.y + z * a_V.z; }
	/*向量叉乘 求两向量的法平面的单位向量*/
	vector3 Cross( vector3 b ) { return vector3( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x ); }
	/*重载操作符*/
	void operator += ( vector3& a_V ) { x += a_V.x; y += a_V.y; z += a_V.z; }
	void operator += ( vector3* a_V ) { x += a_V->x; y += a_V->y; z += a_V->z; }
	void operator -= ( vector3& a_V ) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
	void operator -= ( vector3* a_V ) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
	void operator *= ( float f ) { x *= f; y *= f; z *= f; }
	void operator *= ( vector3& a_V ) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
	void operator *= ( vector3* a_V ) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
	vector3 operator- () const { return vector3( -x, -y, -z ); }
	friend vector3 operator + ( const vector3& v1, const vector3& v2 ) { return vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z ); }
	friend vector3 operator - ( const vector3& v1, const vector3& v2 ) { return vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z ); }
	friend vector3 operator + ( const vector3& v1, vector3* v2 ) { return vector3( v1.x + v2->x, v1.y + v2->y, v1.z + v2->z ); }
	friend vector3 operator - ( const vector3& v1, vector3* v2 ) { return vector3( v1.x - v2->x, v1.y - v2->y, v1.z - v2->z ); }
	friend vector3 operator * ( const vector3& v, float f ) { return vector3( v.x * f, v.y * f, v.z * f ); }
	friend vector3 operator * ( const vector3& v1, vector3& v2 ) { return vector3( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z ); }
	friend vector3 operator * ( float f, const vector3& v ) { return vector3( v.x * f, v.y * f, v.z * f ); }
	/*这个地方有什么用么*/
	union
	{
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float cell[3]; };
	};
};

class plane
{
public:
	plane() : N(0, 0, 0), D( 0 ) {};
	plane(vector3 a_Normal, float a_D ) : N( a_Normal ), D( a_D ) {};
	union
	{
		struct
		{
			vector3 N;
			float D;
		};
		float cell[4];
	};
};

typedef vector3 Color;

}; // namespace Raytracer

#endif