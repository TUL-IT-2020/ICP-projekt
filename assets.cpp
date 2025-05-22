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
