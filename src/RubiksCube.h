#pragma once

#include <glm/glm.hpp>

#include <array>
#include <vector>

class RubiksCube
{
public:
    enum Axis
    {
        AxisX = 0,
        AxisY = 1,
        AxisZ = 2
    };

    struct CubeInstance
    {
        int id = -1;
        glm::ivec3 grid = glm::ivec3(0);
        glm::mat3 orientation = glm::mat3(1.0f);
        glm::vec3 manualTranslation = glm::vec3(0.0f);
        glm::mat3 manualRotation = glm::mat3(1.0f);
        glm::vec3 faceColor[6];
    };

    struct RotationState
    {
        bool active = false;
        Axis axis = AxisX;
        int layer = 0;
        int direction = 1;
        float angleDeg = 0.0f;
        float targetDeg = 90.0f;
    };

public:
    explicit RubiksCube(float spacing = 1.06f);

    void Initialize();
    void Update(float deltaTime);
    bool StartRotation(Axis axis, int layer, int direction, float degrees);
    bool IsRotating() const;

    glm::mat4 GetCubeModel(int id) const;
    glm::vec3 GetCubeCenterWorld(int id) const;
    void SetCubeCenterWorld(int id, const glm::vec3& center);
    void RotateCubeManual(int id, const glm::mat3& rotation);
    const glm::vec3* GetCubeFaceColors(int id) const;

    const std::vector<CubeInstance>& GetCubes() const { return m_Cubes; }
    const RotationState& GetRotationState() const { return m_Rotation; }

private:
    std::vector<CubeInstance> m_Cubes;
    int m_CubeIdAt[3][3][3];
    float m_Spacing = 1.06f;
    float m_CubeScale = 0.96f;
    RotationState m_Rotation;

private:
    bool IsCubeInLayer(const CubeInstance& cube) const;
    glm::mat3 RotationMatrix(Axis axis, float angleDeg) const;
    void ApplyCompletedRotation();
    void RebuildMapping();
};
