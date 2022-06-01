#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform sampler2D hdrScene;
uniform sampler2D bloomscene;
uniform sampler2D dofscene;
uniform sampler2D normalizeddistance;

uniform float exposure;
uniform bool useHDR;
uniform bool useExposure;
uniform bool useGammaCorrection;
uniform bool useBloom;
uniform bool useDOF;

void main() {
    const float gamma = 2.2;  // Standard for computer monitors
    vec3 hdrColor = texture(hdrScene, uv).rgb;
    vec3 dofcolor = texture(dofscene, uv).rgb;
    vec3 bloomColor = texture(bloomscene, uv).rgb;
    float hdrdistance = texture(normalizeddistance,uv).r;
    float ldrdistance = 0.0;

    if (useDOF) {

        // Perform the DOF mixing before Tone Mapping
        ldrdistance = hdrdistance;
        hdrColor=mix(hdrColor,dofcolor,ldrdistance);
    }

    if (useBloom) {

        // Perform this addition before tone mapping so that bloom colors
        // are also mapped
        hdrColor += bloomColor;
    }
    vec3 outputColor;
    if (useHDR) {
        // Perform tone mapping
        if (useExposure) {
            // Exposure tone mapping
            outputColor = vec3(1.0) - exp(-hdrColor * exposure);

        } else {
            // Reinhard tone mapping
            outputColor = hdrColor / (hdrColor + vec3(1.0));
        }
    } else {
        outputColor = hdrColor;
    }

    if (useGammaCorrection) {
        // Gamma correction
        outputColor = pow(outputColor, vec3(1.0 / gamma));
    }

    fragColor = vec4(outputColor, 1.0);
}
