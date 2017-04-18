#ifndef I_SCENE_H
#define I_SCENE_H

#include "raytracer.h"

namespace Raytracer {

// Intersection method return values
#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive

class Material
{
public:
	Material();
	void SetColor( Color& a_Color ) 
	{ 
		m_Color = a_Color;						/*设置材料颜色*/
	}
	Color GetColor() 
	{ 
		return m_Color;							/*得到材料颜色*/
	}
	void SetDiffuse( float a_Diff ) 
	{ 
		m_Diff = a_Diff;						/*设置漫反射系数*/
	}
	void SetReflection( float a_Refl ) 
	{ 
		m_Refl = a_Refl;						/*设置反射系数*/
	}
	float GetSpecular() 
	{ 
		return 1.0f - m_Diff;					/*得到镜面反射系数*/
	}
	float GetDiffuse() 
	{
		return m_Diff;							/*得到漫反射系数*/
	}
	float GetReflection() 
	{ 
		return m_Refl;							/*得到反射系数*/
	}
private:
	Color m_Color;
	float m_Refl;						
	float m_Diff;
};
class Primitive
{
public:
	enum
	{
		SPHERE = 1,
		TRIANGLE = 2,
		PLANE
	};
	Primitive() : m_Name(0), m_Light(false) 
	{};
	Material* GetMaterial() 
	{ 
		return &m_Material;							/*得到材质*/
	}
	void SetMaterial( Material& a_Mat ) 
	{	
		m_Material = a_Mat;							/*设置材质*/
	}
	virtual int GetType() = 0;                      /*得到种类 默认是球*/
	virtual int Intersect(Ray& a_Ray, float& a_Dist) = 0;
	virtual vector3 GetNormal(vector3& a_Pos) = 0;
	virtual Color GetColor( vector3& ) 
	{ 
		return m_Material.GetColor();               /*得到材料的颜色*/
	}
	virtual void Light(bool a_Light) 
	{ 
		m_Light = a_Light;							/*设置光照*/
	}
	bool IsLight() 
	{ 
		return m_Light;                             /*判断光照是否存在*/
	}
	void SetName(char* a_Name);
	char* GetName() 
	{ 
		return m_Name;                            
	}
protected:
	/*三个成员变量 材质 名字 光照*/
	Material m_Material;
	char* m_Name;
	bool m_Light;
};

class Sphere : public Primitive
{
public:
	int GetType() { return SPHERE; }
	/*为啥这个地方要做成这个样子*/
	Sphere(vector3& a_Centre, float a_Radius) : 
		m_Centre(a_Centre), m_SqRadius(a_Radius * a_Radius), 
		m_Radius(a_Radius), m_RRadius(1.0f / a_Radius) {};

	vector3& GetCentre() 
	{ 
		return m_Centre;
	}
	float GetSqRadius() 
	{ 
		return m_SqRadius; 
	}
	int Intersect(Ray& a_Ray, float& a_Dist);//判断在这个距离之内有没有相交
	vector3 GetNormal(vector3& a_Pos) { return (a_Pos - m_Centre) * m_RRadius; }
private:
	vector3 m_Centre;
	float m_SqRadius, m_Radius, m_RRadius;
};
class Triangle : public Primitive
{
public:
	int GetType() 
	{
		return TRIANGLE; 
	}
	Triangle(vector3& point1, vector3& point2, vector3& point3) :
		t_point1(point1), t_point2(point2), t_point3(point3) {};
	int Intersect(Ray& a_Ray, float& a_Dist);
	vector3 GetNormal(vector3& a_Pos) { 
		vector3 normal;
		normal.x = 1.0f;
		float fenzi1 = (t_point3.z - t_point1.z)*(t_point2.x - t_point1.x) - (t_point2.z - t_point1.z)*(t_point3.x - t_point1.x);
		float fenmu1 = (t_point2.z - t_point1.z)*(t_point3.y - t_point1.y) - (t_point3.z - t_point1.z)*(t_point2.y - t_point1.x);
		normal.y = fenzi1 / fenmu1;
		float fenzi2 = (t_point2.x - t_point1.x) + normal.y*(t_point2.y - t_point1.y);
		float fenmu2 = t_point1.z - t_point2.z;
		normal.z = fenzi2 / fenmu2;
		NORMALIZE(normal);
		return normal; }
private:
	vector3 t_point1, t_point2, t_point3;
	//float m_SqRadius, m_Radius, m_RRadius;
};

// -----------------------------------------------------------
// PlanePrim primitive class definition
// -----------------------------------------------------------

class PlanePrim : public Primitive
{
public:
	int GetType() { return PLANE; }
	PlanePrim(vector3& a_Normal, float a_D) : m_Plane(plane(a_Normal, a_D)) {};
	vector3& GetNormal() { return m_Plane.N; }
	float GetD() { return m_Plane.D; }
	int Intersect( Ray& a_Ray, float& a_Dist);
	vector3 GetNormal(vector3& a_Pos);
private:
	plane m_Plane;
};
class Scene
{
public:
	Scene():m_Primitives(0), m_Primitive(0) {};
	~Scene();
	void InitScene();
	int GetNrPrimitives() { return m_Primitives; }//获取物体的数量
	Primitive* GetPrimitive(int a_Idx) { return m_Primitive[a_Idx]; }//获取指定索引处的物体的数量
private:
	int m_Primitives;//物体的数量
	Primitive** m_Primitive;//物体数组
};

}; // namespace Raytracer

#endif