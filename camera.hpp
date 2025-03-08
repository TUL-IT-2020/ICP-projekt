// Camera.hpp
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <GLFW/glfw3.h> // For GLFWwindow
#include <cmath>        // For sin, cos, radians

class Camera
{
public:
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

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up; // camera local UP vector

    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat Roll;

    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime)
    {
        glm::vec3 direction{0};
          
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            direction += Front;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            direction -= Front;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            direction -= Right;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += Right;

        return glm::normalize(direction) * MovementSpeed * deltaTime;
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
    {
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
    void updateCameraVectors()
    {
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
