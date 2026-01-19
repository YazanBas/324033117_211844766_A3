#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Debugger.h>
#include <Shader.h>

class Camera
{
    private:
        // View and Projection
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);

        // View matrix paramters
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Projection matrix parameters
        float m_Near = 0.1f;
        float m_Far = 100.0f;
        float m_FovDeg = 45.0f;
        float m_Aspect = 1.0f;
        int m_Width;
        int m_Height;

        // Orbit/pan camera parameters
        float m_Distance = 6.0f;
        float m_YawDeg = 0.0f;
        float m_PitchDeg = 0.0f;
        glm::vec3 m_PanOffset = glm::vec3(0.0f);
    public:
        // Prevent the camera from jumping around when first clicking left click
        double m_OldMouseX = 0.0;
        double m_OldMouseY = 0.0;
        double m_NewMouseX = 0.0;
        double m_NewMouseY = 0.0;
    public:
        Camera(int width, int height)
            : m_Width(width), m_Height(height)
        {
            m_Aspect = static_cast<float>(width) / static_cast<float>(height);
        };

        // Update Projection matrix for Orthographic mode
        void SetOrthographic(float near, float far);

        // Update Projection matrix for Perspective mode
        void SetPerspective(float fovDeg, float near, float far);
        void SetSize(int width, int height);

        void RotateOrbit(float deltaX, float deltaY);
        void Pan(float deltaX, float deltaY);
        void Zoom(float delta);

        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;
        glm::vec3 GetForward() const;

        inline glm::mat4 GetViewMatrix() const { return m_View; }
        inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }
    private:
        void UpdateView();
};
