#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 TexCoord;

void main()
{
    worldPos = mat3(modelMatrix)*aPos;
    gl_Position = projectionMatrix*viewMatrix*vec4(worldPos,1.0);
    worldNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    TexCoord = aTexCoord;
}