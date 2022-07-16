$SHADER_TYPE VERT

#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aCol; 
  
out vec4 vertexColor; 

uniform mat4 u_model;

void main()
{
    gl_Position = u_model * vec4(aPos, 1.0);
    vertexColor = vec4(aCol, 1.0); 
}

$SHADER_TYPE FRAG

#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor;

uniform float u_time;

void main()
{
    float fixed_sin = sin(u_time) / 2 + 0.5;
    FragColor = vertexColor * fixed_sin;
} 