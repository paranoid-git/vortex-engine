#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

uniform sampler2D tex;
uniform sampler2D normalMap;
uniform vec3 viewPos;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;
uniform SpotLight spotLight;
uniform bool useSpotLight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir   = normalize(-light.direction);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    return (light.ambient + diff * light.diffuse + spec * light.specular) * objectColor;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir   = normalize(light.position - fragPos);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float dist        = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    return (light.ambient + diff * light.diffuse + spec * light.specular) * objectColor * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir   = normalize(light.position - fragPos);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    return (light.ambient + diff * light.diffuse * intensity + spec * light.specular * intensity) * objectColor;
}
float calcShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0
                             || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(norm, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 objectColor = texture(tex, TexCoord).rgb;

    vec3 norm = texture(normalMap, TexCoord).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    norm = normalize(TBN * norm);

    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-dirLight.direction);

    float shadow = calcShadow(FragPosLightSpace, norm, lightDir);



    vec3 ambient  = dirLight.ambient * objectColor;
    vec3 diffuse  = max(dot(norm, lightDir), 0.0) * dirLight.diffuse * objectColor;
    vec3 viewRefl = reflect(-viewDir, norm);
    float spec    = pow(max(dot(viewDir, viewRefl), 0.0), 32.0);
    vec3 specular = spec * dirLight.specular * objectColor;

    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    for (int i = 0; i < numPointLights; i++)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir, objectColor);

    if (useSpotLight)
        result += calcSpotLight(spotLight, norm, FragPos, viewDir, objectColor);

    FragColor = vec4(result, 1.0);
}
