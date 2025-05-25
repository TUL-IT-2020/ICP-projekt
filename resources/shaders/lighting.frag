// Váš Fragment Shader (např. directional.frag)
#version 460 core

out vec4 FragColor;

// Struktura pro vlastnosti jednoho světla (dle skript)
// Upravíme ji, aby obsahovala intenzity přímo
struct LightProperties {
    bool isActive;
    vec3 position_view; // Očekáváme pozici ve view space
    vec3 ambient_intensity;
    vec3 diffuse_intensity;
    vec3 specular_intensity;
    // float spotCutOff; // Pro budoucí rozšíření
};

#define MAX_LIGHTS 10 // Musí se shodovat s C++
uniform LightProperties lights[MAX_LIGHTS]; // Pole všech potenciálních světel

// Vstupy z Vertex Shaderu
in VS_OUT {
    vec3 FragPos_view;
    vec3 N_view;
    vec2 texCoord;
} fs_in;

// Vlastnosti materiálu objektu (z C++)
uniform vec3 ambient_material;
uniform vec3 diffuse_material;
uniform vec3 specular_material;
uniform float specular_shininess; // Lesklost materiálu

// Textura
uniform sampler2D tex0;

void main(void) {
    vec3 N = normalize(fs_in.N_view);
    vec3 V = normalize(-fs_in.FragPos_view); // Vektor od fragmentu ke kameře (view space)
    
    vec3 texture_color = texture(tex0, fs_in.texCoord).rgb;
    vec3 accumulator = vec3(0.0); // Výsledná barva světla

    // Smyčka přes všechna potenciální světla (dle skript)
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].isActive) { // Pouze pokud je světlo aktivní
            // Vektor od fragmentu ke světlu (view space)
            vec3 L = normalize(lights[i].position_view - fs_in.FragPos_view); 
            // Vektor odrazu
            vec3 R = reflect(-L, N);

            // Ambientní složka pro toto světlo
            vec3 current_ambient = ambient_material * lights[i].ambient_intensity;

            // Difúzní složka pro toto světlo
            float diff_factor = max(dot(N, L), 0.0);
            vec3 current_diffuse = diff_factor * diffuse_material * lights[i].diffuse_intensity;

            // Spekulární složka pro toto světlo
            float spec_factor = pow(max(dot(R, V), 0.0), specular_shininess);
            vec3 current_specular = spec_factor * specular_material * lights[i].specular_intensity;
            
            accumulator += current_ambient + current_diffuse + current_specular;
        }
    }
    
    FragColor = vec4(accumulator * texture_color, 1.0);
}