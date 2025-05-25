// Váš Vertex Shader (např. directional.vert)
#version 460 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 m_m; // Model matrix
uniform mat4 v_m; // View matrix
uniform mat4 p_m; // Projection matrix

out VS_OUT {
    vec3 FragPos_view; // Pozice fragmentu v prostoru kamery
    vec3 N_view;       // Normála v prostoru kamery
    vec2 texCoord;
} vs_out;

void main(void) {
    mat4 mv_m = v_m * m_m; // ModelView matice

    vs_out.FragPos_view = vec3(mv_m * aPosition);
    // Správná transformace normály do view space
    vs_out.N_view = mat3(transpose(inverse(mv_m))) * aNormal; 
    vs_out.texCoord = aTexCoord;

    gl_Position = p_m * mv_m * aPosition;
}