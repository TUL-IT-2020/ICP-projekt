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


    // original position
    glm::vec3 origin{};
    glm::vec3 orientation{};  //rotation by x,y,z axis, in radians
    glm::vec3 scale{};
    glm::mat4 local_model_matrix{}; //for complex transformations 

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

    ~Model() {
        // clear all meshes
        // for (auto & mesh : meshes) {
        //    mesh.clear();
        //}
    }

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; s=s0+v*dt
    }

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

		glm::mat4 model_matrix = local_model_matrix * s * rz * ry * rx * t * m_s * m_rz * m_ry * m_rx * m_off;
         
        // call draw() on mesh (all meshes)
        for (auto const& mesh : meshes) {
            mesh.draw(model_matrix);  // do not forget to implement draw() overload with glm::mat4 parameter
        }
    }
    
    void draw(glm::mat4 const & model_matrix) {
        for (auto const& mesh : meshes) {
            mesh.draw(local_model_matrix * model_matrix);  // do not forget to implement...
        }
    }
};

#endif  // MODEL_HPP
