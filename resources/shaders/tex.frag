#version 460 core
in VS_OUT {
    vec2 texcoord;
} fs_in;

uniform sampler2D tex0; // Texture sampler uniform

out vec4 FragColor;

void main() {
    FragColor = texture(tex0, fs_in.texcoord); // Sample texture
}