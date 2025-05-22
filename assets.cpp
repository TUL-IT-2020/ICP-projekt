#include "assets.hpp"

glm::vec3 json_to_vec3(const nlohmann::json& json_array) {
    if (!json_array.is_array() || json_array.size() != 3) {
        throw std::invalid_argument("Invalid JSON format for vec3. Expected an array of size 3.");
    }
    return glm::vec3(json_array[0].get<float>(), json_array[1].get<float>(), json_array[2].get<float>());
}

cv::Mat createCheckerboardTexture() {
    // Vytvoření 2x2 matice s 3 kanály (RGB) a 8bitovými hodnotami
    cv::Mat checkerboard(2, 2, CV_8UC3);

    // Nastavení barev pixelů (černá a bílá)
    checkerboard.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);   // Černá
    checkerboard.at<cv::Vec3b>(0, 1) = cv::Vec3b(255, 255, 255); // Bílá
    checkerboard.at<cv::Vec3b>(1, 0) = cv::Vec3b(255, 255, 255); // Bílá
    checkerboard.at<cv::Vec3b>(1, 1) = cv::Vec3b(0, 0, 0);   // Černá

    return checkerboard;
}


glm::mat4 computeBillboardMatrix(const glm::mat4& view_matrix, const glm::vec3& sprite_position) {
    glm::mat4 billboard_matrix = glm::mat4(1.0f);

    // Extrahujte rotační část z pohledové matice
    billboard_matrix[0] = glm::vec4(glm::normalize(glm::vec3(view_matrix[0])), 0.0f);
    billboard_matrix[1] = glm::vec4(glm::normalize(glm::vec3(view_matrix[1])), 0.0f);
    billboard_matrix[2] = glm::vec4(glm::normalize(glm::vec3(view_matrix[2])), 0.0f);

    // Nastavte pozici spritu
    billboard_matrix[3] = glm::vec4(sprite_position, 1.0f);

    return billboard_matrix;
}


bool aabb_contains_point(const glm::vec3& min, const glm::vec3& max, const glm::vec3& point) {
    return (point.x >= min.x && point.x <= max.x) &&
           (point.y >= min.y && point.y <= max.y) &&
           (point.z >= min.z && point.z <= max.z);
}

bool aabb_intersect(const glm::vec3& minA, const glm::vec3& maxA,
                    const glm::vec3& minB, const glm::vec3& maxB) {
    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}