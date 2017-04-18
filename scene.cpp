
#include "common.h"
#include "string.h"
#include "scene.h"
#include "raytracer.h"

namespace Raytracer {

// -----------------------------------------------------------
// Primitive class implementation
// -----------------------------------------------------------

void Primitive::SetName( char* a_Name )
{
	delete m_Name; 
	m_Name = new char[strlen( a_Name ) + 1]; 
	strcpy( m_Name, a_Name ); 
}

// -----------------------------------------------------------
// Material class implementation
// -----------------------------------------------------------

Material::Material() :
	m_Color( Color( 0.2f, 0.2f, 0.2f ) ),
	m_Refl( 0 ), m_Diff( 0.2f )
{
}

// -----------------------------------------------------------
// Sphere primitive methods
// -----------------------------------------------------------

int Sphere::Intersect( Ray& a_Ray, float& a_Dist )
{
	vector3 v = a_Ray.GetOrigin() - m_Centre;
	float b = -DOT( v, a_Ray.GetDirection() );
	float det = (b * b) - DOT( v, v ) + m_SqRadius;
	int retval = MISS;
	if (det > 0)
	{
		det = sqrtf( det );
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0) 
			{
				if (i2 < a_Dist) 
				{
					a_Dist = i2;
					retval = INPRIM;
				}
			}
			else
			{
				if (i1 < a_Dist)
				{
					a_Dist = i1;
					retval = HIT;
				}
			}
		}
	}
	return retval;
}
int Triangle::Intersect(Ray& a_Ray, float& a_Dist){
	float l1 = t_point1.x * t_point2.y - t_point2.x * t_point1.y;
	float l2 = t_point2.x * t_point3.y - t_point3.x * t_point2.y;
	float m1 = t_point2.y * t_point1.z - t_point1.y * t_point2.z;
	float m2 = t_point3.y * t_point2.z - t_point2.y * t_point3.z;
	float n1 = t_point2.y - t_point1.y;
	float n2 = t_point3.y - t_point2.y;
	float d1 = (l2*m1 - l1*m2) / (m2*n1 - m1*n2);
	float c1 = (-d1*n1 - l1) / m1;
	float b1 = (t_point2.x + c1*t_point2.z + d1) / t_point2.y;
	vector3 o = a_Ray.GetOrigin();
	vector3 D = a_Ray.GetDirection();
	float t = (o.x + b1*o.y + c1*o.z + d1) / (D.x + b1*D.y + c1*D.z)*(-1.0);
	if (t > 0)
	{
		NORMALIZE(a_Ray.GetDirection());
		vector3 temp;
		temp.x = t * a_Ray.GetDirection().x;
		temp.y = t * a_Ray.GetDirection().y; 
		temp.z = t * a_Ray.GetDirection().z;
		vector3 inter = temp + a_Ray.GetOrigin();
		vector3 p21 = t_point2 - t_point1;
		vector3 p31 = t_point3 - t_point1;
		vector3 pp = inter - t_point1;
		double m = (pp.y * p31.x - pp.x) / (p21.y*p31.x - p21.x*p31.y);
		double n = (pp.x - m*p21.x) / p31.x;
		if (t < a_Dist&&m>0 && n>0 && (m + n)<1)
		{
			a_Dist = t;
			return HIT;
		}
	}
	return MISS;
}

// -----------------------------------------------------------
// Plane primitive class implementation
// -----------------------------------------------------------

int PlanePrim::Intersect( Ray& a_Ray, float& a_Dist )
{
	float d = DOT( m_Plane.N, a_Ray.GetDirection() );
	/*通过求法向量和光线的点积 若值=0，说明光线与法向量平行，即miss，else hit */
	if (d != 0)//若直线不与平面平行
	{
		float dist = -(DOT( m_Plane.N, a_Ray.GetOrigin() ) + m_Plane.D) / d;
		if (dist > 0)
		{
			if (dist < a_Dist) 
			{
				a_Dist = dist;
				return HIT;
			}
		}
	}
	return MISS;
}

vector3 PlanePrim::GetNormal( vector3& a_Pos )
{
	return m_Plane.N;
}

// -----------------------------------------------------------
// Scene class implementation
// -----------------------------------------------------------

Scene::~Scene()
{
	delete m_Primitive;
}
/*初始化场景，使得场景中包含一个平面，一个球，两个光源，一个三角形*/
void Scene::InitScene()
{
	m_Primitive = new Primitive*[100];

	// ground plane
	m_Primitive[0] = new PlanePrim(vector3(0, 1.5, 0), 9.4f);
	m_Primitive[0]->SetName( "plane" );
	m_Primitive[0]->GetMaterial()->SetReflection( 0 );
	m_Primitive[0]->GetMaterial()->SetDiffuse( 1.0f );
	m_Primitive[0]->GetMaterial()->SetColor( Color( 0.0f, 1.0f, 0.0f ) );
	
	// sphere1
	m_Primitive[1] = new Sphere( vector3( 0, -1.4f, 3 ), 1.5f );
	m_Primitive[1]->SetName( "sphere1" );
	m_Primitive[1]->GetMaterial()->SetReflection( 0.5f );
	m_Primitive[1]->GetMaterial()->SetColor( Color( 1.0f, 0.0f, 0.0f ) );
	// sphere2
	m_Primitive[2] = new Sphere(vector3(6, -2.8f, 9), 1.0f);
	m_Primitive[2]->SetName("sphere2");
	m_Primitive[2]->GetMaterial()->SetReflection(0.5f);
	m_Primitive[2]->GetMaterial()->SetColor(Color(0.0f, 0.0f, 0.0f));
	// sphere3
	m_Primitive[3] = new Sphere(vector3(-5.6, -2.3f, 5), 1.2f);
	m_Primitive[3]->SetName("sphere3");
	m_Primitive[3]->GetMaterial()->SetReflection(0.5f);
	m_Primitive[3]->GetMaterial()->SetColor(Color(0.0f, 0.0f, 1.0f));
	// light source 1
	m_Primitive[4] = new Sphere(vector3(0, 5, -5), 0.1f);
	m_Primitive[4]->Light(true);
	m_Primitive[4]->GetMaterial()->SetColor(Color(1.0f, 1.0f, 1.0f));
	/*
	// triangle
	m_Primitive[3] = new Triangle( vector3(-6,1, 5), vector3(10,10, 6), vector3(5, 3, 6));
	m_Primitive[3]->SetName("triangle");
	m_Primitive[3]->GetMaterial()->SetReflection(0.5f);
	m_Primitive[3]->GetMaterial()->SetColor(Color(0.0f, 1.0f, 0.0f));
	
	// light source 2
	m_Primitive[4] = new Sphere(vector3(0, 5, -5), 0.1f);
	m_Primitive[4]->Light(true);
	m_Primitive[4]->GetMaterial()->SetColor(Color(0.4f, 0.4f, 0.4f));
	
	m_Primitive[5] = new Sphere(vector3(4, -1.8f, 3), 1.5f);
	m_Primitive[5]->SetName("big sphere1");
	m_Primitive[5]->GetMaterial()->SetReflection(0.5f);
	m_Primitive[5]->GetMaterial()->SetColor(Color(1.0f, 0.4f, 0.0f));
	*/
	/*
	// light source 2
	m_Primitive[3] = new Sphere(vector3(0, 5, -5), 0.1f);
	m_Primitive[3]->Light(true);
	m_Primitive[3]->GetMaterial()->SetColor(Color(0.4f, 0.4f, 0.4f));
	*/
	 //set number of primitives
	m_Primitives = 5;
}

}; // namespace Raytracer
