#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;

out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = VertexTexCoord;
    gl_Position = projection*view*model * vec4(VertexPosition,1.0);
}
