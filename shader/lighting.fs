#version 330 core
/*  텍스처있는 3d모델 전용
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D diffuseTexture;

void main() {
    vec3 texColor = texture(diffuseTexture, texCoord).xyz;
    fragColor = vec4(texColor, 1.0);  // 텍스처 색상만 사용하여 최종 색상 계산
}*/

in vec3 FragPos;   // Fragment의 위치
in vec3 normal;    // Fragment의 법선 벡터

// 출력 변수
out vec4 fragColor;

// 재질 관련 유니폼 (MTL 파일에서 불러온 정보)
uniform vec3 materialdiffuse;   // 재질의 디퓨즈 색상
uniform vec3 materialspecular;  // 재질의 스펙큘러 색상
uniform vec3 materialambient;   // 재질의 주변광 색상
uniform float materialshininess; // 재질의 반짝임 값

// 조명 정보 (광원 정보)
uniform vec3 lightDiffuse;      // 확산광 색상
uniform vec3 lightSpecular;     // 반사광 색상
uniform vec3 lightambient;
uniform vec3 lightPosition;

uniform vec3 viewPos;

void main()
{
    vec3 ambient = 0.5 * materialambient;

    // Diffuse (확산광 계산)
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosition - FragPos);  // 광원 방향 계산
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * materialdiffuse;  // 확산광

    vec3 specular = lightSpecular * materialspecular ;  // 반사광 강도를 줄임

 // 재질의 동적 변화: 조명 각도에 따라 재질이 변화하는 예시
    float angleFactor = max(dot(norm, lightDir), 0.0);     // 조명 각도에 따라 동적 변화
    vec3 dynamicMaterialDiffuse = mix(vec3(0.5, 0.5, 0.5), materialdiffuse, angleFactor);

    // 최종 색상 계산 (Ambient + Diffuse + Specular)
    vec3 finalColor = ambient + diffuse + specular;
    
    fragColor = vec4(finalColor, 1.0);  // 최종 색상 출력
}