#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

// out vec2 TexCoords;
out VS_OUT {
    vec2 TexCoords;
} vs_out;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.TexCoords = aTexCoords;
}