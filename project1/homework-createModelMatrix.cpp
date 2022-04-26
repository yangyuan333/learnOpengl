#include<stb_image.h>
#include"shader.h"
#include<glad/glad.h>
#include<glfw3.h>
#include<iostream>
#include<Eigen/Eigen>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif

#define MY_PI 3.1415926
int framebuffer_width = 800;
int framebuffer_height = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    framebuffer_width = width;
    framebuffer_height = height;
}

void processInput(GLFWwindow* window, float& weight)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

Eigen::Matrix4f get_model_matrix(float rotation_angle,Eigen::Vector3f translation)
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

    Shader myshader("../data/shader/vertexShaderModelMatrix.vs", "../data/shader/frugShaderModelMatrix.fs");

    // 矩形顶点
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // 十个矩形的全局位移
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

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // 自绑定起，后续的Buffer绑定配置 都相当于记录在了VAO上
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 纹理设置
    stbi_set_flip_vertically_on_load(true);
    int width1, height1, nrChannels1;
    auto tex1Data = stbi_load("../data/texture/container.jpg", &width1, &height1, &nrChannels1, 0);
    int width2, height2, nrChannels2;
    auto tex2Data = stbi_load("../data/texture/awesomeface.png", &width2, &height2, &nrChannels2, 0);
    unsigned int texture[2];
    glGenTextures(2, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, tex1Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex2Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    myshader.use();
    glUniform1i(glGetUniformLocation(myshader.ID, "ourTexture1"), 0);
    myshader.setInt("ourTexture2", 1);

    // 启动z-buffer
    glEnable(GL_DEPTH_TEST);

    // mvp矩阵
    Eigen::Matrix4f viewMatrix = get_view_matrix({ 0,0,3 });
    Eigen::Matrix4f proMatrix = get_projection_matrix(45.0f, (float)framebuffer_width / (float)framebuffer_height, 0.1f, 100.0f);
    Eigen::Matrix4f modelMatrix;

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)framebuffer_width / (float)framebuffer_height, 0.1f, 100.0f);

    float weight = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, weight);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myshader.use();

        auto i = 0;
        for (auto curP : cubePositions) {
            if (i % 3) {
                modelMatrix = get_model_matrix(
                    glfwGetTime()*50.0f*MY_PI/180.0f, curP
                );
            }
            else {
                modelMatrix = get_model_matrix(
                    20.0f * i * MY_PI / 180.0f, curP
                );
            }
            ++i;

            glUniformMatrix4fv(glGetUniformLocation(myshader.ID, "model"), 1, GL_FALSE, modelMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(myshader.ID, "view"), 1, GL_FALSE, viewMatrix.data());
            // glUniformMatrix4fv(glGetUniformLocation(myshader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

            glUniformMatrix4fv(glGetUniformLocation(myshader.ID, "projection"), 1, GL_FALSE, proMatrix.data());

            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(myshader.ID);

    glfwTerminate();
    return 0;

}