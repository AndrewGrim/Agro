#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureUV;
layout (location = 2) in vec4 aColor;

out vec2 TexCoords;
out vec4 color;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
    TexCoords = textureUV;
    color = aColor;
} 