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

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
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
    float m_cameraPitch{0.0f};
    float m_cameraYaw{0.0f};
    glm::vec3 m_cameraPos{glm::vec3(0.0f, 0.0f, 3.0f)};
    glm::vec3 m_cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 m_cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};

    int m_width{WINDOW_WIDTH};
    int m_height{WINDOW_HEIGHT};
};

#endif // __CONTEXT_H__