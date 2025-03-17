#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <vector>
#include <GL/glew.h> 
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

//vertex description
struct vertex {
    glm::vec3 position;
};

// color description
struct color {
    GLfloat r, g, b, a;
};


/* Generate string representation of glm::mat4
* Return mat converted to sequence of lines
* @param matrix: glm::mat4 to be converted to string
*/
inline std::string mat4_to_string(const glm::mat4& matrix) {
    std::string result = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result += std::to_string(matrix[i][j]) + " ";
        }
        result += "\n";
    }
    return result;
}

#endif // ASSETS_HPP