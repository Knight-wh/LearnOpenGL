#version 330 core
out vec4 FragColor;

// in vec2 TexCoords;
in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform sampler2D screenTexture;

void main() {
    FragColor = texture(screenTexture, fs_in.TexCoords);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}