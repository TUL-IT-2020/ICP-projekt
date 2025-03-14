#ifndef MODEL_HPP
#define MODEL_HPP

#include <bits/stl_numeric.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <iterator>  // Include this header for std::begin and std::end
#include <string>
#include <vector>

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

    // Default constructor
    Model() = default;

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
    }

    Model(std::shared_ptr<Mesh> mesh) {
        meshes.push_back(*mesh);
    }

    // Copy constructor
    Model(const Model& other)
        : meshes(other.meshes),
          name(other.name),
          origin(other.origin),
          orientation(other.orientation) {
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
        // origin += glm::vec3(3,0,0) * delta_t; 
    }

    // call draw() on mesh (all meshes)
    void draw(glm::vec3 const& offset = glm::vec3(0.0),
              glm::vec3 const& rotation = glm::vec3(0.0f)) {
        for (auto& mesh : meshes) {
            mesh.draw(origin + offset, orientation + rotation);
        }
    }
};

#endif  // MODEL_HPP
