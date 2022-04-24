#include<glad/glad.h>
#include<glfw3.h>
#include<iostream>
#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif

// ���ö�����ɫ��������
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// ����Ƭ����ɫ��������
const char* fragmentShaderSourceRed = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.2f, 0.2f, 1.0f);\n"
"}\n\0";

const char* fragmentShaderSourceBlue = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.2f, 0.2f, 1.0f, 1.0f);\n"
"}\n\0";

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
    glfwMakeContextCurrent(window); // ���ô�������Ϊ��ǰ�̵߳�������
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // ע��ص����� ָʾ ÿ�δ��ڴ�С�任��Ӧ����������
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ������ɫ��
    unsigned int vertexShader, fragmentShaderRed, fragmentShaderBlue;
    unsigned int shaderProgramRed, shaderProgramBlue;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentShaderBlue = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgramRed = glCreateProgram();
    shaderProgramBlue = glCreateProgram();

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // ���ö�����ɫ��
    glCompileShader(vertexShader); // ���붥����ɫ��
    glShaderSource(fragmentShaderRed, 1, &fragmentShaderSourceRed, NULL); // ����Ƭ����ɫ��
    glCompileShader(fragmentShaderRed); // ����Ƭ����ɫ��
    glShaderSource(fragmentShaderBlue, 1, &fragmentShaderSourceBlue, NULL); // ����Ƭ����ɫ��
    glCompileShader(fragmentShaderBlue); // ����Ƭ����ɫ��

    glAttachShader(shaderProgramRed, vertexShader);
    glAttachShader(shaderProgramRed, fragmentShaderRed);
    glLinkProgram(shaderProgramRed);
    glAttachShader(shaderProgramBlue, vertexShader);
    glAttachShader(shaderProgramBlue, fragmentShaderBlue);
    glLinkProgram(shaderProgramBlue);

    // ���ö�������
    float vertices[] = {
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };

    unsigned int VBO, VAO[2];
    glGenVertexArrays(2, VAO); // �԰��𣬺�����Buffer������ ���൱�ڼ�¼����VAO��
    glGenBuffers(2, &VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[1]);
    // ��������һֱ��
    // opengl�൱����һ��״̬����֮ǰ���ù��� ״̬ �󣬲���Ҫÿ�ζ�����
    //glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/2, (vertices+3*3), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgramRed);
        glBindVertexArray(VAO[0]); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(shaderProgramBlue);
        glBindVertexArray(VAO[1]); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgramRed);

    glfwTerminate();
    return 0;
}

