$SHADER_TYPE VERT

#version 330 core
layout (location = 0) in vec3 aPos; 

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(aPos, 1.0);
}

$SHADER_TYPE FRAG

#version 330 core
out vec4 FragColor;
  
void main()
{
    FragColor = vec4(1,1,1,1);
} 