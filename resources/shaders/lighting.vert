#version 460 core

// Vertex attributes
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Matrices
uniform mat4 m_m, v_m, p_m;

// Outputs to the fragment shader
out VS_OUT {
    vec3 FragPos; // Fragment position in View Space
    vec3 N;       // Normal in View Space
    vec3 V;       // View vector in View Space
    vec2 texCoord;
} vs_out;

void main(void) {
    // Create Model-View matrix
    mat4 mv_m = v_m * m_m;

    // Calculate view-space coordinate (the fragment's position)
    vec4 P = mv_m * aPosition;
    vs_out.FragPos = P.xyz;

    // Calculate normal in view space
    vs_out.N = mat3(mv_m) * aNormal;

    // Calculate view vector (from fragment to camera)
    vs_out.V = -P.xyz;

    // Pass texture coordinates through
    vs_out.texCoord = aTexCoord;

    // Calculate the final clip-space position of the vertex
    gl_Position = p_m * P;
}