#include <Camera.h>

void Camera::SetOrthographic(float near, float far)
{
    m_Near = near;
    m_Far = far;

    // Rest Projection and View matrices
    m_Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near, far);
    UpdateView();
}

void Camera::SetPerspective(float fovDeg, float near, float far)
{
    m_FovDeg = fovDeg;
    m_Near = near;
    m_Far = far;
    m_Aspect = (m_Height > 0) ? static_cast<float>(m_Width) / static_cast<float>(m_Height) : 1.0f;
    m_Projection = glm::perspective(glm::radians(m_FovDeg), m_Aspect, m_Near, m_Far);
    UpdateView();
}

void Camera::SetSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    m_Aspect = (m_Height > 0) ? static_cast<float>(m_Width) / static_cast<float>(m_Height) : 1.0f;
}

void Camera::RotateOrbit(float deltaX, float deltaY)
{
    const float rotateSpeed = 0.2f;
    m_YawDeg += deltaX * rotateSpeed;
    m_PitchDeg += deltaY * rotateSpeed;
    m_PitchDeg = glm::clamp(m_PitchDeg, -89.0f, 89.0f);
    UpdateView();
}

void Camera::Pan(float deltaX, float deltaY)
{
    const float panSpeed = 0.005f * m_Distance;
    glm::vec3 right = GetRight();
    glm::vec3 up = GetUp();
    m_PanOffset += (-right * static_cast<float>(deltaX) + up * static_cast<float>(deltaY)) * panSpeed;
    UpdateView();
}

void Camera::Zoom(float delta)
{
    const float zoomSpeed = 0.5f;
    m_Distance -= static_cast<float>(delta) * zoomSpeed;
    m_Distance = glm::clamp(m_Distance, 2.0f, 25.0f);
    UpdateView();
}

glm::vec3 Camera::GetRight() const
{
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(m_YawDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot, glm::radians(m_PitchDeg), glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::normalize(glm::vec3(rot * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
}

glm::vec3 Camera::GetUp() const
{
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(m_YawDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot, glm::radians(m_PitchDeg), glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::normalize(glm::vec3(rot * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
}

glm::vec3 Camera::GetForward() const
{
    return glm::normalize(-glm::vec3(m_View[2]));
}

void Camera::UpdateView()
{
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(m_YawDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot, glm::radians(m_PitchDeg), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 target = m_PanOffset;
    glm::vec3 offset = glm::vec3(rot * glm::vec4(0.0f, 0.0f, m_Distance, 0.0f));
    m_Position = target + offset;
    m_Up = glm::normalize(glm::vec3(rot * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    m_Orientation = glm::normalize(target - m_Position);
    m_View = glm::lookAt(m_Position, target, m_Up);
}
