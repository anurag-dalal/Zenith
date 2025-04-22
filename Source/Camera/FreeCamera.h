#ifndef FREECAMERA_H
#define FREECAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class FreeCamera {
public:
    FreeCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = -90.0f, float pitch = 0.0f);
    
    glm::mat4 getViewMatrix() const;
    void processKeyboard(GLFWwindow* window, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    float getZoom() const { return zoom; }
    
    // Setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setFront(const glm::vec3& f) { front = f; }
    void setUp(const glm::vec3& u) { up = u; }
    void setYaw(float y) { yaw = y; updateCameraVectors(); }
    void setPitch(float p) { pitch = p; updateCameraVectors(); }
    void setZoom(float z) { zoom = z; }

private:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // Euler angles
    float yaw;
    float pitch;
    
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};

#endif // FREECAMERA_H