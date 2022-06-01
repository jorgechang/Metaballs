#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform sampler2D scene;
uniform bool horizontal;

#define BLUR_RADIUS 4

// Without bilinear:
//const float offsets[BLUR_RADIUS] = float[](0., 1., 2., 3., 4., 5., 6., 7.);
//const float gaussianWeights[BLUR_RADIUS] = float[](
//        0.20947266, 0.18328857, 0.12219238, 0.06109619, 0.0222168, 0.0055542, 0.00085449, 0.00006104
//);

// This uses a bilinear texture sampling technique to reduce the number of pixel
// lookups required on the GPU. https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
const float offsets[BLUR_RADIUS] = float[](0., 1.4, 3.2666666986744803, 5.133333021257075);
const float gaussianWeights[BLUR_RADIUS] = float[](
        0.20947266, 0.30548095, 0.08331299, 0.00640869);

void main() {
    vec2 texelSize = 1.0 / textureSize(scene, 0).xy;
    fragColor = texture(scene, uv) * gaussianWeights[0];
    float weights = 0.;
    if (horizontal) {
        for (int i = 1; i < BLUR_RADIUS; i++) {
            float weight = gaussianWeights[i];
            fragColor += texture(scene, uv + vec2(texelSize.x * offsets[i], 0.)) * weight;
            fragColor += texture(scene, uv - vec2(texelSize.x * offsets[i], 0.)) * weight;
        }
    } else {
        for (int i = 1; i < BLUR_RADIUS; i++) {
            float weight = gaussianWeights[i];
            fragColor += texture(scene, uv + vec2(0., texelSize.y * offsets[i])) * weight;
            fragColor += texture(scene, uv - vec2(0., texelSize.y * offsets[i])) * weight;
        }
    }
}
