#version 330 core

uniform vec3 color;

in vec2 uv;

out vec4 FragColor;

void main()
{
	FragColor = vec4((uv + 1.0f) / 2.0f, 0.0f, 1.0f);
}