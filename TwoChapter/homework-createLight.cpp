#include"../OneChapter/Camera.h"
#include"../OneChapter/shader.h"
#include<stb_image.h>

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

#include"myOpengl.h"

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
	MyOpengl Myopengl;
	// init shader
	Shader shader("../data/shader/vertexShaderModelMatrix.vs", "../data/shader/frugShaderModelMatrix.fs");
	
}