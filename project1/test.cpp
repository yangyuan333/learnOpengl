#include<glad/glad.h>
#include<glfw3.h>
#include<iostream>
#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit(); // 初始化glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opengl版本号 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // opengl次版本号 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用opengl的核心模式
    
    // 空指针即为 0 NULL nullptr(可转换为任意类型的指针)
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); // 窗口大小
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // 设置窗口内容为当前线程的主内容

    // 调用opengl函数前，需要初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600); // 视口 左下角坐标 宽 高

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 注册回调函数 指示 每次窗口大小变换后，应该怎样操作
    
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清屏颜色
        glClear(GL_COLOR_BUFFER_BIT); // 设置 清屏颜色缓冲
        processInput(window);
        glfwSwapBuffers(window); // 双缓冲，前缓冲即窗口显示的像素值，后缓冲等所有的渲染进程结束后更新，之后交换前后缓冲，此举为了防止出现渲染闪烁等现象
        glfwPollEvents(); // 检查交互式事件是否被触发
    }

    glfwTerminate();

    return 0;
}