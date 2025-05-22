#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <vector>
#include <GL/glew.h> 
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

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

/* compute complete transformation
 * @param origin: translation vector
 * @param rotation: rotation vector
 * @param scale_change: scale vector
 * @return: transformation matrix
 */
inline glm::mat4 complete_transformation(glm::vec3 const& origin = glm::vec3(0.0),
                                  glm::vec3 const& rotation = glm::vec3(0.0f),
                                  glm::vec3 const& scale_change = glm::vec3(1.0f)) {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), origin);
    glm::mat4 rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 s = glm::scale(glm::mat4(1.0f), scale_change);

    return s * rz * ry * rx * t;
}

glm::vec3 json_to_vec3(const nlohmann::json& json_array);
cv::Mat createCheckerboardTexture();

#endif // ASSETS_HPP