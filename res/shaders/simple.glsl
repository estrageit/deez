$SHADER_TYPE VERT

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
  
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_normal;

void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(aPos, 1.0);
    v_normal = mat3(transpose(inverse(u_model))) * aNormal;
}

$SHADER_TYPE FRAG

#version 330 core
out vec4 FragColor;
  
uniform vec3 u_color;

in vec3 v_normal;

void main()
{
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 normal = normalize(v_normal);
    
    float diffuse = max(dot(normal, light_dir), 0.0);
    FragColor = vec4(u_color * diffuse + u_color * 0.1, 1.0);
    //FragColor = vec4(u_color, 1.0);
} 