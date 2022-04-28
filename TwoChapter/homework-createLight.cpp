#include"../OneChapter/Camera.h"
#include"../OneChapter/shader.h"
#include<stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include<iostream>
#include<Eigen/Eigen>

#include<glad/glad.h>
#include<glfw3.h>
#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif

#define MY_PI 3.1415926

double last_mouse_x = 0.0;
double last_mouse_y = 0.0;
bool first_use = true;
Camera camera({ 0,0,3 });
int framebuffer_width = 800;
int framebuffer_height = 600;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	framebuffer_width = width;
	framebuffer_height = height;
}

void processInput(GLFWwindow* window, Camera& camera, float deltatime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltatime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (first_use) {
		first_use = false;
		last_mouse_x = xpos;
		last_mouse_y = ypos;
	}
	camera.ProcessMouseMovement(xpos - last_mouse_x, last_mouse_y - ypos);
	last_mouse_x = xpos;
	last_mouse_y = ypos;
	//std::cout << "x_delta:" << xpos - last_mouse_x << "y_delta:" << last_mouse_y - ypos << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
		-eye_pos[2], 0, 0, 0, 1;

	view = translate * view;

	return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector3f axis, Eigen::Vector3f translation)
{
	Eigen::Matrix4f rotMatrix = Eigen::Matrix4f::Identity();
	if (rotation_angle <= 1e-3f) {
		rotMatrix.block(0, 0, 3, 3) = Eigen::Matrix3f::Identity();
	}
	else {
		Eigen::AngleAxisf AA(rotation_angle, axis / axis.norm());
		rotMatrix.block(0, 0, 3, 3) = AA.toRotationMatrix();
	}
	Eigen::Matrix4f transMatrix = Eigen::Matrix4f::Identity();
	transMatrix.block(0, 3, 3, 1) = translation;
	return transMatrix * rotMatrix;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector3f translation)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

	model <<
		cos(rotation_angle * MY_PI / 180), -sin(rotation_angle * MY_PI / 180), 0, translation[0],
		sin(rotation_angle * MY_PI / 180), cos(rotation_angle * MY_PI / 180), 0, translation[1],
		0, 0, 1, translation[2],
		0, 0, 0, 1;

	return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
	float zNear, float zFar)
{
	// Students will implement this function

	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	zNear = -zNear;
	zFar = -zFar;
	Eigen::Matrix4f squishMatrix;
	squishMatrix <<
		zNear, 0, 0, 0,
		0, zNear, 0, 0,
		0, 0, zNear + zFar, -zNear * zFar,
		0, 0, 1, 0;
	// Then, trans the cuboid into [-1,1]*[-1,1]*[-1,1]
	// x:[l,r] y:[b,t] z:[f,n]
	float b = tan(eye_fov / 2 * MY_PI / 180) * zNear;
	float t = -b;
	float l = b * aspect_ratio;
	float r = t * aspect_ratio;
	float f = zFar;
	float n = zNear;
	Eigen::Matrix4f orthoMatrixTrans;
	orthoMatrixTrans <<
		1, 0, 0, -(l + r) / 2,
		0, 1, 0, -(b + t) / 2,
		0, 0, 1, -(f + n) / 2,
		0, 0, 0, 1;
	Eigen::Matrix4f orthoMatrixScale;
	orthoMatrixScale <<
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (n - f), 0,
		0, 0, 0, 1;

	Eigen::Matrix4f right2left;
	right2left <<
		-1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, -1;

	projection = right2left * orthoMatrixScale * orthoMatrixTrans * squishMatrix;

	return projection;
}

int main()
{
	/*
	* Blinn Phong光照模型综合测试
	* 平行光源，衰减式点光源，聚光源
	* Camera移动控制
	* 同一箱子的多重显示
	* 材质纹理贴图，光照纹理贴图
	* 提高：聚光特效
	*/
	
	// init opengl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(framebuffer_width, framebuffer_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // 设置窗口内容为当前线程的主内容
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 注册回调函数 指示 每次窗口大小变换后，应该怎样操作
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// init shader
	Shader shader("../data/shader/vertexShaderLight.glsl", "../data/shader/frugShaderLight.glsl");
	// init data
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	Eigen::Vector3f cubePositions[] = {
		Eigen::Vector3f(0.0f,  0.0f,  0.0f),
		Eigen::Vector3f(2.0f,  5.0f, -15.0f),
		Eigen::Vector3f(-1.5f, -2.2f, -2.5f),
		Eigen::Vector3f(-3.8f, -2.0f, -12.3f),
		Eigen::Vector3f(2.4f, -0.4f, -3.5f),
		Eigen::Vector3f(-1.7f,  3.0f, -7.5f),
		Eigen::Vector3f(1.3f, -2.0f, -2.5f),
		Eigen::Vector3f(1.5f,  2.0f, -2.5f),
		Eigen::Vector3f(1.5f,  0.2f, -1.5f),
		Eigen::Vector3f(-1.3f,  1.0f, -1.5f)
	};

	//glm::vec3 cubePositions[] = {
	//	glm::vec3(0.0f,  0.0f,  0.0f),
	//	glm::vec3(2.0f,  5.0f, -15.0f),
	//	glm::vec3(-1.5f, -2.2f, -2.5f),
	//	glm::vec3(-3.8f, -2.0f, -12.3f),
	//	glm::vec3(2.4f, -0.4f, -3.5f),
	//	glm::vec3(-1.7f,  3.0f, -7.5f),
	//	glm::vec3(1.3f, -2.0f, -2.5f),
	//	glm::vec3(1.5f,  2.0f, -2.5f),
	//	glm::vec3(1.5f,  0.2f, -1.5f),
	//	glm::vec3(-1.3f,  1.0f, -1.5f)
	//};

	Eigen::Vector3f pointLightPositions[] = {
		Eigen::Vector3f(0.7f,  0.2f,  2.0f),
		Eigen::Vector3f(2.3f, -3.3f, -4.0f),
		Eigen::Vector3f(-4.0f,  2.0f, -12.0f),
		Eigen::Vector3f(0.0f,  0.0f, -3.0f)
	};

	// init texture
	stbi_set_flip_vertically_on_load(true);
	int width1, height1, nrChannels1;
	auto tex1Data = stbi_load("../data/texture/container2.png", &width1, &height1, &nrChannels1, 0);
	int width2, height2, nrChannels2;
	auto tex2Data = stbi_load("../data/texture/container2_specular.png", &width2, &height2, &nrChannels2, 0);
	int width3, height3, nrChannels3;
	auto tex3Data = stbi_load("../data/texture/awesomeface.png", &width3, &height3, &nrChannels3, 0);
	unsigned int texture[3];
	glGenTextures(3, texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1Data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex2Data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex3Data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// bind data
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO); // 自绑定起，后续的Buffer绑定配置 都相当于记录在了VAO上
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	shader.use();

	shader.setInt("material.diffuse", 0);
	shader.setInt("material.specular", 1);
	shader.setInt("spotLight.touying", 2);

	// set uniform data
	glEnable(GL_DEPTH_TEST);
	Eigen::Matrix4f viewMatrix = camera.GetViewMatrix();
	Eigen::Matrix4f proMatrix = get_projection_matrix(45.0f, (float)framebuffer_width / (float)framebuffer_height, 0.1f, 100.0f);
	Eigen::Matrix4f modelMatrix;
	// 注册 鼠标移动函数 键盘相应函数 滚轮相应函数
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	float lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera, glfwGetTime() - lastTime);
		lastTime = glfwGetTime();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		// set light data
		shader.setVec3("parallelLight.direction", -0.2f, -1.0f, -0.3f);
		shader.setVec3("parallelLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("parallelLight.diffuse", 0.4f, 0.4f, 0.4f);
		shader.setVec3("parallelLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09f);
		shader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[1].constant", 1.0f);
		shader.setFloat("pointLights[1].linear", 0.09f);
		shader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		shader.setVec3("pointLights[2].position", pointLightPositions[2]);
		shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[2].constant", 1.0f);
		shader.setFloat("pointLights[2].linear", 0.09f);
		shader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		shader.setVec3("pointLights[3].position", pointLightPositions[3]);
		shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[3].constant", 1.0f);
		shader.setFloat("pointLights[3].linear", 0.09f);
		shader.setFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		shader.setVec3("spotLight.position", camera.Position);
		shader.setVec3("spotLight.direction", camera.Front);
		shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.09f);
		shader.setFloat("spotLight.quadratic", 0.032f);
		shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// shader.setFloat("spotLight.outerCutOff", cos(15.0f * MY_PI / 180.0f));

		shader.setFloat("material.shininess", 32.0f);

		auto i = 0;
		for (auto curP : cubePositions) {

			modelMatrix = get_model_matrix(0.0f, { 0.0,1.0,0.0 }, curP);
			modelMatrix = modelMatrix * get_model_matrix(20.0f * i * MY_PI / 180.0f, { 1.0f, 0.3f, 0.5f }, { 0.0,0.0,0.0 });
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "modelMatrix"), 1, GL_FALSE, modelMatrix.data());
			++i;

			viewMatrix = camera.GetViewMatrix();
			proMatrix = get_projection_matrix(camera.Zoom, (float)framebuffer_width / (float)framebuffer_height, 0.1f, 100.0f);
			shader.setVec3("viewPos", camera.Position);

			//glUniformMatrix4fv(glGetUniformLocation(shader.ID, "modelMatrix"), 1, GL_FALSE, modelMatrix.data());
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "viewMatrix"), 1, GL_FALSE, viewMatrix.data());
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projectionMatrix"), 1, GL_FALSE, proMatrix.data());

			glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
			glDrawArrays(GL_TRIANGLES, 0, 36);

		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader.ID);

	glfwTerminate();
	return 0;

}