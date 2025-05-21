#ifndef MODEL_HPP
#define MODEL_HPP

#include <numeric>

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>  // Pro glm::to_string
#include <iterator>  // Include this header for std::begin and std::end
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "Mesh.hpp"
#include "OBJloader.hpp"
#include "ShaderProgram.hpp"
#include "Vertex.hpp"
#include "assets.hpp"

class Model {
public:
    std::vector<Mesh> meshes;
    std::string name;
    glm::vec3 origin{};
    glm::vec3 orientation{};
    glm::vec3 scale{};
    glm::mat4 local_model_matrix{}; //for complex transformations 

    GLuint texture_id{0}; // texture id=0  means no texture
    bool isSprite = false;
    bool transparent = false;
    // for collectible objects
    bool collectible = false; 
    std::string collect_type = "";
    int value = 0; 
    // for enemy objects
    bool isEnemy = false;
    float radius = 0.5f;
    int health = 1; 

    // for light sources
    bool light_source = false;

    // parse json to model
    static Model parse_json_to_model(const nlohmann::json& model_data, Model& model,
                      std::unordered_map<std::string, Model> model_cache);

    // Default constructor
    Model() = default;

    void init_position() {
        this->origin = glm::vec3(0.0f);
        this->orientation = glm::vec3(0.0f);
        this->scale = glm::vec3(1.0f);
        this->local_model_matrix = glm::identity<glm::mat4>();
    }

    /* Model loader from json
     * Will update shader cache with new shader if not already present.
     * @param model_data: json data with model information
     * @param shader_cache: cache of shader programs
     * @return: Model object
     */
    Model(const nlohmann::json& model_data,
          std::unordered_map<std::string, ShaderProgram>& shader_cache) {
        // Načtení základních informací o modelu
        name = model_data["name"];
        std::string path = model_data["obj_path"];
        std::filesystem::path vertex_shader_path = model_data["vertex_shader_path"];
        std::filesystem::path fragment_shader_path = model_data["fragment_shader_path"];

        // Kontrola existence shaderů
        if (!std::filesystem::exists(vertex_shader_path) ||
            !std::filesystem::exists(fragment_shader_path)) {
            throw std::runtime_error("Shader file not found: " + vertex_shader_path.string() +
                                     " or " + fragment_shader_path.string());
        }

        // Kontrola existence modelového souboru
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("Model file not found: " + path);
        }

        // Načtení nebo vytvoření shaderu
        std::string shader_key = vertex_shader_path.string() + fragment_shader_path.string();
        if (shader_cache.find(shader_key) == shader_cache.end()) {
            shader_cache[shader_key] = ShaderProgram(vertex_shader_path, fragment_shader_path);
            std::cout << "Shader program ID: " << shader_cache[shader_key].getID()
                      << " compiled and cached." << std::endl;
        } else {
            std::cout << "Shader program ID: " << shader_cache[shader_key].getID()
                      << " loaded from cache." << std::endl;
        }

        // Inicializace modelu
        ShaderProgram& shader = shader_cache[shader_key];
        Model model(path, shader);
        model.name = name;
        *this = model;
    }

    Model(const std::filesystem::path& filename, ShaderProgram& shader) {
        // load mesh (all meshes) of the model, load material of each mesh, load textures...
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its
        // properties
        //    notice: you can load multiple meshes and place them to proper positions,
        //            multiple textures (with reusing) etc. to construct single complicated Model
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!loadOBJ(filename.c_str(), vertices, uvs, normals)) {
            throw std::runtime_error("Failed to load model from " + filename.string());
        }

        std::vector<Vertex> vertexData;
        for (size_t i = 0; i < vertices.size(); ++i) {
            Vertex vertex;
            vertex.Position = vertices[i];
            vertex.Normal = normals[i];
            vertex.TexCoords = uvs[i];
            vertexData.push_back(vertex);
        }

        std::vector<GLuint> indices(vertexData.size());
        std::iota(indices.begin(), indices.end(), 0);

        meshes.emplace_back(GL_TRIANGLES, shader, vertexData, indices, origin, orientation);
        init_position();
    }

    Model(std::shared_ptr<Mesh> mesh) {
        meshes.push_back(*mesh);
        init_position();
    }

    // Copy constructor
    Model(const Model& other): 
        meshes(other.meshes),
        name(other.name),
        origin(other.origin),
        orientation(other.orientation),
        scale(other.scale),
        texture_id(other.texture_id),
        local_model_matrix(other.local_model_matrix),
        isSprite(other.isSprite),
        transparent(other.transparent),
        collectible(other.collectible),
        collect_type(other.collect_type),
        value(other.value),
        light_source(other.light_source),
        isEnemy(other.isEnemy),
        radius(other.radius),
        health(other.health) {
    }

    ~Model() {
        // clear all meshes
        // for (auto & mesh : meshes) {
        //    mesh.clear();
        //}
    }

    /* update position etc. based on running time
     * e.g.: s=s0+v*dt
     * @param delta_t: time passed since last update
     */
    void update(const float delta_t) {
		glm::vec3 offset = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, delta_t, 0.0f);  // Rotace kolem osy Y
        glm::vec3 scale_change = glm::vec3(1.0f);
        local_model_matrix *=  complete_transformation(offset, rotation, scale_change);
    }

    // call draw() on mesh (all meshes)
    void draw(glm::vec3 const & offset = glm::vec3(0.0),
              glm::vec3 const & rotation = glm::vec3(0.0f),
              glm::vec3 const & scale_change = glm::vec3(1.0f) ) {

        // compute complete transformation
		glm::mat4 t = glm::translate(glm::mat4(1.0f), origin);
		glm::mat4 rx = glm::rotate(glm::mat4(1.0f), orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 ry = glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rz = glm::rotate(glm::mat4(1.0f), orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

		glm::mat4 m_off = glm::translate(glm::mat4(1.0f), offset);
		glm::mat4 m_rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 m_ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 m_rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 m_s = glm::scale(glm::mat4(1.0f), scale_change);

		glm::mat4 model_matrix = s * rz * ry * rx * t * m_s * m_rz * m_ry * m_rx * m_off;

        if (texture_id == 0) {
            std::cerr << "Error: Texture ID is 0! Texture not loaded properly." << std::endl;
        }
        // conect texture to shader
        if (texture_id != 0) {
            ShaderProgram &shader = meshes[0].shader;
            shader.activate();
    
            int texture_unit = 0;
            glActiveTexture(GL_TEXTURE0 + texture_unit);
            glBindTexture(GL_TEXTURE_2D, texture_id);
    
            int texUniformLocation = glGetUniformLocation(shader.getID(), "tex0");
            glUniform1i(texUniformLocation, texture_unit);
        }
        draw(model_matrix);
    }
    
    void draw(glm::mat4 const & model_matrix) {
        for (auto const& mesh : meshes) {
            mesh.draw(local_model_matrix * model_matrix);
        }
    }
};

GLuint textureInit(const std::filesystem::path& file_name);
glm::vec3 json_to_vec3(const nlohmann::json& json_array);
cv::Mat createCheckerboardTexture();
Model parse_json_to_model(const nlohmann::json& model_data, Model& model,
                         std::unordered_map<std::string, Model> model_cache);


#endif  // MODEL_HPP
