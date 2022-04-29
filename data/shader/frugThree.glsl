#version 330 core

struct ParallelLight
{
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight
{
    vec3 direction;
    vec3 position;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 worldPos;
in vec3 worldNormal;
in vec2 TexCoord;

uniform ParallelLight paralleLight;
#define pointLightNums 1
uniform PointLight pointLights[pointLightNums];
uniform SpotLight spotLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

uniform vec3 camPosition;

out vec4 FragColor;

vec3 CalParallelLight(ParallelLight light,vec3 normal,vec3 viewDir);
vec3 CalPointLight(PointLight light,vec3 normal,vec3 viewDir,vec3 frugpoint);
vec3 CalSpotLight(SpotLight light,vec3 normal,vec3 viewDir,vec3 frugpoint);

void main()
{
    vec3 viewDir = normalize(camPosition - worldPos);
    vec3 normal = normalize(worldNormal);

    FragColor = vec4(CalParallelLight(paralleLight,normal,viewDir),0.0);

    for (int i = 0; i < pointLightNums; i++)
    {
        FragColor += vec4(CalPointLight(pointLights[i],normal,viewDir,worldPos),0.0);
    }
    FragColor += vec4(CalSpotLight(spotLight,normal,viewDir,worldPos),0.0);
    FragColor.w = 1.0;
}

vec3 CalParallelLight(ParallelLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal,lightDir),0.0);
    vec3 halfvec = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfvec), 0.0), shininess);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalPointLight(PointLight light,vec3 normal,vec3 viewDir,vec3 frugpoint)
{
    vec3 lightDir = light.position - frugpoint;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);

    float diff = max(dot(normal,lightDir),0.0);
    vec3 halfvec = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfvec), 0.0), shininess);

    vec3 ambient = attenuation * light.ambient * vec3(texture(texture_diffuse1, TexCoord));
    vec3 diffuse = attenuation * light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = attenuation * light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    
    return (ambient+diffuse+specular);
}

vec3 CalSpotLight(SpotLight light,vec3 normal,vec3 viewDir,vec3 frugpoint)
{
    vec3 lightDir = light.position - frugpoint;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 halfvec = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfvec), 0.0), shininess);

    float theta = dot(lightDir, normalize(-light.direction));

    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}