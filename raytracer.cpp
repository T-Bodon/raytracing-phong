// -----------------------------------------------------------
// raytracer.cpp
// 2004 - Jacco Bikker - jacco@bik5.com - www.bik5.com -   <><
// -----------------------------------------------------------

#include "raytracer.h"
#include "scene.h"
#include "common.h"
#include "windows.h"
#include "winbase.h"

namespace Raytracer {

Ray::Ray(vector3& a_Origin, vector3& a_Dir): 
	m_Origin( a_Origin ), 
	m_Direction( a_Dir )
{
}

Engine::Engine()
{
	m_Scene = new Scene();
}

Engine::~Engine()
{
	delete m_Scene;
}
/*ΪʲôҪ�����Ⱥ͸߶�*/
void Engine::SetTarget( Pixel* a_Dest, int a_Width, int a_Height )
{
	// set pixel buffer address & size
	m_Dest = a_Dest;
	m_Width = a_Width;
	m_Height = a_Height;
}

/*������ߣ�������ߺ�ÿһ�������ཻ�㣬������һ��������ģ����Ǿ���������Ⱥ�˳��*/
Primitive* Engine::Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist )
{
	/*�����ȴ��ڶ���ĸ�����ȣ�����0*/
	if (a_Depth > TRACEDEPTH) return 0;
	// trace primary ray
	/*һ��ʼ�ľ����ʼ��Ϊ�ܴ� Ȼ�����ӳ�䵽�ҽ�*/
	a_Dist = 1000000.0f;
	vector3 pi;
	Primitive* prim = 0;
	int result;
	// �ҵ�����Ľ���㣬��ÿһ��ͼԪ�����б������ж��Ƿ�hit
	for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
	{
		Primitive* pr = m_Scene->GetPrimitive( s );
		int res;
		if (res = pr->Intersect( a_Ray, a_Dist )) 
		{
			prim = pr;
			result = res; // 0 = miss, 1 = hit, -1 = hit from inside primitive
		}
	}
	// һ�����嶼û�л��У���ֹ����
	if (!prim) return 0;
	// handle intersection
	if (prim->IsLight())
	{
		// ���hit��һ����Դ����ֹͣ����
		a_Acc = Color( 1, 1, 1 );
	}
	else
	{
		/*�����ཻ�����ɫ*/
		pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist;//�ҵ�����
															   /*ͨ���ѹ��ߵ���ʼλ��+���ߵķ���X���ߵľ�����Եõ�����*/
		// ���ٹ���
		/*�Գ����е�ͼԪ���б���*/
		for ( int l = 0; l < m_Scene->GetNrPrimitives(); l++ )
		{
			Primitive* p = m_Scene->GetPrimitive( l );/*��ȡָ����ͼԪ*/
			if (p->IsLight()) /*�����ͼԪ�ǹ�Դ*/
			{
				Primitive* light = p;
				// ������Դ
				float shade = 1.0f;
				if (light->GetType() == Primitive::SPHERE)
				{
					vector3 L = ((Sphere*)light)->GetCentre() - pi;
					float tdist = LENGTH( L );
					L *= (1.0f / tdist);/*������ߵķ���*/
					Ray r = Ray( pi + L * EPSILON, L );//���㵽��Դ������
					for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )//ȷ����Դ�뽻��֮���Ƿ����������塣
					{
						Primitive* pr = m_Scene->GetPrimitive( s );
						if ((pr != light) && (pr->Intersect( r, tdist )))//����������Դ֮������������ཻ  �����屻�ڵ���
						{
							shade = 0;
							break;
						}
					}
				}
				// calculate diffuse shading
				vector3 L = ((Sphere*)light)->GetCentre() - pi;/*��Դ���ĺ��ཻ��֮�������*/
				NORMALIZE( L );
				vector3 N = prim->GetNormal( pi );
				if (prim->GetMaterial()->GetDiffuse() > 0)/*��ȡ���ϵ�������ϵ��*/
				{
					float dot = DOT( L, N );/*ʵ��NL���߳�*/
					if (dot > 0)
					{
						float diff = dot * prim->GetMaterial()->GetDiffuse() * shade;/**/
						// add diffuse component to ray color
						a_Acc += diff * light->GetMaterial()->GetColor() * prim->GetMaterial()->GetColor();
					}
				}
				// determine specular component
				if (prim->GetMaterial()->GetSpecular() > 0)
				{
					// point light source: sample once for specular highlight
					vector3 V = a_Ray.GetDirection();
					vector3 R = L - 2.0f * DOT( L, N ) * N;
					float dot = DOT( V, R );
					if (dot > 0)
					{
						float spec = powf( dot, 20 ) * prim->GetMaterial()->GetSpecular() * shade;
						// add specular component to ray color
						a_Acc += spec * light->GetMaterial()->GetColor();
					}
				}
			}
		}
		// calculate reflection
		float refl = prim->GetMaterial()->GetReflection();
		if (refl > 0.0f)
		{
			vector3 N = prim->GetNormal( pi );
			vector3 R = a_Ray.GetDirection() - 2.0f * DOT( a_Ray.GetDirection(), N ) * N;
			if (a_Depth < TRACEDEPTH) 
			{
				Color rcol( 0, 0, 0 );
				float dist;
				Raytrace( Ray( pi + R * EPSILON, R ), rcol, a_Depth + 1, a_RIndex, dist );
				a_Acc += refl * rcol * prim->GetMaterial()->GetColor();
			}
		}
	}
	// return pointer to primitive hit by primary ray
	return prim;
}

// -----------------------------------------------------------
// Engine::InitRender
// Initializes the renderer, by resetting the line / tile
// counters and precalculating some values
// -----------------------------------------------------------
void Engine::InitRender()
{
	// set firts line to draw to
	m_CurrLine = 20;
	// set pixel buffer address of first pixel
	m_PPos = 20 * m_Width;
	// screen plane in world space coordinates ��Ļ��Ӧ����������ϵ�е�λ��
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calculate deltas for interpolation �����ֵ΢Ԫ
	m_DX = (m_WX2 - m_WX1) / m_Width;
	m_DY = (m_WY2 - m_WY1) / m_Height;
	m_SY += 20 * m_DY;
	// allocate space to store pointers to primitives for previous line
	/*����洢����ָ��Ŀռ�*/
	m_LastRow = new Primitive*[m_Width];
	memset( m_LastRow, 0, m_Width * 4 );
}
/*Render���������εķ�����߽���ɨ�裬һ�η���һ������*/
bool Engine::Render()
{
	// render scene
	vector3 o(0, 0, -5);/*��������ڵ�λ��*/
	// initialize timer �Ӳ���ϵͳ�������صĺ�����
	int msecs = GetTickCount();
	// reset last found primitive pointer ���һ�����ֵ�����
	Primitive* lastprim = 0;
	// render remaining lines
	for (int y = m_CurrLine;y < (m_Height - 20); y++)
	{
		m_SX = m_WX1;
		// render pixels for current line
		for ( int x = 0; x < m_Width; x++ )
		{
			// fire primary ray
			Color acc( 0, 0, 0 );
			/*�����o��dir������*/
			vector3 dir = vector3( m_SX, m_SY, 0 ) - o;
			NORMALIZE( dir );
			Ray r( o, dir );
			float dist;
			Primitive* prim = Raytrace( r, acc, 6, 1.0f, dist );
			int red = (int)(acc.r * 256);
			int green = (int)(acc.g * 256);
			int blue = (int)(acc.b * 256);
			if (red > 255) red = 255;
			if (green > 255) green = 255;
			if (blue > 255) blue = 255;
			m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
			m_SX += m_DX;
		}
		m_SY += m_DY;
		if ((GetTickCount() - msecs) > 100) 
		{
			// return control to windows so the screen gets updated
			m_CurrLine = y + 1;
			return false;
		}
	}
	// all done
	return true;
}

}; // namespace Raytracer