#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>
#include <iostream>

#include "assets.hpp"
#include "ShaderProgram.hpp"
#include "Vertex.hpp"

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

    void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f)) {
        if (VAO == 0) {
            std::cerr << "VAO not initialized!\n";
            return;
        }
        
        shader.activate();
        glBindVertexArray(VAO);

        glDrawElements(primitive_type, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
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
