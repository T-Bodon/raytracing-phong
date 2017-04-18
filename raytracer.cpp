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
/*为什么要定义宽度和高度*/
void Engine::SetTarget( Pixel* a_Dest, int a_Width, int a_Height )
{
	// set pixel buffer address & size
	m_Dest = a_Dest;
	m_Width = a_Width;
	m_Height = a_Height;
}

/*发射光线，计算光线和每一个物体相交点，决定哪一个是最近的，就是决定物体的先后顺序*/
Primitive* Engine::Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist )
{
	/*如果深度大于定义的跟踪深度，返回0*/
	if (a_Depth > TRACEDEPTH) return 0;
	// trace primary ray
	/*一开始的距离初始化为很大 然后把他映射到灰阶*/
	a_Dist = 1000000.0f;
	vector3 pi;
	Primitive* prim = 0;
	int result;
	// 找到最近的交叉点，对每一个图元，进行遍历，判断是否hit
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
	// 一个物体都没有击中，终止光线
	if (!prim) return 0;
	// handle intersection
	if (prim->IsLight())
	{
		// 如果hit到一个光源，则停止传输
		a_Acc = Color( 1, 1, 1 );
	}
	else
	{
		/*计算相交点的颜色*/
		pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist;//找到交点
															   /*通过把光线的起始位置+光线的方向X光线的距离可以得到交点*/
		// 跟踪光线
		/*对场景中的图元进行遍历*/
		for ( int l = 0; l < m_Scene->GetNrPrimitives(); l++ )
		{
			Primitive* p = m_Scene->GetPrimitive( l );/*获取指定的图元*/
			if (p->IsLight()) /*如果该图元是光源*/
			{
				Primitive* light = p;
				// 处理点光源
				float shade = 1.0f;
				if (light->GetType() == Primitive::SPHERE)
				{
					vector3 L = ((Sphere*)light)->GetCentre() - pi;
					float tdist = LENGTH( L );
					L *= (1.0f / tdist);/*定义光线的方向*/
					Ray r = Ray( pi + L * EPSILON, L );//交点到光源的射线
					for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )//确定光源与交点之间是否有其他物体。
					{
						Primitive* pr = m_Scene->GetPrimitive( s );
						if ((pr != light) && (pr->Intersect( r, tdist )))//如果射线与光源之外的其他物体相交  则物体被遮挡。
						{
							shade = 0;
							break;
						}
					}
				}
				// calculate diffuse shading
				vector3 L = ((Sphere*)light)->GetCentre() - pi;/*光源中心和相交点之间的向量*/
				NORMALIZE( L );
				vector3 N = prim->GetNormal( pi );
				if (prim->GetMaterial()->GetDiffuse() > 0)/*获取材料的漫反射系数*/
				{
					float dot = DOT( L, N );/*实现NL的线程*/
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
	// screen plane in world space coordinates 屏幕对应到世界坐标系中的位置
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calculate deltas for interpolation 计算插值微元
	m_DX = (m_WX2 - m_WX1) / m_Width;
	m_DY = (m_WY2 - m_WY1) / m_Height;
	m_SY += 20 * m_DY;
	// allocate space to store pointers to primitives for previous line
	/*分配存储物体指针的空间*/
	m_LastRow = new Primitive*[m_Width];
	memset( m_LastRow, 0, m_Width * 4 );
}
/*Render方法从左导游的发射光线进行扫描，一次发射一根光线*/
bool Engine::Render()
{
	// render scene
	vector3 o(0, 0, -5);/*照相机所在的位置*/
	// initialize timer 从操作系统启动返回的毫秒数
	int msecs = GetTickCount();
	// reset last found primitive pointer 最后一个发现的物体
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
			/*定义从o到dir的射线*/
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