#ifndef I_RAYTRACER_H
#define I_RAYTRACER_H

#include "common.h"
namespace Raytracer {
/*定义光线类*/
class Ray
{
public:
	Ray() : m_Origin( vector3( 0, 0, 0 ) ), m_Direction( vector3( 0, 0, 0 ) ) 
	{

	};
	Ray(vector3& a_Origin, vector3& a_Dir );	/*用户构造函数定义，确定光线的位置和方向*/
	void SetOrigin( vector3& a_Origin )
	{ 
		m_Origin = a_Origin;					/*设置光线位置*/
	}
	void SetDirection(vector3& a_Direction)
	{
		m_Direction = a_Direction;				/*设置光线方向*/
	}
	vector3& GetOrigin() 
	{ 
		return m_Origin;						/*得到光线位置*/
	}
	vector3& GetDirection() 
	{
		NORMALIZE(m_Direction);                 /*得到光线方向*/
		return m_Direction; 
	}
private:
	vector3 m_Origin;
	vector3 m_Direction;
};
class Scene;
class Primitive;
class Engine
{
public:
	Engine();
	~Engine();
	void SetTarget(Pixel* a_Dest, int a_Width, int a_Height);
	Scene* GetScene() 
	{ 
		return m_Scene; 
	}
	Primitive* Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist );
	void InitRender();
	bool Render();
protected:
	// renderer data
	float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
	Scene* m_Scene;
	Pixel* m_Dest;
	int m_Width, m_Height, m_CurrLine, m_PPos;
	Primitive** m_LastRow;
};

}; 

#endif