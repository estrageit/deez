$SHADER_TYPE VERT

#version 330 core
layout (location = 0) in vec3 aPos; 
  
out vec4 vertexColor; 

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(1, 0, 0, 1.0); 
}

$SHADER_TYPE FRAG

#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor;

void main()
{
    FragColor = vertexColor;
} 