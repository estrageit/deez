THIS IS A MODIFIED VERSION OF CODE AVAILABLE AT https://learnopengl.com/
LICENSED UNDER https://creativecommons.org/licenses/by-nc/4.0/

$SHADER_TYPE VERT

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
  
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_normal;
out vec3 v_fpos;
out vec2 v_texcoord;

void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(aPos, 1.0);
    v_normal = mat3(transpose(inverse(u_model))) * aNormal;
    v_fpos = vec3(u_model * vec4(aPos, 1.0));
    v_texcoord = aTexCoord;
}

$SHADER_TYPE FRAG

#version 330 core
out vec4 FragColor;

struct mat_t {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct ldir_t {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct lpoint_t {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct lspot_t {
    vec3 position;

    vec3 direction;
    float cutoff;
    float outercutoff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define POINT_C $INTEGER 0
#if POINT_C > 0
uniform lpoint_t u_lpoint[POINT_C];
#endif

#define SPOT_C $INTEGER 1
#if SPOT_C > 0
uniform lspot_t u_lspot[SPOT_C];
#endif

uniform ldir_t u_ldir;

uniform mat_t u_mat;

uniform vec3 u_camera;

in vec3 v_normal;
in vec3 v_fpos;
in vec2 v_texcoord;

vec3 CalcDirLight(ldir_t light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_mat.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(u_mat.diffuse, v_texcoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_mat.diffuse, v_texcoord));
    vec3 specular = light.specular * spec * vec3(texture(u_mat.specular, v_texcoord));
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(lpoint_t light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_mat.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(u_mat.diffuse, v_texcoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_mat.diffuse, v_texcoord));
    vec3 specular = light.specular * spec * vec3(texture(u_mat.specular, v_texcoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(lspot_t light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_mat.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(u_mat.diffuse, v_texcoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(u_mat.diffuse, v_texcoord));
    vec3 specular = light.specular * spec * vec3(texture(u_mat.specular, v_texcoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outercutoff;
    float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);

    diffuse  *= intensity;
    specular *= intensity;
    return (ambient + diffuse + specular);
}

void main(){
    vec3 norm = normalize(v_normal);
    vec3 viewDir = normalize(u_camera - v_fpos);

    vec3 result = CalcDirLight(u_ldir, norm, viewDir);
#if POINT_C > 0
    for(int i = 0; i < POINT_C; i++){
        result += CalcPointLight(u_lpoint[i], norm, v_fpos, viewDir);
    }
#endif
#if SPOT_C > 0
    for(int i = 0; i < SPOT_C; i++){ 
        result += CalcSpotLight(u_lspot[i], norm, v_fpos, viewDir);
    }
#endif
    FragColor = vec4(result, 1.0);
}