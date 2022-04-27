#ifndef MYOPENGL_H
#define MYOPENGL_H

#include<glad/glad.h>
#include<glfw3.h>
#include<iostream>

class MyOpengl
{
public:
    MyOpengl(std::string nameWindow="LearnOpenGL", int framebuffer_width=800, int framebuffer_height=600)
	{
        if (initOpengl(nameWindow)) {
            std::cout << "Init opengl success!" << std::endl;
        }
        else {
            std::cout << "Init opengl errors!" << std::endl;
        }
	}

	bool initOpengl(std::string nameWindow)
	{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(framebuffer_width, framebuffer_height, nameWindow.c_str(), NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window); // 设置窗口内容为当前线程的主内容
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }
        return true;
	}

    void setFramebufferSizeCallback(GLFWframebuffersizefun callback)
    {
        glfwSetFramebufferSizeCallback(window, callback);
    }

public:
	int framebuffer_width;
	int framebuffer_height;
    GLFWwindow* window;
};

#endif