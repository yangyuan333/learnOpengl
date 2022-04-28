#version 330 core
out vec4 FragColor;

struct ParallelLight
{
	vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform ParallelLight parallelLight;

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
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

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

    sampler2D touying;

};
uniform SpotLight spotLight;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};
uniform Material material;

uniform vec3 viewPos;

in vec2 TexCoord;
in vec3 worldPoint;
in vec3 worldNormal;

vec3 CalParallelLight(ParallelLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal,lightDir),0.0);
    vec3 halfvec = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfvec), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalPointLight(PointLight light, vec3 normal, vec3 frugpoint, vec3 viewDir)
{
    vec3 lightDir = light.position - frugpoint;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);

    float diff = max(dot(normal,lightDir),0.0);
    vec3 halfvec = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfvec), 0.0), material.shininess);

    vec3 ambient = attenuation * light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = attenuation * light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = attenuation * light.specular * spec * vec3(texture(material.specular, TexCoord));
    
    return (ambient+diffuse+specular);
}

vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 frugpoint, vec3 viewDir)
{
    vec3 lightDir = light.position - frugpoint;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 halfvec = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfvec), 0.0), material.shininess);

    float theta = dot(lightDir, normalize(-light.direction));

    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(mix(texture(material.diffuse, TexCoord),texture(spotLight.touying, TexCoord),0.2f));
    vec3 diffuse = light.diffuse * diff * vec3(mix(texture(material.diffuse, TexCoord),texture(spotLight.touying, TexCoord),0.2f));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);


}


void main()
{
    vec3 viewDir = normalize(viewPos-worldPoint);
    vec3 norm = normalize(worldNormal);
    vec3 color = CalParallelLight(parallelLight,norm,viewDir);
    // for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    // {
    //     color += CalPointLight(pointLights[i],norm,worldPoint,viewDir);
    // }
    color += CalSpotLight(spotLight,norm,worldPoint,viewDir);

    FragColor = vec4(color, 1.0);

    // FragColor = texture(material.specular,TexCoord);

}