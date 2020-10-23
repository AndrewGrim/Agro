#version 330 core
in vec2 TexCoords;
in vec4 color;

out vec4 fColor;

uniform sampler2D text;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    fColor = color * sampled;
}  