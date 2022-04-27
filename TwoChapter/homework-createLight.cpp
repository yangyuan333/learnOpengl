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
	* Blinn Phong����ģ���ۺϲ���
	* ƽ�й�Դ��˥��ʽ���Դ���۹�Դ
	* Camera�ƶ�����
	* ͬһ���ӵĶ�����ʾ
	* ����������ͼ������������ͼ
	* ��ߣ��۹���Ч
	*/
	
	// init opengl
	MyOpengl Myopengl;
	// init shader
	Shader shader("../data/shader/vertexShaderModelMatrix.vs", "../data/shader/frugShaderModelMatrix.fs");
	
}