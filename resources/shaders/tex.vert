#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

// RENAMED these uniforms to match the C++ code
uniform mat4 p_m = mat4(1.0f);
uniform mat4 m_m = mat4(1.0f);
uniform mat4 v_m = mat4(1.0f);

out VS_OUT {
    vec2 texcoord;
} vs_out;

void main() {
    // Outputs the positions/coordinates of all vertices
    gl_Position = p_m * v_m * m_m * vec4(aPos, 1.0f);
    
    vs_out.texcoord = aTex;
}