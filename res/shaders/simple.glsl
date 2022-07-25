$SHADER_TYPE VERT

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
  
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_normal;
out vec3 v_fpos;

void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(aPos, 1.0);
    v_normal = mat3(transpose(inverse(u_model))) * aNormal;
    v_fpos = vec3(u_model * vec4(aPos, 1.0));
}

$SHADER_TYPE FRAG

#version 330 core
out vec4 FragColor;

struct mat_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform mat_t u_mat;

uniform vec3 u_camera;
uniform vec3 u_light;

in vec3 v_normal;
in vec3 v_fpos;

void main()
{
    // ambient
    vec3 ambient = 0.1 * u_mat.ambient;
  	
    // diffuse 
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_light - v_fpos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * u_mat.diffuse);
    
    // specular
    vec3 viewDir = normalize(u_camera - v_fpos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_mat.shininess);
    vec3 specular = (spec * u_mat.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}