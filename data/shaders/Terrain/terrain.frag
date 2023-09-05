#version 330 core
out vec4 FragColor;

in float height;

void main()
{
    float grey = ((height + 16.0f) / 64.0f);
    FragColor = vec4(grey, grey, grey, 1.0f);
    // FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}