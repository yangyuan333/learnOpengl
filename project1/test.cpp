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
    glfwInit(); // ��ʼ��glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opengl�汾�� 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // opengl�ΰ汾�� 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ʹ��opengl�ĺ���ģʽ
    
    // ��ָ�뼴Ϊ 0 NULL nullptr(��ת��Ϊ�������͵�ָ��)
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); // ���ڴ�С
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // ���ô�������Ϊ��ǰ�̵߳�������

    // ����opengl����ǰ����Ҫ��ʼ��glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600); // �ӿ� ���½����� �� ��

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // ע��ص����� ָʾ ÿ�δ��ڴ�С�任��Ӧ����������
    
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // ����������ɫ
        glClear(GL_COLOR_BUFFER_BIT); // ���� ������ɫ����
        processInput(window);
        glfwSwapBuffers(window); // ˫���壬ǰ���弴������ʾ������ֵ���󻺳�����е���Ⱦ���̽�������£�֮�󽻻�ǰ�󻺳壬�˾�Ϊ�˷�ֹ������Ⱦ��˸������
        glfwPollEvents(); // ��齻��ʽ�¼��Ƿ񱻴���
    }

    glfwTerminate();

    return 0;
}