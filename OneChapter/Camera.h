#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include<Eigen/Eigen>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	Camera(Eigen::Vector3f position = { 0,0,0 }, Eigen::Vector3f up = { 0,1,0 }, float yaw = YAW, float pitch = PITCH) :Front({ 0,0,-1 }), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front({ 0.0f, 0.0f, -1.0f }), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = { posX, posY, posZ };
		WorldUp = { upX, upY, upZ };
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	Eigen::Matrix4f GetViewMatrix()
	{
		/*
		* 根据相机当前的一级属性 获得 view 矩阵
		*/
		Eigen::Matrix4f viewMatrix;
		viewMatrix.block(0, 0, 1, 3) = Right;
		viewMatrix.block(1, 0, 1, 3) = Up;
		viewMatrix.block(2, 0, 1, 3) = -1 * Front;
		viewMatrix.row(4) = Eigen::Vector4f{ 0,0,0,1 };
		viewMatrix.block(0, 3, 3, 1) = -1 * Position;
		return viewMatrix;
	}
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		/*
		* 键盘移动控制
		* deltaTime保证设备处理速度不影响采样点的时间均匀性
		*/
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		/*
		* 根据鼠标的本次的移动位移 获得 俯仰角
		*/
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		/*
		* 根据滚轮影响 投影矩阵的 fov
		*/
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

public:
	// 属性
	Eigen::Vector3f Position;
	Eigen::Vector3f Front;
	Eigen::Vector3f Up;
	Eigen::Vector3f Right;
	Eigen::Vector3f WorldUp;
	// 控制
	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

private:
	void updateCameraVectors()
	{
		/*
		* 类似利用 二级属性 计算一级属性
		* Yaw + Pitch --> Front
		* WorldUp + Front --> Right
		* Right + Front --> Up
		*/
		Eigen::Vector3f front;
		front[1] = sin(Pitch);
		front[0] = cos(Pitch) * cos(Yaw);
		front[2] = cos(Pitch) * sin(Yaw);
		Front = front.normalized();
		Right = Front.cross(WorldUp).normalized();
		Up = Right.cross(Front).normalized();
	}

};
#endif