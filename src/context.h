#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow *window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

    void CreateGrid(int gridSize, float boxLength);
    void RenderGrid(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &gridTransform);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_program1;
    ProgramUPtr m_simpleProgram;

    ModelUPtr m_model;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;

    //animation
    bool m_animation{true};

    // claer color
    glm::vec4 m_clearColor{glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)};

    // material parameter
    struct Material
    {
        TextureUPtr diffuse;
        TextureUPtr specular;
        float shininess{32.0f};
    };
    Material m_material;

    // camera parameter
    bool m_cameraControl{false};
    glm::vec2 m_prevMousePos{glm::vec2(0.0f)};
    float m_cameraPitch{-20.0f};
    float m_cameraYaw{0.0f};
    float j1{0.0f};
    float j2{0.0f};
    float j3{0.0f};
    float j4{0.0f};
    float j5{0.0f};
    float j6{0.0f};
    glm::vec3 targetPosition{0.0f, 0.0f, 0.0f};
    glm::vec3 m_cameraPos{glm::vec3(0.0f, 3.0f, 3.0f)};
    glm::vec3 m_cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 m_cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};

    int m_width{WINDOW_WIDTH};
    int m_height{WINDOW_HEIGHT};

    std::vector<float> m_gridVertices;
    std::vector<float> m_boldGridVertices;
    GLuint m_gridVAO, m_gridVBO;
    GLuint m_boldGridVAO, m_boldGridVBO;
};

#endif // __CONTEXT_H__