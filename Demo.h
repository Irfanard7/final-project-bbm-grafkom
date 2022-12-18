#pragma once
#include "RenderEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <SOIL/SOIL.h>

class Demo :
	public RenderEngine
{
public:
	Demo();
	~Demo();
private:
	GLuint depthmapShader, shadowmapShader, cubeVBO, cubeVAO, cubeEBO, cube_texture, planeVBO, planeVAO, planeEBO, plane_texture, stexture, stexture2, depthMapFBO, depthMap,
		VBO2, VAO2, EBO2, texture2,
		VBO3, VAO3, EBO3, texture3,
		VBO4, VAO4, EBO4, texture4,
		VBOLampu, VAOLampu, EBOLampu, textureLampu,
		VBOSky, VAOSky, EBOSky, textureSky,
		VBOjalan, VAOjalan, EBOjalan, texturejalan;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	float viewCamX, viewCamY, viewCamZ, upCamX, upCamY, upCamZ, posCamX, posCamY, posCamZ, CAMERA_SPEED, fovy;
	float x1, z1;
	float x2, z2;
	float timestamp = 0;
	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow *window);
	void BuildTexturedCube();
	void BuildTexturedPlane();
	void DrawTexturedCube(GLuint shader, float x, float z, float puter);
	void DrawTexturedPlane(GLuint shader);
	void DrawSky(GLuint shader);
	void DrawJalan(GLuint shader);
	void DrawTree(GLuint shader, float x, float z);
	void DrawVillager(GLuint shader, float x, float z, float puter);
	void DrawLampuJalan(GLuint shader, float x, float z);
	void BuildDepthMap();
	void BuildShaders();
	void BuildTree();
	void BuildVillager();
	void BuildSky();
	void BuildJalan();
	void BuildLampuJalan();
	void MoveCamera(float speed);
	void StrafeCamera(float speed);
	void RotateCamera(float speed);
	void InitCamera();
};

