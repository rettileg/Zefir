#ifndef SHADERPACK_H
#define SHADERPACK_H

// UI Vertex Shader
static const char* ui_vert_src = R"(
#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform vec2 uSize;

out vec2 TexCoord;
out vec2 FragPos;
out vec2 FragSize;

void main() {
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
    FragPos = aTexCoord * uSize;
    FragSize = uSize;
}
)";

// UI Fragment Shader
static const char* ui_frag_src = R"(
#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 FragPos;
in vec2 FragSize;

uniform vec4 uColor;
uniform float uRadius;
uniform int uStyle;
uniform float uBorderWidth;
uniform vec4 uBorderColor;
uniform vec4 uHighlightColor;
uniform vec4 uShadowColor;

float roundedBoxSDF(vec2 center, vec2 size, float radius) {
    vec2 d = abs(center) - (size * 0.5 - radius);
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main() {
    vec2 center = FragPos - FragSize * 0.5;
    float dist = roundedBoxSDF(center, FragSize, uRadius);
    float innerDist = roundedBoxSDF(center, FragSize - uBorderWidth * 2.0, uRadius - uBorderWidth);
    
    float softEdge = 1.0;
    float outerAlpha = 1.0 - smoothstep(-softEdge, softEdge, dist);
    float innerAlpha = 1.0 - smoothstep(-softEdge, softEdge, innerDist);
    
    if (outerAlpha < 0.01) discard;
    
    vec4 bgColor = uColor;
    vec4 borderColor = uBorderColor;
    float borderMask = outerAlpha - innerAlpha;
    
    if (uStyle == 1) {
        vec2 shadowCenter = center - vec2(2.0, -2.0);
        float shadowDist = roundedBoxSDF(shadowCenter, FragSize, uRadius);
        float shadowAlpha = 1.0 - smoothstep(-softEdge, softEdge + 1.0, shadowDist);
        
        vec2 highlightCenter = center - vec2(-2.0, 2.0);
        float highlightDist = roundedBoxSDF(highlightCenter, FragSize, uRadius);
        float highlightAlpha = 1.0 - smoothstep(-softEdge, softEdge + 1.0, highlightDist);
        
        vec4 result = bgColor;
        result = mix(result, borderColor, borderMask);
        result = mix(result, uShadowColor, shadowAlpha * 0.5);
        result = mix(result, uHighlightColor, highlightAlpha * 0.6);
        FragColor = vec4(result.rgb, outerAlpha);
    } else if (uStyle == 2) {
        vec2 innerShadowCenter = center - vec2(-1.5, 1.5);
        float innerShadowDist = roundedBoxSDF(innerShadowCenter, FragSize - uBorderWidth * 4.0, uRadius - uBorderWidth);
        float innerShadowAlpha = 1.0 - smoothstep(-softEdge, softEdge + 1.0, innerShadowDist);
        
        vec4 result = bgColor * 0.95;
        result = mix(result, borderColor * 0.8, borderMask);
        result = mix(result, uShadowColor, innerShadowAlpha * 0.3);
        FragColor = vec4(result.rgb, outerAlpha);
    } else {
        vec4 result = mix(bgColor, borderColor, borderMask);
        FragColor = vec4(result.rgb, outerAlpha);
    }
}
)";

// Quad Vertex Shader
static const char* quad_vert_src = R"(
#version 460 core
layout(location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

// Quad Fragment Shader
static const char* quad_frag_src = R"(
#version 460 core
out vec4 FragColor;
uniform vec4 uColor;
void main() {
    FragColor = uColor;
}
)";

#endif