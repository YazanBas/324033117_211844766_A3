#include <RubiksCube.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

RubiksCube::RubiksCube(float spacing)
    : m_Spacing(spacing)
{
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 3; ++z)
            {
                m_CubeIdAt[x][y][z] = -1;
            }
        }
    }
}

void RubiksCube::Initialize()
{
    m_Cubes.clear();
    int id = 0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int z = -1; z <= 1; ++z)
            {
                CubeInstance cube;
                cube.id = id++;
                cube.grid = glm::ivec3(x, y, z);
                cube.orientation = glm::mat3(1.0f);
                cube.manualTranslation = glm::vec3(0.0f);
                cube.manualRotation = glm::mat3(1.0f);
                for (int i = 0; i < 6; ++i)
                {
                    cube.faceColor[i] = glm::vec3(0.0f);
                }
                if (x == 1)
                {
                    cube.faceColor[0] = glm::vec3(1.0f, 0.0f, 0.0f);
                }
                if (x == -1)
                {
                    cube.faceColor[1] = glm::vec3(1.0f, 0.5f, 0.0f);
                }
                if (y == 1)
                {
                    cube.faceColor[2] = glm::vec3(1.0f);
                }
                if (y == -1)
                {
                    cube.faceColor[3] = glm::vec3(1.0f, 1.0f, 0.0f);
                }
                if (z == 1)
                {
                    cube.faceColor[4] = glm::vec3(0.0f, 1.0f, 0.0f);
                }
                if (z == -1)
                {
                    cube.faceColor[5] = glm::vec3(0.0f, 0.0f, 1.0f);
                }
                m_Cubes.push_back(cube);
            }
        }
    }
    RebuildMapping();
}

void RubiksCube::Update(float deltaTime)
{
    if (!m_Rotation.active)
    {
        return;
    }

    const float rotationSpeed = 180.0f;
    m_Rotation.angleDeg += rotationSpeed * deltaTime;
    if (m_Rotation.angleDeg >= m_Rotation.targetDeg - 0.0001f)
    {
        m_Rotation.angleDeg = m_Rotation.targetDeg;
        ApplyCompletedRotation();
        m_Rotation.active = false;
    }
}

bool RubiksCube::StartRotation(Axis axis, int layer, int direction, float degrees)
{
    if (m_Rotation.active)
    {
        return false;
    }

    m_Rotation.active = true;
    m_Rotation.axis = axis;
    m_Rotation.layer = layer;
    m_Rotation.direction = direction >= 0 ? 1 : -1;
    m_Rotation.angleDeg = 0.0f;
    m_Rotation.targetDeg = degrees;
    return true;
}

bool RubiksCube::IsRotating() const
{
    return m_Rotation.active;
}

glm::mat4 RubiksCube::GetCubeModel(int id) const
{
    if (id < 0 || id >= static_cast<int>(m_Cubes.size()))
    {
        return glm::mat4(1.0f);
    }

    const CubeInstance& cube = m_Cubes[id];
    glm::vec3 pos = m_Spacing * glm::vec3(cube.grid) + cube.manualTranslation;
    glm::mat3 orient = cube.orientation;

    if (m_Rotation.active && IsCubeInLayer(cube))
    {
        glm::mat3 rot = RotationMatrix(m_Rotation.axis, m_Rotation.direction * m_Rotation.angleDeg);
        pos = rot * pos;
        orient = rot * orient;
    }

    glm::mat3 finalOrient = cube.manualRotation * orient;
    const float s = m_CubeScale;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)
        * glm::mat4(finalOrient)
        * glm::scale(glm::mat4(1.0f), glm::vec3(s));
    return model;
}

glm::vec3 RubiksCube::GetCubeCenterWorld(int id) const
{
    if (id < 0 || id >= static_cast<int>(m_Cubes.size()))
    {
        return glm::vec3(0.0f);
    }

    const CubeInstance& cube = m_Cubes[id];
    return m_Spacing * glm::vec3(cube.grid) + cube.manualTranslation;
}

void RubiksCube::SetCubeCenterWorld(int id, const glm::vec3& center)
{
    if (id < 0 || id >= static_cast<int>(m_Cubes.size()))
    {
        return;
    }

    CubeInstance& cube = m_Cubes[id];
    glm::vec3 basePos = m_Spacing * glm::vec3(cube.grid);
    cube.manualTranslation = center - basePos;
}

void RubiksCube::RotateCubeManual(int id, const glm::mat3& rotation)
{
    if (id < 0 || id >= static_cast<int>(m_Cubes.size()))
    {
        return;
    }

    CubeInstance& cube = m_Cubes[id];
    cube.manualRotation = rotation * cube.manualRotation;
}

const glm::vec3* RubiksCube::GetCubeFaceColors(int id) const
{
    if (id < 0 || id >= static_cast<int>(m_Cubes.size()))
    {
        return nullptr;
    }
    return m_Cubes[id].faceColor;
}

bool RubiksCube::IsCubeInLayer(const CubeInstance& cube) const
{
    switch (m_Rotation.axis)
    {
        case AxisX:
            return cube.grid.x == m_Rotation.layer;
        case AxisY:
            return cube.grid.y == m_Rotation.layer;
        case AxisZ:
            return cube.grid.z == m_Rotation.layer;
        default:
            return false;
    }
}

glm::mat3 RubiksCube::RotationMatrix(Axis axis, float angleDeg) const
{
    glm::vec3 axisVec(0.0f);
    switch (axis)
    {
        case AxisX:
            axisVec = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case AxisY:
            axisVec = glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case AxisZ:
            axisVec = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        default:
            axisVec = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
    }
    return glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), axisVec));
}

void RubiksCube::ApplyCompletedRotation()
{
    glm::mat3 rot = RotationMatrix(m_Rotation.axis, m_Rotation.direction * m_Rotation.targetDeg);
    for (CubeInstance& cube : m_Cubes)
    {
        if (!IsCubeInLayer(cube))
        {
            continue;
        }

        glm::vec3 gridPos = glm::vec3(cube.grid);
        glm::vec3 rotated = rot * gridPos;
        glm::ivec3 snapped = glm::ivec3(
            static_cast<int>(std::round(rotated.x)),
            static_cast<int>(std::round(rotated.y)),
            static_cast<int>(std::round(rotated.z))
        );

        cube.grid = snapped;
        cube.orientation = rot * cube.orientation;
    }

    RebuildMapping();
}

void RubiksCube::RebuildMapping()
{
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 3; ++z)
            {
                m_CubeIdAt[x][y][z] = -1;
            }
        }
    }

    for (const CubeInstance& cube : m_Cubes)
    {
        int xi = cube.grid.x + 1;
        int yi = cube.grid.y + 1;
        int zi = cube.grid.z + 1;
        if (xi >= 0 && xi < 3 && yi >= 0 && yi < 3 && zi >= 0 && zi < 3)
        {
            m_CubeIdAt[xi][yi][zi] = cube.id;
        }
    }
}
