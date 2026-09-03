#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float time;
};

void main()
{
    vec2 uv = qt_TexCoord0 - 0.5;
    float radius = length(uv);
    float wave = sin(radius * 42.0 - time * 1.6) * 0.5 + 0.5;
    float gridX = smoothstep(0.96, 1.0, sin((uv.x + time * 0.012) * 72.0) * 0.5 + 0.5);
    float gridY = smoothstep(0.97, 1.0, sin(uv.y * 72.0) * 0.5 + 0.5);
    float halo = exp(-radius * 3.8) * 0.55;
    vec3 cyan = vec3(0.15, 0.93, 0.86);
    vec3 blue = vec3(0.18, 0.45, 0.95);
    vec3 color = mix(blue, cyan, wave) * (halo + (gridX + gridY) * 0.13);
    fragColor = vec4(color, (halo * 0.42 + wave * 0.035) * qt_Opacity);
}
