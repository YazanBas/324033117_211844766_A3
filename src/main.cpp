#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <Camera.h>
#include <RubiksCube.h>

#include <algorithm>
#include <iostream>
#include <string>

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
const float near = 0.1f;
const float far = 100.0f;

/* Cube vertices: positions, colors, texCoords, faceId */
float vertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  4.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  4.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  4.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  4.0f,
    // Back face
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  5.0f,
    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  5.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  5.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  5.0f,
    // Left face
    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  1.0f,
    // Right face
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  0.0f,
    // Top face
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  2.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  2.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  2.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  2.0f,
    // Bottom face
    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  3.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  3.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  3.0f,
    -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  3.0f
};

unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,       // Front
    4, 5, 6, 6, 7, 4,       // Back
    8, 9, 10, 10, 11, 8,    // Left
    12, 13, 14, 14, 15, 12, // Right
    16, 17, 18, 18, 19, 16, // Top
    20, 21, 22, 22, 23, 20  // Bottom
};

struct AppState
{
    Camera* camera = nullptr;
    RubiksCube* rubiks = nullptr;
    Shader* shader = nullptr;
    VertexArray* va = nullptr;
    IndexBuffer* ib = nullptr;
    Texture* texture = nullptr;
    bool pickingMode = false;
    int selectedCubeId = -1;
    float pickDepth = 1.0f;
    bool leftDown = false;
    bool rightDown = false;
    glm::vec2 lastMouse = glm::vec2(0.0f);
    bool rotateClockwise = true;
    int turnAngle = 90;
    glm::vec3 dragOffset = glm::vec3(0.0f);
};

static glm::vec4 EncodeIdColor(int id)
{
    int idx = id + 1;
    int r = (idx & 0xFF);
    int g = (idx >> 8) & 0xFF;
    int b = (idx >> 16) & 0xFF;
    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

static int DecodeIdColor(unsigned char r, unsigned char g, unsigned char b)
{
    int idx = static_cast<int>(r) + (static_cast<int>(g) << 8) + (static_cast<int>(b) << 16);
    return idx == 0 ? -1 : idx - 1;
}

static int DefaultDirectionForLayer(int layer)
{
    return layer == 1 ? -1 : 1;
}

static void TryStartRotation(AppState* state, RubiksCube::Axis axis, int layer)
{
    if (!state || state->rubiks->IsRotating())
    {
        return;
    }
    int dir = DefaultDirectionForLayer(layer);
    if (!state->rotateClockwise)
    {
        dir = -dir;
    }
    state->rubiks->StartRotation(axis, layer, dir, static_cast<float>(state->turnAngle));
}

static void PerformPicking(GLFWwindow* window, AppState* state, double mouseX, double mouseY)
{
    if (!state)
    {
        return;
    }

    int winWidth = 0;
    int winHeight = 0;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    float sx = winWidth > 0 ? static_cast<float>(fbWidth) / static_cast<float>(winWidth) : 1.0f;
    float sy = winHeight > 0 ? static_cast<float>(fbHeight) / static_cast<float>(winHeight) : 1.0f;

    float mouseXFB = static_cast<float>(mouseX) * sx;
    float mouseYFB = static_cast<float>(mouseY) * sy;
    float mouseYGL = static_cast<float>(fbHeight) - mouseYFB - 1.0f;

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    glm::mat4 view = state->camera->GetViewMatrix();
    glm::mat4 proj = state->camera->GetProjectionMatrix();

    state->shader->Bind();
    state->shader->SetUniform1i("u_Picking", 1);
    state->shader->SetUniform1i("u_Texture", 0);

    state->va->Bind();
    state->ib->Bind();

    for (const auto& cube : state->rubiks->GetCubes())
    {
        glm::mat4 model = state->rubiks->GetCubeModel(cube.id);
        glm::mat4 mvp = proj * view * model;
        glm::vec4 pickColor = EncodeIdColor(cube.id);
        state->shader->SetUniform4f("u_Color", pickColor);
        state->shader->SetUniformMat4f("u_MVP", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, state->ib->GetCount(), GL_UNSIGNED_INT, nullptr));
    }

    GLCall(glFinish());

    int px = static_cast<int>(mouseXFB);
    int py = static_cast<int>(mouseYGL);

    unsigned char pixel[4] = { 0, 0, 0, 0 };
    GLCall(glReadPixels(px, py, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel));

    float depth = 1.0f;
    GLCall(glReadPixels(px, py, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth));

    int id = DecodeIdColor(pixel[0], pixel[1], pixel[2]);
    state->selectedCubeId = id;
    state->pickDepth = depth;
}

static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if (!state)
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_P)
        {
            state->pickingMode = !state->pickingMode;
            state->selectedCubeId = -1;
            return;
        }
        if (key == GLFW_KEY_SPACE)
        {
            state->rotateClockwise = !state->rotateClockwise;
            return;
        }
        if (key == GLFW_KEY_Z)
        {
            state->turnAngle = std::max(90, state->turnAngle / 2);
            return;
        }
        if (key == GLFW_KEY_A)
        {
            state->turnAngle = std::min(180, state->turnAngle * 2);
            return;
        }

        if (key == GLFW_KEY_R)
        {
            TryStartRotation(state, RubiksCube::AxisX, 1);
        }
        else if (key == GLFW_KEY_L)
        {
            TryStartRotation(state, RubiksCube::AxisX, -1);
        }
        else if (key == GLFW_KEY_U)
        {
            TryStartRotation(state, RubiksCube::AxisY, 1);
        }
        else if (key == GLFW_KEY_D)
        {
            TryStartRotation(state, RubiksCube::AxisY, -1);
        }
        else if (key == GLFW_KEY_F)
        {
            TryStartRotation(state, RubiksCube::AxisZ, 1);
        }
        else if (key == GLFW_KEY_B)
        {
            TryStartRotation(state, RubiksCube::AxisZ, -1);
        }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        const float arrowStepDeg = 5.0f;
        const float rotateSpeed = 0.2f;
        float delta = arrowStepDeg / rotateSpeed;
        switch (key)
        {
            case GLFW_KEY_UP:
                state->camera->RotateOrbit(0.0f, -delta);
                break;
            case GLFW_KEY_DOWN:
                state->camera->RotateOrbit(0.0f, delta);
                break;
            case GLFW_KEY_LEFT:
                state->camera->RotateOrbit(-delta, 0.0f);
                break;
            case GLFW_KEY_RIGHT:
                state->camera->RotateOrbit(delta, 0.0f);
                break;
            default:
                break;
        }
    }
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if (!state)
    {
        return;
    }

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    state->lastMouse = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        state->leftDown = (action == GLFW_PRESS);
        if (action == GLFW_PRESS && state->pickingMode && !state->rubiks->IsRotating())
        {
            PerformPicking(window, state, mouseX, mouseY);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        state->rightDown = (action == GLFW_PRESS);
        if (action == GLFW_PRESS && state->pickingMode && state->selectedCubeId >= 0)
        {
            int winWidth = 0;
            int winHeight = 0;
            glfwGetWindowSize(window, &winWidth, &winHeight);

            int fbWidth = 0;
            int fbHeight = 0;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

            float sx = winWidth > 0 ? static_cast<float>(fbWidth) / static_cast<float>(winWidth) : 1.0f;
            float sy = winHeight > 0 ? static_cast<float>(fbHeight) / static_cast<float>(winHeight) : 1.0f;

            float mouseXFB = static_cast<float>(mouseX) * sx;
            float mouseYFB = static_cast<float>(mouseY) * sy;
            float mouseYGL = static_cast<float>(fbHeight) - mouseYFB - 1.0f;

            glm::vec3 worldPos = glm::unProject(
                glm::vec3(mouseXFB, mouseYGL, state->pickDepth),
                state->camera->GetViewMatrix(),
                state->camera->GetProjectionMatrix(),
                glm::vec4(0.0f, 0.0f, static_cast<float>(fbWidth), static_cast<float>(fbHeight))
            );
            glm::vec3 center = state->rubiks->GetCubeCenterWorld(state->selectedCubeId);
            state->dragOffset = center - worldPos;
        }
    }
}

static void CursorPosCallback(GLFWwindow* window, double mouseX, double mouseY)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if (!state)
    {
        return;
    }

    float dx = static_cast<float>(mouseX) - state->lastMouse.x;
    float dy = static_cast<float>(mouseY) - state->lastMouse.y;
    state->lastMouse = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));

    if (state->leftDown)
    {
        if (state->pickingMode && state->selectedCubeId >= 0)
        {
            const float rotateSpeed = 0.01f;
            glm::vec3 up = state->camera->GetUp();
            glm::vec3 right = state->camera->GetRight();
            glm::mat3 rotYaw = glm::mat3(glm::rotate(glm::mat4(1.0f), -dx * rotateSpeed, up));
            glm::mat3 rotPitch = glm::mat3(glm::rotate(glm::mat4(1.0f), -dy * rotateSpeed, right));
            state->rubiks->RotateCubeManual(state->selectedCubeId, rotPitch * rotYaw);
        }
        else if (!state->pickingMode)
        {
            state->camera->RotateOrbit(dx, dy);
        }
    }

    if (state->rightDown)
    {
        if (state->pickingMode && state->selectedCubeId >= 0)
        {
            int winWidth = 0;
            int winHeight = 0;
            glfwGetWindowSize(window, &winWidth, &winHeight);

            int fbWidth = 0;
            int fbHeight = 0;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

            float sx = winWidth > 0 ? static_cast<float>(fbWidth) / static_cast<float>(winWidth) : 1.0f;
            float sy = winHeight > 0 ? static_cast<float>(fbHeight) / static_cast<float>(winHeight) : 1.0f;

            float mouseXFB = static_cast<float>(mouseX) * sx;
            float mouseYFB = static_cast<float>(mouseY) * sy;
            float mouseYGL = static_cast<float>(fbHeight) - mouseYFB - 1.0f;

            glm::vec3 worldPos = glm::unProject(
                glm::vec3(mouseXFB, mouseYGL, state->pickDepth),
                state->camera->GetViewMatrix(),
                state->camera->GetProjectionMatrix(),
                glm::vec4(0.0f, 0.0f, static_cast<float>(fbWidth), static_cast<float>(fbHeight))
            );
            glm::vec3 center = worldPos + state->dragOffset;
            state->rubiks->SetCubeCenterWorld(state->selectedCubeId, center);
        }
        else if (!state->pickingMode)
        {
            state->camera->Pan(dx, -dy);
        }
    }
}

static void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY)
{
    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if (!state)
    {
        return;
    }
    state->camera->Zoom(scrollOffsetY);
}

static void FramebufferSizeCallback(GLFWwindow* window, int fbWidth, int fbHeight)
{
    GLCall(glViewport(0, 0, fbWidth, fbHeight));

    AppState* state = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    if (!state || !state->camera)
    {
        return;
    }

    state->camera->SetSize(fbWidth, fbHeight);
    state->camera->SetPerspective(45.0f, near, far);
}

int main(int argc, char* argv[])
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }
    
    /* Set OpenGL to Version 3.3.0 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Load GLAD so it configures OpenGL */
    gladLoadGL();

    /* Control frame rate */
    glfwSwapInterval(1);

    /* Print OpenGL version after completing initialization */
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* Set scope so that on window close the destructors will be called automatically */
    {
        /* Blend to fix images with transperancy */
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        /* Generate VAO, VBO, EBO and bind them */
        VertexArray va;
        VertexBuffer vb(vertices, sizeof(vertices));
        IndexBuffer ib(indices, sizeof(indices));

        VertexBufferLayout layout;
        layout.Push<float>(3);  // positions
        layout.Push<float>(3);  // colors
        layout.Push<float>(2);  // texCoords
        layout.Push<float>(1);  // faceId
        va.AddBuffer(vb, layout);

        /* Create texture */
        Texture texture("res/textures/plane.png");
        texture.Bind();
         
        /* Create shaders */
        Shader shader("res/shaders/basic.shader");
        shader.Bind();

        /* Unbind all to prevent accidentally modifying them */
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        /* Enables the Depth Buffer */
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));

        /* Create camera */
        Camera camera(width, height);
        camera.SetPerspective(45.0f, near, far);

        RubiksCube rubiks(1.06f);
        rubiks.Initialize();

        AppState appState;
        appState.camera = &camera;
        appState.rubiks = &rubiks;
        appState.shader = &shader;
        appState.va = &va;
        appState.ib = &ib;
        appState.texture = &texture;

        glfwSetWindowUserPointer(window, &appState);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetScrollCallback(window, ScrollCallback);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        double lastTime = glfwGetTime();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            double currentTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;

            rubiks.Update(deltaTime);

            /* Set white background color */
            GLCall(glClearColor(0.05f, 0.05f, 0.05f, 1.0f));

            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 proj = camera.GetProjectionMatrix();

            shader.Bind();
            shader.SetUniform1i("u_Picking", 0);
            shader.SetUniform1i("u_Texture", 0);

            va.Bind();
            ib.Bind();
            texture.Bind();

            glm::vec4 color = glm::vec4(1.0f);
            shader.SetUniform4f("u_Color", color);

            for (const auto& cube : rubiks.GetCubes())
            {
                const glm::vec3* faceColors = rubiks.GetCubeFaceColors(cube.id);
                if (faceColors)
                {
                    for (int i = 0; i < 6; ++i)
                    {
                        shader.SetUniform3f("u_FaceColors[" + std::to_string(i) + "]", faceColors[i]);
                    }
                }
                glm::mat4 model = rubiks.GetCubeModel(cube.id);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);
                GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
            }

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
