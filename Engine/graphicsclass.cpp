////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_oldMouseX = 0;
	m_oldMouseY = 0;
	m_tessFactor = 0.0f;
	m_Solid = true;
	m_D3D = 0;
	m_Camera = 0;
	m_Mesh = 0;
	m_ColorShader = 0;
	m_Light = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, float tessFactor)
{
	bool result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(3.0f, 1.5f, -6.0f);
	m_Camera->SetRotation(17.0f, -12.0f, 0.0f);

	// Create the mesh object.
	m_Mesh = new state::vector;
	if(!m_Mesh)
	{
		return false;
	}

	// Initialize the mesh object.
	result = m_Mesh->Initialize(m_D3D->GetDevice(), {-0.5f,0.5f,0.0f}, {3.0f,2.0f,0.0f}, 16, 10, 0.005f, 0.003f, true);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the mesh object.", L"Error", MB_OK);
		return false;
	}
	state::gravity = v3(0.0f, -9.81f, 0.0f);
	state::wind = v3(0.0f, 0.0f, 0.0f);

	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -0.3f, 1.0f);

	// Set the tessellation factor
	m_tessFactor = tessFactor;

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the color shader object.
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if (m_Mesh)
	{
		m_Mesh->Shutdown();
		delete m_Mesh;
		m_Mesh = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(float frameTime, int mouseX, int mouseY, bool clicked, bool play)
{
	bool result;
	int deltaX, deltaY;

	// Calculate the mouse movement deltas
	deltaX = mouseX - m_oldMouseX;
	deltaY = mouseY - m_oldMouseY;
	m_oldMouseX = mouseX;
	m_oldMouseY = mouseY;

	// Rotate the scene when the mouse is held down
	if (clicked)
	{
		m_Camera->addRotation(((float)deltaY) / 180.0f, ((float)deltaX) / 180.0f, 0.0f);
	}

	// Run the frame processing for the particle system.
	if (play)
	{
		m_Mesh->Frame(frameTime, m_D3D->GetDeviceContext());
	}

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::switchRenderer() {
	bool result;

	m_Solid = !m_Solid;

	result = m_D3D->switchRaster(m_Solid);

	return result;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Put the mesh vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Mesh->Render(m_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Mesh->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_tessFactor);
	if(!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}