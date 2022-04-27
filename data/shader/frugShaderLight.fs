#version 330 core

struct ParallelLight
{
	vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}
uniform ParallelLight parallelLight;

struct PointLight
{
    vec3 position;
	vec3 direction;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight
{
    vec3 direction;
    vec3 position;
    float fhi;
}
uniform SpotLight spotLight;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 
uniform Material material;

in vec2 TexCoord;
in vec3 viewPoint;
in vec3 ViewNormal;

out vec4 FragColor;

void CalParallelLight(ParallelLight light, vec3 normal, vec3 viewDirection)
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * max(dot(normal, -lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * pow(max(dot(normalize(-viewDirection-lightDir),normal),0.0),material.shininess) * vec3(texture(material.specular, TexCoords));
    return (ambient+diffuse+specular);
}

void CalPointLight(PointLight)

void main()
{
    
}