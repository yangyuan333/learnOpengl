#version 330 core
out vec4 FragColor;

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

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

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
    vec3 lightDir = light.direction;

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * max(dot(normal, -lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * pow(max(dot(normalize(-viewDirection-lightDir),normal),0.0),material.shininess) * vec3(texture(material.specular, TexCoords));
    
    return (ambient+diffuse+specular);
}

void CalPointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDir = viewPoint - light.position;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);

    vec3 ambient = attenuation * light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = attenuation * light.diffuse * max(dot(normal, -lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = attenuation * light.specular * pow(max(dot(normalize(-viewDirection-lightDir),normal),0.0),material.shininess) * vec3(texture(material.specular, TexCoords));
    
    return (ambient+diffuse+specular);
}

void CalSpotLight(SpotLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDir = viewPoint - light.position;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float sita = dot(lightDir,light.direction);

    if (sita < light.fhi)
    {
        sita = clamp(sita,light.fhi,1.0);
        float sigma = (1.0-light.fhi)/3.0;
        float attenuation = exp(-(sita-1.0)*(sita-1.0)/(2*sigma*sigma));

        float attenuation1 = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);

        vec3 ambient = attenuation1 * light.ambient * vec3(texture(material.diffuse, TexCoords));
        vec3 diffuse = attenuation1 * light.diffuse * max(dot(normal, -lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
        vec3 specular = attenuation1 * light.specular * pow(max(dot(normalize(-viewDirection-lightDir),normal),0.0),material.shininess) * vec3(texture(material.specular, TexCoords));

        return (attenuation*(ambient+diffuse+specular));
    }
    else
    {
        float attenuation1 = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);
        return vec3(attenuation1 * light.ambient * vec3(texture(material.diffuse, TexCoords)));
    }
}


void main()
{
    vec3 viewDir = normalize(viewPoint);
    vec3 color = CalParallelLight(parallelLight,ViewNormal,viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        color += CalPointLight(pointLights[i],ViewNormal,viewDir);
    }
    color += CalSpotLight(spotLight,ViewNormal,viewDir);

    FragColor = vec4(result, 1.0);
}