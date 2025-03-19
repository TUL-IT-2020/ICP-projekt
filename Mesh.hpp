#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>
#include <iostream>

#include "assets.hpp"
#include "Vertex.hpp"
#include "ShaderProgram.hpp"

class Mesh {
public:
    // mesh data
    glm::vec3 origin{};
    glm::vec3 orientation{};
    
    GLuint texture_id{0}; // texture id=0  means no texture
    GLenum primitive_type = GL_POINT;
    ShaderProgram &shader;
    
    // mesh material
    glm::vec4 ambient_material{1.0f}; //white, non-transparent 
    glm::vec4 diffuse_material{1.0f}; //white, non-transparent 
    glm::vec4 specular_material{1.0f}; //white, non-transparent
    float reflectivity{1.0f}; 
    
    // indirect (indexed) draw 
    Mesh() = delete;

    Mesh(GLenum primitive_type, ShaderProgram & shader, std::vector<Vertex> const & vertices, std::vector<GLuint> const & indices, glm::vec3 const & origin, glm::vec3 const & orientation, GLuint const texture_id = 0):
        primitive_type(primitive_type),
        shader(shader),
        vertices(vertices),
        indices(indices),
        origin(origin),
        orientation(orientation),
        texture_id(texture_id)
    {
        /*
        GLuint position_attribute_location = glGetAttribLocation(shader.getID(), "atrribute_position");
        glEnableVertexArrayAttrib(VAO, position_attribute_location);
        glVertexArrayAttribFormat(VAO, position_attribute_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
        // (GLuint vaobj, GLuint attribindex, GLuint bindingindex)
        glVertexArrayAttribBinding(VAO, position_attribute_location, 0);

        // Create and fill data
        glCreateBuffers(1, &VBO);
        glNamedBufferData(VBO, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Connect together
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        */
        
        ///*
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
        //*/
    }

    // Copy constructor
    Mesh(const Mesh& other) 
        : origin(other.origin),
          orientation(other.orientation),
          texture_id(other.texture_id),
          primitive_type(other.primitive_type),
          shader(other.shader),
          ambient_material(other.ambient_material),
          diffuse_material(other.diffuse_material),
          specular_material(other.specular_material),
          reflectivity(other.reflectivity),
          vertices(other.vertices),
          indices(other.indices),
          VBO(0),
          VAO(0),
          EBO(0)
    {
        // Generate and bind VAO, VBO, and EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }

    // Copy assignment operator
    Mesh& operator=(const Mesh& other) {
        if (this != &other) {
            origin = other.origin;
            orientation = other.orientation;
            texture_id = other.texture_id;
            primitive_type = other.primitive_type;
            shader = other.shader;
            ambient_material = other.ambient_material;
            diffuse_material = other.diffuse_material;
            specular_material = other.specular_material;
            reflectivity = other.reflectivity;
            vertices = other.vertices;
            indices = other.indices;

            // Generate and bind VAO, VBO, and EBO
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

            glBindVertexArray(0);
        }
        return *this;
    }

    void draw(glm::mat4 const & model_matrix) const {
        if (VAO == 0) {
            std::cerr << "VAO not initialized!\n";
            return;
        }
        
        shader.activate();
        
        // Set uniform matrices
        shader.setUniform("uM_m", model_matrix);
        
        glBindVertexArray(VAO);

        glDrawElements(primitive_type, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f)) {
        // Compute model matrix
        glm::mat4 m_off = glm::translate(glm::mat4(1.0f), offset);
		glm::mat4 m_rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 m_ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 m_rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model_matrix = m_rz * m_ry * m_rx * m_off;

        draw(model_matrix);        
    }

    void clear(void) {
        texture_id = 0;
        primitive_type = GL_POINT;
        vertices.clear();
        indices.clear();
        origin = glm::vec3(0.0f);
        orientation = glm::vec3(0.0f);

        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO != 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
    }

private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
    
    GLuint VBO{ 0 };
    GLuint VAO{ 0 };
    GLuint EBO{ 0 };
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

#endif // MESH_HPP
