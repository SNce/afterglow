#ifndef _MESH
#define _MESH
#include "ThirdParty\glew-1.9.0\include\GL\glew.h"
#include "ThirdParty\glm-0.9.3.4\glm\glm.hpp"

namespace AfterGlow
{

struct AgVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

class Mesh
{
private:
	void* m_pVertexNormalBuffer;
	//GLubyte* m_pPolyBuffer;
	GLuint* m_pPolyBuffer;

	GLuint m_uiVertexBuffer;
	/*GLuint m_uiIndexBuffer;*/
	AgVertex* m_pVertices;

public:
	Mesh();
	~Mesh();

	GLuint m_uiVertexArrayID;
	GLuint m_uiIndexBuffer;

	unsigned int m_uiVertexCount;
	unsigned int m_uiPolyCount;
	unsigned int m_uiProgramID;

	int loadMesh(const char* meshFile);
	int loadShaders(const char* vertexFile, const char* fragmentFile);
};

}

#endif