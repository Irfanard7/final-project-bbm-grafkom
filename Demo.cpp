#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	BuildShaders();
	BuildDepthMap();
	BuildTexturedCube();
	BuildTexturedPlane();
	BuildJalan();
	BuildLampuJalan();
	BuildTree();
	BuildVillager();
	InitCamera();
}
void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.001f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &planeEBO);
	glDeleteBuffers(1, &depthMapFBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &EBO3);
	glDeleteVertexArrays(1, &VAO4);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &EBO4);
	glDeleteVertexArrays(1, &VAOLampu);
	glDeleteBuffers(1, &VBOLampu);
	glDeleteBuffers(1, &EBOLampu);
	glDeleteVertexArrays(1, &VAOjalan);
	glDeleteBuffers(1, &VBOjalan);
	glDeleteBuffers(1, &EBOjalan);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);




}

void Demo::Update(double deltaTime) {
	timestamp += 2;
	x1 = -10;
	x2 = -7;
	if (z1 <= 500.f) {
		z1 = 6 + timestamp / 1000.f;
		z2 = -10 + timestamp / 1000.f;
	}
	else {
		z1 -= timestamp / 1000.f;
		z2 -= timestamp / 1000.f;
	}

	if (timestamp >= 15000.f) {
		timestamp = 0;
	}
}

void Demo::Render() {
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	
	// Step 1 Render depth of scene to texture
	// ----------------------------------------
	glm::mat4 lightProjection, lightView, lightProjection2, lightView2;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.0f, far_plane = 100.f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightProjection2 = glm::ortho(-20.f, 20.f, -20.f, 20.f, near_plane, far_plane);
	lightView = glm::lookAt(glm::vec3(-2, 10, -1.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightView2 = glm::lookAt(glm::vec3(7, 0.2, 4 + timestamp/1000.f), glm::vec3(7, 0.2, 20 + timestamp/1000.f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	UseShader(this->depthmapShader);
	glUniformMatrix4fv(glGetUniformLocation(this->depthmapShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glViewport(0, 0, this->SHADOW_WIDTH, this->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawTexturedCube(this->depthmapShader,0,0, 0);
	DrawTexturedCube(this->depthmapShader, 0, -15, 0);
	DrawTexturedCube(this->depthmapShader, 13, -15, 0);
	DrawTexturedCube(this->depthmapShader, 12, 10, 0);
	DrawTexturedCube(this->depthmapShader, 0, 15, 1);
	DrawTexturedPlane(this->depthmapShader);
	DrawJalan(this->depthmapShader);
	for (int i = 0; i < 9; i++) {
		DrawTree(this->depthmapShader, -15, -18 + i*4);
	}
	for (int i = 0; i < 9; i++) {
		DrawTree(this->depthmapShader, -19, -18 + i * 4);
	}
	for (int i = 0; i < 9; i++) {
		DrawTree(this->depthmapShader, -23, -18 + i * 4);
	}
	DrawVillager(this->depthmapShader,  x1,  z1, 0);
	//DrawVillager(this->depthmapShader, x2, z2, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	// Step 2 Render scene normally using generated depth map
	// ------------------------------------------------------
	glViewport(0, 0, this->screenWidth, this->screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Pass perspective projection matrix
	UseShader(this->shadowmapShader);
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->shadowmapShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::vec3 cameraPos = glm::vec3(posCamX, posCamY, posCamZ);
	glm::vec3 cameraFront = glm::vec3(viewCamX, viewCamY, viewCamZ);
	glm::mat4 view = glm::lookAt(cameraPos, cameraFront, glm::vec3(upCamX, upCamY, upCamZ));
	glUniformMatrix4fv(glGetUniformLocation(this->shadowmapShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	
	// Setting Light Attributes
	glUniformMatrix4fv(glGetUniformLocation(this->shadowmapShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniform3f(glGetUniformLocation(this->shadowmapShader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(this->shadowmapShader, "lightPos"), -2.0f, 20.0f, -1.0f);

	// Configure Shaders
	glUniform1i(glGetUniformLocation(this->shadowmapShader, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(this->shadowmapShader, "shadowMap"), 1);

	// Render floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, plane_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawTexturedPlane(this->shadowmapShader);
	
	// Render cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawTexturedCube(this->shadowmapShader, 0, 0,0);

	// Render cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawTexturedCube(this->shadowmapShader, 0, -15, 0);

	// Render cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawTexturedCube(this->shadowmapShader, 13, -15, 0);

	// Render cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawTexturedCube(this->shadowmapShader, 12, 10,0);

	// Render cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawTexturedCube(this->shadowmapShader, 0, 15, 1);

	// Render cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturejalan);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawJalan(this->shadowmapShader);

	for (int i = 0; i < 9; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		DrawTree(this->shadowmapShader, -15, -18 + i * 4);
	}
	for (int i = 0; i < 9; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		DrawTree(this->shadowmapShader, -19, -18 + i * 4);
	}
	for (int i = 0; i < 9; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		DrawTree(this->shadowmapShader, -23, -18 + i * 4);
	}


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawVillager(this->shadowmapShader, x1, z1, 0);

	glDisable(GL_DEPTH_TEST);
	
}

void Demo::BuildLampuJalan() {
	glGenTextures(1, &textureLampu);
	glBindTexture(GL_TEXTURE_2D, textureLampu);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("brown.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords, normal
		// front
		-1.0, -1.0, 1.0, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		1.0, -1.0, 1.0, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		1.0,  1.0, 1.0, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-1.0,  1.0, 1.0, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.0,  1.0,  1.0, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.0,  1.0, -1.0, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.0, -1.0, -1.0, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.0, -1.0,  1.0, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		1.0,  -1.0, -1.0, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		1.0,   1.0, -1.0, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-1.0,  1.0, -1.0, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.0, -1.0, -1.0, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		-1.0, -1.0,  1.0, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		-1.0,  1.0,  1.0, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		-1.0,  1.0, -1.0, 0, 1, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.0, 1.0,  1.0, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-1.0, 1.0, 1.0, 1, 0,   0.0f,  1.0f,  0.0f, // 17
		-1.0, 1.0, -1.0, 1, 1,  0.0f,  1.0f,  0.0f, // 18
		1.0, 1.0, -1.0, 0, 1,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		1.0, -1.0, -1.0, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		1.0, -1.0,  1.0, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-1.0, -1.0,  1.0, 0, 1, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAOLampu);
	glGenBuffers(1, &VBOLampu);
	glGenBuffers(1, &EBOLampu);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOLampu);

	glBindBuffer(GL_ARRAY_BUFFER, VBOLampu);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOLampu);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::DrawLampuJalan(GLuint shader, float x, float z)
{
	UseShader(shader);

	glBindVertexArray(VAOLampu); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(shader, "objectColor");
	GLint modelLoc = glGetUniformLocation(shader, "model");

	//gambar tiang
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.53f, 0.33f, 0.29f);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(x, 0, z));
	model = glm::scale(model, glm::vec3(0.05, 2, 0.05));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar penampu
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 1, 1, 0.69f);
	glm::mat4 model2;
	model2 = glm::translate(model2, glm::vec3(x, 1, z));
	model2 = glm::scale(model2, glm::vec3(0.08, 0.2f, 0.08f));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
}
void Demo::BuildVillager() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("brown.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords, normal
		// front
		-1.0, -1.0, 1.0, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		1.0, -1.0, 1.0, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		1.0,  1.0, 1.0, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-1.0,  1.0, 1.0, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.0,  1.0,  1.0, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.0,  1.0, -1.0, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.0, -1.0, -1.0, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.0, -1.0,  1.0, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		1.0,  -1.0, -1.0, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		1.0,   1.0, -1.0, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-1.0,  1.0, -1.0, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.0, -1.0, -1.0, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		-1.0, -1.0,  1.0, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		-1.0,  1.0,  1.0, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		-1.0,  1.0, -1.0, 0, 1, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.0, 1.0,  1.0, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-1.0, 1.0, 1.0, 1, 0,   0.0f,  1.0f,  0.0f, // 17
		-1.0, 1.0, -1.0, 1, 1,  0.0f,  1.0f,  0.0f, // 18
		1.0, 1.0, -1.0, 0, 1,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		1.0, -1.0, -1.0, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		1.0, -1.0,  1.0, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-1.0, -1.0,  1.0, 0, 1, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::DrawVillager(GLuint shader, float x, float z, float puter)
{
	UseShader(shader);

	glBindVertexArray(VAO4); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(shader, "objectColor");
	GLint modelLoc = glGetUniformLocation(shader, "model");

	//gambar kepala
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.84f, 0.75f, 0.68f);

	glm::mat4 model, rotate;
	rotate = glm::rotate(rotate, glm::radians(90.f), glm::vec3(0, 1, 0));

	if (puter == 1) {
		model *= rotate;
	}
	model = glm::translate(model, glm::vec3(x, 0.4, z));

	model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar alis
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f);

	glm::mat4 model1;
	if (puter == 1) {
		model1 *= rotate;
	}
	model1 = glm::translate(model1, glm::vec3(x, 0.44, z + 0.2));
	model1 = glm::scale(model1, glm::vec3(0.14, 0.03, 0.03));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar hidung belang
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.2f, 0.2f, 0.2f);

	glm::mat4 modelHidung;
	if (puter == 1) {
		modelHidung *= rotate;
	}
	modelHidung = glm::translate(modelHidung, glm::vec3(x, 0.37, z + 0.2));
	
	modelHidung = glm::scale(modelHidung, glm::vec3(0.03, 0.05, 0.03));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelHidung));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar mata kiri
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.2f, 0.2f, 0.2f);

	glm::mat4 model2;
	if (puter == 1) {
		model2 *= rotate;
	}
	model2 = glm::translate(model2, glm::vec3(x - 0.04, 0.41, z + 0.2));
	
	model2 = glm::scale(model2, glm::vec3(0.03, 0.03, 0.03));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar mata kanan
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.2f, 0.2f, 0.2f);

	glm::mat4 model3;
	if (puter == 1) {
		model3 *= rotate;
	}
	model3 = glm::translate(model3, glm::vec3(x + 0.04, 0.41, z + 0.2));
	
	model3 = glm::scale(model3, glm::vec3(0.03, 0.03, 0.03));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar badan
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.4f, 0.35f, 0.28f);

	glm::mat4 model4;
	if (puter == 1) {
		model4 *= rotate;
	}
	model4 = glm::translate(model4, glm::vec3(x, 0.1, z));
	
	model4 = glm::scale(model4, glm::vec3(0.2, 0.4, 0.2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar tangan kanan
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.84f, 0.75f, 0.68f);

	glm::mat4 model5;
	if (puter == 1) {
		model5 *= rotate;
	}
	model5 = glm::translate(model5, glm::vec3(x + 0.2f, 0.05f, z));
	
	model5 = glm::scale(model5, glm::vec3(0.1, 0.2, 0.1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar tangan kiri
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.84f, 0.75f, 0.68f);

	glm::mat4 model6;
	if (puter == 1) {
		model6 *= rotate;
	}
	model6 = glm::translate(model6, glm::vec3(x - 0.2f, 0.05f, z));
	
	model6 = glm::scale(model6, glm::vec3(0.1, 0.2, 0.1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar kaki kiri
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.4f, 0.35f, 0.28f);

	glm::mat4 model7;
	if (puter == 1) {
		model7 *= rotate;
	}
	model7 = glm::translate(model7, glm::vec3(x + 0.05f, -0.2f, z));
	
	model7 = glm::scale(model7, glm::vec3(0.1, 0.4, 0.1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar kaki kanan
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.4f, 0.35f, 0.28f);

	glm::mat4 model8;
	if (puter == 1) {
		model8 *= rotate;
	}
	model8 = glm::translate(model8, glm::vec3(x - 0.05f, -0.2f, z));
	
	model8 = glm::scale(model8, glm::vec3(0.1, 0.4, 0.1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar kaki kanan bawah
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.4f, 0.35f, 0.28f);

	glm::mat4 model9;
	if (puter == 1) {
		model9 *= rotate;
	}
	model9 = glm::translate(model9, glm::vec3(x - 0.05f, -0.4f, z + 0.05f));
	
	model9 = glm::scale(model9, glm::vec3(0.1, 0.1, 0.15));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar kaki kiri bawah
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.4f, 0.35f, 0.28f);

	glm::mat4 model10;
	if (puter == 1) {
		model10 *= rotate;
	}
	model10 = glm::translate(model10, glm::vec3(x + 0.05f, -0.4f, z + 0.05f));
	
	model10 = glm::scale(model10, glm::vec3(0.1, 0.1, 0.15));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//gambar senter
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.3, 0.3, 0.3);

	glm::mat4 model11;
	if (puter == 1) {
		model11 *= rotate;
	}
	model11 = glm::translate(model11, glm::vec3(x - 0.2f, -0.2, z + 0.05f));
	
	model11 = glm::scale(model11, glm::vec3(0.05, 0.05, 0.2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.8, 0.8, 0.8);

	glm::mat4 model12;
	if (puter == 1) {
		model12 *= rotate;
	}
	model12 = glm::translate(model12, glm::vec3(x - 0.2f, -0.2, z + 0.1f));
	
	model12 = glm::scale(model12, glm::vec3(0.05, 0.05, 0.2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model12));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// set lighting attribute
	GLint lightPosLoc2 = glGetUniformLocation(shader, "lightPos2");
	if (puter == 1) {
		glUniform3f(lightPosLoc2,  z + 0.1f, -0.2, x - 0.13f);
	}
	else {
		glUniform3f(lightPosLoc2, x - 0.13f, -0.2, z + 0.1f);
	}
	GLint viewPosLoc2 = glGetUniformLocation(shader, "viewPos2");
	if (puter == 1) {
		glUniform3f(viewPosLoc2,  z + 10000, -0.2, x - 0.13f);
	}
	else {
		glUniform3f(viewPosLoc2, x - 0.13f, -0.2, z + 10000);
	}
	GLint lightColorLoc2 = glGetUniformLocation(shader, "lightColor2");
	glUniform3f(lightColorLoc2, 1.0f, 1.0f, 0.86f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildJalan() {
	glGenTextures(1, &texturejalan);
	glBindTexture(GL_TEXTURE_2D, texturejalan);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("brown.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords, normal
		// front
		-1.0, -1.0, 1.0, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		1.0, -1.0, 1.0, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		1.0,  1.0, 1.0, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-1.0,  1.0, 1.0, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.0,  1.0,  1.0, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.0,  1.0, -1.0, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.0, -1.0, -1.0, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.0, -1.0,  1.0, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		1.0,  -1.0, -1.0, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		1.0,   1.0, -1.0, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-1.0,  1.0, -1.0, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.0, -1.0, -1.0, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		-1.0, -1.0,  1.0, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		-1.0,  1.0,  1.0, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		-1.0,  1.0, -1.0, 0, 1, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.0, 1.0,  1.0, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-1.0, 1.0, 1.0, 1, 0,   0.0f,  1.0f,  0.0f, // 17
		-1.0, 1.0, -1.0, 1, 1,  0.0f,  1.0f,  0.0f, // 18
		1.0, 1.0, -1.0, 0, 1,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		1.0, -1.0, -1.0, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		1.0, -1.0,  1.0, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-1.0, -1.0,  1.0, 0, 1, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAOjalan);
	glGenBuffers(1, &VBOjalan);
	glGenBuffers(1, &EBOjalan);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOjalan);

	glBindBuffer(GL_ARRAY_BUFFER, VBOjalan);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOjalan);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
void Demo::DrawJalan(GLuint shader)
{
	UseShader(shader);

	glBindVertexArray(VAOjalan); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(shader, "objectColor");
	GLint modelLoc = glGetUniformLocation(shader, "model");

	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.6, 0.6, 0.6);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(-10, -.5, 7));
	model = glm::scale(model, glm::vec3(1, 0.05, 30));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model2;
	model2 = glm::translate(model2, glm::vec3(10, -.5, -7));
	model2 = glm::scale(model2, glm::vec3(20, 0.05, 1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}
void Demo::BuildTexturedCube()
{
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &cube_texture);
	glBindTexture(GL_TEXTURE_2D, cube_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("wood.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords, normal
		// front
		-1.0, -1.0, 1.0, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		1.0, -1.0, 1.0, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		1.0,  1.0, 1.0, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-1.0,  1.0, 1.0, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		 // right
		 1.0,  1.0,  1.0, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		 1.0,  1.0, -1.0, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		 1.0, -1.0, -1.0, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		 1.0, -1.0,  1.0, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		1.0,  -1.0, -1.0, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		1.0,   1.0, -1.0, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-1.0,  1.0, -1.0, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		 // left
		 -1.0, -1.0, -1.0, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		 -1.0, -1.0,  1.0, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		 -1.0,  1.0,  1.0, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		 -1.0,  1.0, -1.0, 0, 1, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.0, 1.0,  1.0, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-1.0, 1.0, 1.0, 1, 0,   0.0f,  1.0f,  0.0f, // 17
		-1.0, 1.0, -1.0, 1, 1,  0.0f,  1.0f,  0.0f, // 18
		1.0, 1.0, -1.0, 0, 1,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		1.0, -1.0, -1.0, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		1.0, -1.0,  1.0, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-1.0, -1.0,  1.0, 0, 1, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::BuildTree() {
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("brown.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords, normal
		// front
		-1.0, -1.0, 1.0, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		1.0, -1.0, 1.0, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		1.0,  1.0, 1.0, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-1.0,  1.0, 1.0, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.0,  1.0,  1.0, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.0,  1.0, -1.0, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.0, -1.0, -1.0, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.0, -1.0,  1.0, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		1.0,  -1.0, -1.0, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		1.0,   1.0, -1.0, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-1.0,  1.0, -1.0, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.0, -1.0, -1.0, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		-1.0, -1.0,  1.0, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		-1.0,  1.0,  1.0, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		-1.0,  1.0, -1.0, 0, 1, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.0, 1.0,  1.0, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-1.0, 1.0, 1.0, 1, 0,   0.0f,  1.0f,  0.0f, // 17
		-1.0, 1.0, -1.0, 1, 1,  0.0f,  1.0f,  0.0f, // 18
		1.0, 1.0, -1.0, 0, 1,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.0, -1.0, -1.0, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		1.0, -1.0, -1.0, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		1.0, -1.0,  1.0, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-1.0, -1.0,  1.0, 0, 1, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawTree(GLuint shader, float x, float z)
{
	UseShader(shader);

	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(shader, "objectColor");
	GLint modelLoc = glGetUniformLocation(shader, "model");

	//gambar batang
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.3f, 0.2f, 0.2f);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(x, -2, z));
	model = glm::scale(model, glm::vec3(0.2, 3, 0.2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	//gambar daun
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.81, 0.99f, 0.88f);

	glm::mat4 model2;
	model2 = glm::translate(model2, glm::vec3(x, 0.7 * 2, z));
	model2 = glm::scale(model2, glm::vec3(0.4, 0.4, 0.4));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model3;
	model3 = glm::translate(model3, glm::vec3(x + 0.3, 0.6 * 2, z));
	model3 = glm::scale(model3, glm::vec3(0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model4;
	model4 = glm::translate(model4, glm::vec3(x + 0.15, 0.6 * 2, z + 0.2));
	model4 = glm::scale(model4, glm::vec3(0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model5;
	model5 = glm::translate(model5, glm::vec3(x - 0.07, 0.55 * 2, z + 0.25));
	model5 = glm::scale(model5, glm::vec3(0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model6;
	model6 = glm::translate(model6, glm::vec3(x - 0.15, 0.75 * 2, z + 0.15));
	model6 = glm::scale(model6, glm::vec3(-0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model7;
	model7 = glm::translate(model7, glm::vec3(x - 0.2, 0.8 * 2, z + 0.15));
	model7 = glm::scale(model7, glm::vec3(-0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model8;
	model8 = glm::translate(model8, glm::vec3(x - 0.1, 0.65 * 2, z - 0.15));
	model8 = glm::scale(model8, glm::vec3(-0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model9;
	model9 = glm::translate(model9, glm::vec3(x + 0.1, 0.65 * 2, z + 0.15));
	model9 = glm::scale(model9, glm::vec3(-0.4 * 2, 0.4 * 2, 0.4 * 2));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildTexturedPlane()
{
	// Load and create a texture 
	glGenTextures(1, &plane_texture);
	glBindTexture(GL_TEXTURE_2D, plane_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("brown.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-25.0f,	-0.5f, -25.0f,  0,  0, 0.0f,  1.0f,  0.0f,
		25.0f,	-0.5f, -25.0f, 25,  0, 0.0f,  1.0f,  0.0f,
		25.0f,	-0.5f,  25.0f, 25, 25, 0.0f,  1.0f,  0.0f,
		-25.0f,	-0.5f,  25.0f,  0, 25, 0.0f,  1.0f,  0.0f,
	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawTexturedCube(GLuint shader, float x, float z, float puter)
{
	UseShader(shader);
	glBindVertexArray(cubeVAO);
	GLint objectColorLoc = glGetUniformLocation(shader, "objectColor");
	GLint modelLoc = glGetUniformLocation(shader, "model");

	float x1 = x;
	float z1 = z;
	float tinggiCagak = 3;
	float rotasi1 = 0;
	float ceil = 5;
	float widthScaleCeil = 1;
	float heightScaleCeil = 0.5;

	//atap
	for (int i = 0; i < ceil; i++) {
		objectColorLoc = glGetUniformLocation(shader, "objectColor");
		glUniform3f(objectColorLoc, 0.6f, 0.5f, 0.4f);

		glm::mat4 model2;
		model2 = glm::translate(model2, glm::vec3(x1, tinggiCagak + (ceil - i - 2) * heightScaleCeil, z1));
		model2 = glm::scale(model2, glm::vec3((i + 1) * widthScaleCeil, heightScaleCeil, (i + 1) * widthScaleCeil));

		glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//cagak
	for (int i = 0; i < 4; i++) {
		objectColorLoc = glGetUniformLocation(shader, "objectColor");
		glUniform3f(objectColorLoc, 0.4f, 0.3f, 0.2f);

		glm::mat4 model2;
		float kalix = i % 3 == 0 ? -1 : 1;
		float kaliz = i < 2 ? -1 : 1;
		model2 = glm::translate(model2, glm::vec3(x1 + (kalix * widthScaleCeil * (ceil - 3)), 0, z1 + (kaliz * widthScaleCeil * (ceil - 3))));
		model2 = glm::scale(model2, glm::vec3(0.2, tinggiCagak + 0.5, 0.2));

		glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//tembok depan
	float lebarTembok = widthScaleCeil * (ceil - 1);
	float margin = lebarTembok;
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.9f, 0.8f, 0.8f);

	glm::mat4 model3;
	model3 = glm::translate(model3, glm::vec3(x1, 0.2, z1 + margin));
	model3 = glm::scale(model3, glm::vec3(lebarTembok, tinggiCagak + 0.1, 0.1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//tembok kanan
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.9f, 0.8f, 0.8f);

	glm::mat4 model4;
	model4 = glm::translate(model4, glm::vec3(x1 + margin, 0.2, z1));
	model4 = glm::scale(model4, glm::vec3(0.1, tinggiCagak + 0.1, lebarTembok));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//tembok belakang
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.9f, 0.8f, 0.8f);

	glm::mat4 model5;
	model5 = glm::translate(model5, glm::vec3(x1, 0.2, z1 - margin));
	model5 = glm::scale(model5, glm::vec3(lebarTembok, tinggiCagak + 0.1, 0.1));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//tembok kiri
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.9f, 0.8f, 0.8f);

	glm::mat4 model6;
	model6 = glm::translate(model6, glm::vec3(x1 - margin, 0.2, z1));
	model6 = glm::scale(model6, glm::vec3(0.1, tinggiCagak + 0.1, lebarTembok));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//lawang
	objectColorLoc = glGetUniformLocation(shader, "objectColor");
	glUniform3f(objectColorLoc, 0.9f, 0.9f, 0.9f);

	glm::mat4 model7;
	
	model7 = glm::translate(model7, glm::vec3(x1, 0.05, z1 + margin + 0.005));
	if (puter == 1) {
		model7 = glm::rotate(model7, glm::radians(90.f), glm::vec3(0, 1, 0));
		model7 = glm::translate(model7, glm::vec3(lebarTembok/1.f,0, -(lebarTembok/1.f)));
	}
	model7 = glm::scale(model7, glm::vec3(0.6, 1, 0.25));

	glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawTexturedPlane(GLuint shader)
{
	UseShader(shader);
	glBindVertexArray(planeVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildDepthMap() {
	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->SHADOW_WIDTH, this->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Demo::BuildShaders()
{
	// build and compile our shader program
	// ------------------------------------
	shadowmapShader = BuildShader("shadowMapping.vert", "shadowMapping.frag", nullptr);
	depthmapShader = BuildShader("depthMap.vert", "depthMap.frag", nullptr);
}


int main(int argc, char** argv) {
	RenderEngine &app = Demo();	
	app.Start("Shadow Mapping Demo", 1600, 1200, false, true);
}