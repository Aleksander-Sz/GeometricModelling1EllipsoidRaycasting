#version 330 core

in vec2 uv;

uniform sampler2D frame;

out vec4 FragColor;

void main()
{
	vec2 coords = uv;
	coords += 1.0f;
	coords /= 2.0f;
	FragColor = texture(frame, coords);
	//FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
}