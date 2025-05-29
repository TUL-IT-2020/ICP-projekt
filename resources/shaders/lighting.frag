#version 460 core

#define MAX_LIGHTS 20 // This MUST match MAX_LIGHTS in App.hpp

out vec4 FragColor;

// Struct for light properties, matching the C++ and uniform setup
struct Light {
    bool isActive;
    vec3 position; // NOTE: Received in WORLD space from C++
    vec3 ambient_intensity;
    vec3 diffuse_intensity;
    vec3 specular_intensity;
};

// Uniforms from C++
uniform Light lights[MAX_LIGHTS];
uniform mat4 v_m; // View matrix (to transform light positions)

// Material properties
uniform vec3 ambient_material;
uniform vec3 diffuse_material;
uniform vec3 specular_material;
uniform float specular_shinines;

// Texture
uniform sampler2D tex0;

// Input from vertex shader
in VS_OUT {
    vec3 FragPos; // Fragment position in View Space
    vec3 N;       // Normal in View Space
    vec3 V;       // View vector in View Space
    vec2 texCoord;
} fs_in;

void main(void) {
    // Normalize interpolated vectors from the vertex shader
    vec3 N = normalize(fs_in.N);
    vec3 V = normalize(fs_in.V);

    // Initialize total lighting components to zero
    vec3 totalAmbient = vec3(0.0);
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    // Loop through all possible lights and accumulate their effect
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].isActive) {
            // Transform the light's world position to view space
            vec3 lightPosView = (v_m * vec4(lights[i].position, 1.0)).xyz;

            // Calculate light vector (from fragment to light)
            vec3 L = normalize(lightPosView - fs_in.FragPos);

            // Calculate reflection vector
            vec3 R = reflect(-L, N);

            // Calculate distance and attenuation
            float distance = length(lightPosView - fs_in.FragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

            // --- Accumulate Components ---
            // Ambient
            totalAmbient += ambient_material * lights[i].ambient_intensity * attenuation;

            // Diffuse
            float diffFactor = max(dot(N, L), 0.0);
            totalDiffuse += diffFactor * diffuse_material * lights[i].diffuse_intensity * attenuation;

            // Specular
            float specFactor = pow(max(dot(R, V), 0.0), specular_shinines);
            totalSpecular += specFactor * specular_material * lights[i].specular_intensity * attenuation;
        }
    }

    // Get the base color from the texture
    vec3 textureColor = texture(tex0, fs_in.texCoord).rgb;

    // Combine lighting with the texture color
    vec3 finalColor = (totalAmbient + totalDiffuse) * textureColor + totalSpecular;

    FragColor = vec4(finalColor, 1.0);
}