#version 330 core

layout(location = 0) in vec3 aPos;   // 버텍스 좌표 입력

uniform mat4 transform;              // 변환 행렬
uniform mat4 modelscale;
void main()
{
    gl_Position = transform  * vec4(aPos, 1.0);  // 변환 행렬을 적용한 최종 위치 계산
}
