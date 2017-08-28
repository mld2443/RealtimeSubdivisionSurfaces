////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "state.h"
#include "colorshaderclass.h"
#include "lightclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, float);
	void Shutdown();
	bool Frame(float, int, int, bool, bool);

	bool switchRenderer();

private:
	bool Render();

private:
	bool m_Solid;
	int m_oldMouseX, m_oldMouseY;
	float m_tessFactor;
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ColorShaderClass* m_ColorShader;
	LightClass* m_Light;
	state::vector* m_Mesh;
};

#endif