// Camera.hpp
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <GLFW/glfw3.h> // For GLFWwindow
#include <cmath>        // For sin, cos, radians

class Camera {
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up; // camera local UP vector

    GLfloat camera_height = 0.0f;

    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat Roll;
    
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;

    Camera() : Position(0.0f, 0.0f, 0.0f), Front(0.0f, 0.0f, -1.0f), Right(1.0f, 0.0f, 0.0f), Up(0.0f, 1.0f, 0.0f)
    {
        // Default constructor initializes camera's position and orientation
        this->Yaw = -90.0f;
        this->Pitch = 0.0f;
        this->Roll = 0.0f;
        this->MovementSpeed = 1.0f;
        this->MouseSensitivity = 0.25f;
        this->updateCameraVectors();
    }

    Camera(glm::vec3 position) : Position(position)
    {
        // Constructor that takes a position argument
        this->Yaw = -90.0f;
        this->Pitch = 0.0f;
        this->Roll = 0.0f;
        this->MovementSpeed = 1.0f;
        this->MouseSensitivity = 0.25f;
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime) {
        glm::vec3 direction{0};
          
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            direction += Front;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            direction -= Front;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            direction -= Right;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += Right;
        
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            direction += Up;
        
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            direction -= Up;
        
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            this->Yaw -= MouseSensitivity * deltaTime * 125;
            this->updateCameraVectors();
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            this->Yaw += MouseSensitivity * deltaTime * 125;
            this->updateCameraVectors();

        return glm::normalize(direction) * MovementSpeed * deltaTime;
    }

    /* True if the movement is not zero or abs(nan)
    */
    bool ValidMovement(glm::vec3 movement) {
        return movement != glm::vec3(0) && !std::isnan(movement.x) && !std::isnan(movement.y) && !std::isnan(movement.z);
    }

    void UpdateCameraPosition(glm::vec3 movement) {
        if (ValidMovement(movement)) {
            this->Position += movement;
        }
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE) {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw += xoffset;
        this->Pitch += yoffset;

        if (constraintPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }

        this->updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

        this->Front = glm::normalize(front);
        this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }
};

#endif // CAMERA_HPP
