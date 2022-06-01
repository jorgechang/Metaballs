#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform sampler2D tex;

uniform bool horizontal;
uniform bool finalpass;

//Performs Horizontal & Vertical Blurring in two consecutve passes

void main() {

    float weights = 0.0;
    const int supportWidth = 20;


    // Calculate the texelSize
    vec2 texelSize = 1.0 / textureSize(tex, 0).xy;

    fragColor = vec4(0.0);

    if(horizontal)
    {
        for (int i = -supportWidth; i <= supportWidth; i++) {
            float weight = (supportWidth + 1) - abs(i);
            // Add weight * sampleColor to fragColor, where
            //               sampleColor = tex sampled at uv + the offset
            //               in the x direction (you are moving over by "i" texels)
            fragColor += weight * texture(tex, uv + vec2(i*texelSize.x, 0));
            weights += weight;
        }


    }
    else
    {
        for (int i = -supportWidth; i <= supportWidth; i++) {
            float weight = (supportWidth + 1) - abs(i);
            //Add weight * sampleColor to fragColor, where
            //                sampleColor = tex sampled at uv + the offset
            //                in the y direction (you are moving vertically by "i" texels)
           fragColor += weight * texture(tex, uv + vec2(0, i * texelSize.y));
            weights += weight;

    }
    }



    fragColor /= weights;

    if(finalpass)
    {
        fragColor = fragColor;
    }

}
