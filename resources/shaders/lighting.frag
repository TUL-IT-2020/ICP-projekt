// Vložte do directional.frag - toto je finální verze

#version 460 core

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient_intensity;
    vec3 diffuse_intensity;
    vec3 specular_intensity;
};

in VS_OUT {
    vec3 FragPos_view;
    vec3 N_view;
    vec2 texCoord;
} fs_in;

uniform vec3 ambient_material;
uniform vec3 diffuse_material;
uniform vec3 specular_material;
uniform float specular_shininess;
uniform sampler2D tex0;

#define MAX_LIGHTS 10
uniform Light lights[MAX_LIGHTS];
uniform int num_active_lights;

void main(void) {
    vec3 N = normalize(fs_in.N_view);
    vec3 V = normalize(-fs_in.FragPos_view);
    
    vec3 texture_color = texture(tex0, fs_in.texCoord).rgb;

    vec3 total_ambient = vec3(0.0);
    vec3 total_diffuse = vec3(0.0);
    vec3 total_specular = vec3(0.0);

    for (int i = 0; i < num_active_lights; i++) {
        vec3 L = normalize(lights[i].position - fs_in.FragPos_view);
        vec3 R = reflect(-L, N);

        // Ambientní složka
        total_ambient += ambient_material * lights[i].ambient_intensity;

        // Difúzní složka
        float diff_factor = max(dot(N, L), 0.0);
        total_diffuse += diff_factor * diffuse_material * lights[i].diffuse_intensity;

        // Spekulární složka
        float spec_factor = pow(max(dot(R, V), 0.0), specular_shininess);
        total_specular += spec_factor * specular_material * lights[i].specular_intensity;
    }
    
    // Finální zkombinování:
    // Ambientní a difúzní světlo ovlivňuje barvu textury.
    // Spekulární (odlesk) se přičítá navrch, aby byl většinou bílý.
    vec3 final_color = (total_ambient + total_diffuse) * texture_color + total_specular;
    
    FragColor = vec4(final_color, 1.0);
}