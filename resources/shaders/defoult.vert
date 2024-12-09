#version 460 core
in vec3 attribute_Position;

void main() {
    gl_Position = vec4(attribute_Position, 1.0);
}