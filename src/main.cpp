#include "context.h"
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLFW 콜백 함수
void OnFramebufferSizeChange(GLFWwindow *window, int width, int height)
{
    auto context = (Context *)glfwGetWindowUserPointer(window);
    if (context)
    {
        context->Reshape(width, height);
    }
}

void OnMouseButton(GLFWwindow *window, int button, int action, int modifier)
{
    // ImGui 입력 처리
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    // 사용자 정의 입력 처리
    auto context = (Context *)glfwGetWindowUserPointer(window);
    if (context)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        context->MouseButton(button, action, x, y);
    }
}

void OnCursorPos(GLFWwindow *window, double x, double y)
{
    // ImGui의 마우스 입력 처리
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);

    // ImGui가 마우스를 캡처하지 않은 경우에만 사용자 정의 로직 실행
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        auto context = (Context *)glfwGetWindowUserPointer(window);
        if (context)
        {
            context->MouseMove(x, y);
        }
    }
}

int main()
{
    if (!glfwInit())
    {
        SPDLOG_ERROR("Failed to initialize GLFW");
        return -1;
    }

    // OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW 윈도우 생성
    GLFWwindow *window = glfwCreateWindow(1280, 720, "ImGui Test", nullptr, nullptr);
    if (!window)
    {
        SPDLOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD 초기화
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("Failed to initialize OpenGL context");
        return -1;
    }

    // OpenGL 버전 출력
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // ImGui 초기화
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Context 객체 생성
    auto context = Context::Create();
    if (!context)
    {
        SPDLOG_ERROR("Failed to create context");
        glfwTerminate();
        return -1;
    }

    // GLFW 콜백 함수 설정
    glfwSetWindowUserPointer(window, context.get());
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetCursorPosCallback(window, OnCursorPos);

    // 렌더링 루프
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // 이벤트 처리

        // ImGui 프레임 초기화
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI 렌더링 호출

        context->ProcessInput(window);
        context->Render(); // 여기서 ImGui UI 코드가 호출되어야 합니다.
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        glfwSwapBuffers(window);
    }

    // 리소스 정리
    context.reset();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}