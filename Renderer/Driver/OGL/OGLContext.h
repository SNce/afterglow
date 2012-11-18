#ifndef _OGLCONTEXT
#define _OGLCONTEXT

#include "Renderer\Windows\main.h"
#include "Renderer\Mesh\Mesh.h"

#include "ThirdParty\glm-0.9.3.4\glm\glm.hpp"
#include "ThirdParty\glm-0.9.3.4\glm\gtc\matrix_transform.hpp"

/**
	OGLContext is a class designed to store all of your OpenGL functions and keep them
	out of the way of your application logic. Here we have the ability to create an OpenGL
	context on a given window and then render to that window.
*/
class OGLContext 
{
public:
	OGLContext(void); // Default constructor
	OGLContext(HWND hwnd); // Constructor for creating our context given a hwnd
	~OGLContext(void); // Destructor for cleaning up our application
	bool create30Context(HWND hwnd); // Creation of our OpenGL 3.x context
	void setupScene(void); // All scene information can be setup here
	void reshapeWindow(int w, int h); // Method to get our window width and height on resize
	void preRenderScene(void); // Pre render scene
	void renderScene(void); // Render scene 
	void postRenderScene(void);	// Post render scene

private:
	int windowWidth; // Store the width of our window
	int windowHeight; // Store the height of our window

	glm::mat4 modelMat;
	glm::mat4 viewMat;
	glm::mat4 projectionMat;
	glm::mat4 mvMat;

	glm::vec3 cameraPos;
	glm::vec3 cameraLookat;
	glm::vec3 cameraUp;

	AfterGlow::Mesh* mesh;

protected:
	HGLRC hrc; // Rendering context
	HDC hdc; // Device context
	HWND hwnd; // Window identifier
};

#endif