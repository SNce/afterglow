#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <vector>

#define C_FILE_READ

namespace AfterGlow
{

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	delete m_pVertexNormalBuffer;
	delete m_pPolyBuffer;
}

int Mesh::loadMesh(const char* meshFile)
{
	std::FILE *fp;
	fp = fopen(meshFile, "rb");

	if(fp)
	{
		int n;
		n = fread(&m_uiVertexCount, sizeof(unsigned int), 1, fp);
		m_pVertices = new AgVertex[m_uiVertexCount];

		n = 0;
		while(n < m_uiVertexCount)
		{
			int m = fread((char*)&(m_pVertices[n]), sizeof(AgVertex), 1, fp);
			n++;
		}

		n = fread(&m_uiPolyCount, sizeof(unsigned int), 1, fp);

		m_pPolyBuffer = new GLuint[3 * m_uiPolyCount];
		
		n = 0;
		while(n < m_uiPolyCount * 3)
		{
			int m = fread((char*)&(m_pPolyBuffer[n]), sizeof(GLuint), 1, fp);
			n++;
		}
	}

	// Create vertex array object;
	glGenVertexArrays(1, &m_uiVertexArrayID);
	glBindVertexArray(m_uiVertexArrayID);

	// Create vertex buffer object with the required attribs
	glGenBuffers(1, &m_uiVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, m_uiVertexCount * sizeof(AgVertex), m_pVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AgVertex), reinterpret_cast<void*>(offsetof(AgVertex, position)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AgVertex), reinterpret_cast<void*>(offsetof(AgVertex, normal)));

	// Create the index buffer object
	glGenBuffers(1, &m_uiIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIndexBuffer);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * m_uiPolyCount * sizeof(GLuint), m_pPolyBuffer, GL_STATIC_DRAW);

	// Make sure to reset the binding to prevent someone else from tampering with it.
	glBindVertexArray(0);

	m_uiProgramID = loadShaders("..\\Content\\Shaders\\clip.vert", "..\\Content\\Shaders\\red.frag");

	return 0;
}

int Mesh::loadShaders(const char* vertexFile, const char* fragmentFile)
{
	GLint result = GL_FALSE;
	int infoLogLength;
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::ofstream outputLog("output.txt", std::ios::out);

	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexFile, std::ios::in);
	if(vertexShaderStream.is_open())
	{
		char line[256];
		while(vertexShaderStream.getline(line, 256))
		{
			vertexShaderCode += std::string("\n");
			vertexShaderCode += std::string(line);
		}
		vertexShaderStream.close();
	}
	const char* vertexShaderSource = vertexShaderCode.c_str();

	// Compile vertex shader
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderID);

	// Check vertex shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> vertexShaderErrorMessage(infoLogLength);
	glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
	
	if(outputLog.is_open())
	{
		outputLog<<&vertexShaderErrorMessage[0]<<std::endl;
	}

	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentFile, std::ios::in);
	if(fragmentShaderStream.is_open())
	{
		char line[256];
		while(fragmentShaderStream.getline(line, 256))
		{
			fragmentShaderCode += std::string("\n");
			fragmentShaderCode += std::string(line);
		}
		fragmentShaderStream.close();
	}
	const char* fragmentShaderSource = fragmentShaderCode.c_str();

	// Compile fragment shader
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);

	// Check fragment shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> fragmentShaderErrorMessage(infoLogLength);
	glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
	
	if(outputLog.is_open())
	{
		outputLog<<&fragmentShaderErrorMessage[0]<<std::endl;
	}

	// Create and link a program
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> programErrorMessage(infoLogLength);
	glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
	
	if(outputLog.is_open())
	{
		outputLog<<&programErrorMessage[0]<<std::endl;
	}

	outputLog.close();

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}

}
