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
    float fhi;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

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
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalPointLight(PointLight light, vec3 normal, vec3 frugpoint, vec3 viewDirection)
{
    vec3 lightDir = frugpoint - light.position;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float attenuation = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);

    vec3 ambient = attenuation * light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = attenuation * light.diffuse * max(dot(normal, -lightDir), 0.0) * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = attenuation * light.specular * pow(max(dot(normalize(-viewDirection-lightDir),normal),0.0),material.shininess) * vec3(texture(material.specular, TexCoord));
    
    return (ambient+diffuse+specular);
}

vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 frugpoint, vec3 viewDirection)
{
    vec3 lightDir = frugpoint - light.position;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    float sita = dot(lightDir,light.direction);

    if (sita < light.fhi)
    {
        sita = clamp(sita,light.fhi,1.0);
        float sigma = (1.0-light.fhi)/3.0;
        float attenuation = exp(-(sita-1.0)*(sita-1.0)/(2*sigma*sigma));

        float attenuation1 = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);

        vec3 ambient = attenuation1 * light.ambient * vec3(texture(material.diffuse, TexCoord));
        vec3 diffuse = attenuation1 * light.diffuse * max(dot(normal, -lightDir), 0.0) * vec3(texture(material.diffuse, TexCoord));
        vec3 specular = attenuation1 * light.specular * pow(max(dot(normalize(-viewDirection-lightDir),normal),0.0),material.shininess) * vec3(texture(material.specular, TexCoord));

        return (attenuation*(ambient+diffuse+specular));
    }
    else
    {
        float attenuation1 = 1/(light.constant + light.linear*distance + light.quadratic*distance*distance);
        return vec3(attenuation1 * light.ambient * vec3(texture(material.diffuse, TexCoord)));
    }
}


void main()
{
    vec3 viewDir = normalize(viewPos-worldPoint);
    vec3 norm = normalize(worldNormal);
    vec3 color = CalParallelLight(parallelLight,norm,viewDir);
    // for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    // {
    //     color += CalPointLight(pointLights[i],worldNormal,worldPoint,viewDir);
    // }
    // color += CalSpotLight(spotLight,worldNormal,worldPoint,viewDir);

    FragColor = vec4(color, 1.0);

    // FragColor = texture(material.specular,TexCoord);

}