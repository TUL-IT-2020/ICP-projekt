#pragma once

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 
#include <bits/stl_numeric.h>

#include "Vertex.h"
#include "Mesh.h"
#include "ShaderProgram.hpp"
#include "OBJloader.hpp"

class Model {
public:
    std::vector<Mesh> meshes;
    std::string name;
    glm::vec3 origin{};
    glm::vec3 orientation{};

    Model(const std::filesystem::path & filename, ShaderProgram & shader) {
        // load mesh (all meshes) of the model, load material of each mesh, load textures...
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its properties
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

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; s=s0+v*dt
    }
    
    void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f)) {
        // call draw() on mesh (all meshes)
        for (auto & mesh : meshes) {
            mesh.draw(origin+offset, orientation+rotation);
        }
    }
};

