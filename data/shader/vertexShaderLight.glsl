#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 TexCoord;
out vec3 worldPoint;
out vec3 worldNormal;

void main()
{
	TexCoord = aTexCoord;
	vec4 temV = modelMatrix * vec4(aPos,1.0f);
	worldPoint = vec3(temV);
	gl_Position = projectionMatrix * viewMatrix * temV;
	worldNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;
}
