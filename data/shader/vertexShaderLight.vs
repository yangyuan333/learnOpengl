#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec2 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normMatrix;

out vec2 TexCoord;
out vec3 viewPoint;
out vec3 ViewNormal;

void main()
{
	TexCoord = aTexCoord;
	viewPoint = viewMatrix * modelMatrix * aPos;
	gl_Position = projectionMatrix * viewPoint;
	ViewNormal = normMatrix * vec4(aNormal,0);
}
