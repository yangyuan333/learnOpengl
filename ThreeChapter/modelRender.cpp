#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <Eigen/Eigen>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc142-mtd.lib")
#else
#pragma comment(lib, "assimp-vc142-mt.lib")
#endif

#include "../utils/Camera.h"
#include "../utils/mesh.h"
#include "../utils/model.h"
#include "../utils/shader.h"

#include <glad/glad.h>
#include <glfw3.h>
#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif

using namespace std;

#define MY_PI 3.1415926
double last_mouse_x = 0.0;
double last_mouse_y = 0.0;
bool first_use = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window, Camera& camera, float deltatime);

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector3f axis, Eigen::Vector3f translation);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera({ 0,0,3.0 });

int framebuffer_width = 800;
int framebuffer_height = 600;

int main()
{
    // init opengl
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    // init model data
    Model model("../data/model/nanosuit/nanosuit.obj");
    // init shader
    Shader shader("../data/shader/vertexShaderLight.glsl", "../data/shader/frugShaderLight.glsl");
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // begin render
    float lasttime = glfwGetTime();
    while (glfwWindowShouldClose(window))
    {
        processInput(window, camera, glfwGetTime() - lasttime);
        lasttime = glfwGetTime();

        shader.use();
        // get MVP 
        Eigen::Matrix4f modelMatrix = get_model_matrix(0.0, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 });
        Eigen::Matrix4f viewMatrix = camera.GetViewMatrix();
        Eigen::Matrix4f projectionMatrix = get_projection_matrix(camera.Zoom, (float)framebuffer_width / (float)framebuffer_height, 0.1f, 100.0f);
        
        shader.setMat4("modelMatrix", modelMatrix);
        shader.setMat4("viewMatrix", viewMatrix);
        shader.setMat4("projectionMatrix", projectionMatrix);

		shader.setFloat("")

        model.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
    }
	glfwTerminate();
	return 0;
}

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