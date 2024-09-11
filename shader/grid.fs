#version 330 core

out vec4 FragColor;                  // 최종 출력 색상

uniform vec3 gridColor;              // 그리드 색상

void main()
{
    FragColor = vec4(gridColor, 1.0);  // 그리드 색상 출력
}
