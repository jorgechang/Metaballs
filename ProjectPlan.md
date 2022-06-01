# Team Astro Globe Project Plan

## Primary Features

### Metaballs

Metaballs are “blobby” objects which merge as they move closer to one another, 
and split as they move further apart. Metaballs are isosurfaces and can be 
described by an implicit equation:

![equation](https://render.githubusercontent.com/render/math?math=F(p,%20m)%20=%20\frac{G(m)^2}{\text{distance}(m.\text{xyz},%20p)^2})

Where p is a point in space, m is a metaball instance, G(.) is the field for a 
given metaball. Metaballs are implemented by calculating a “field” effect that 
surrounding metaballs have on a point in space. To calculate said effect, we 
loop through all available metaballs in the scene and accumulate their 
influence on a given position input using the isosurface equation. This 
aggregate influence can then be used a distance for an SDF and/or as a weight 
to mix the final color. 

#### Resources

- [Practical tips on metaballs](http://www.geisswerks.com/ryan/BLOBS/blobs.html)
- [An introduction to metaballs](https://en.wikipedia.org/wiki/Metaballs)

### HDR and Bloom

High Dynamic Range will be implemented by storing color values in a floating 
point framebuffer (16-bit or 32-bit), preventing automatic clamping to 
(0.0, 1.0). In the fragment shader, the framebuffer will be sampled and 
Reinhard tone mapping will be employed to map the HDR color values to LDR for 
rendering to the display. We may also explore exposure tone mapping to find a 
tone mapping algorithm that produces the most aesthetically pleasing results 
for our metaballs scene.

Bloom will be implemented as a post-processing step with a blur filter. A 
fragment shader will be used to render colors into two framebuffers, one for 
all colors, and one for colors brighter than a certain threshold. A 
linearly-separable Gaussian blur filter (based on the Filter assignment) 
will be used to blur the bright colors, after which the fragment shader used 
for HDR will be adapted to blend the scene’s HDR texture and the blurred 
bright color texture.

#### Resources

- [An OpenGL tutorial on implementing HDR](https://learnopengl.com/Advanced-Lighting/HDR)
- [An OpenGL tutorial on implementing Bloom](https://learnopengl.com/Advanced-Lighting/Bloom)

### GPU Ray Marching

We’ll use ray marching to render the metaball scene geometry in real-time. 
Simply put, instead of calculating points of intersection as we did in 
raytracing, we detect points that are close to objects in the scene up to a 
predetermined threshold by stepping through a direction. We’ll base our 
implementation on the lab, with some modifications.

#### Resources

- [Tutorial on implementing ray marching in OpenGL](http://viclw17.github.io/2018/11/29/raymarching-algorithm/)
- [Ray Marching and Signed Distance Functions](http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/)
- [Examples of complex scenes rendered using ray marching](https://www.shadertoy.com/user/iq)

### Depth of Field

Depth of field will be implemented with two framebuffers. First, we will 
render the completely focused scene in a texture. Then, the second step would 
be to render an out-of-focus scene. As with Bloom, we will obtain the 
out-of-focus scene by passing a blur filter. 

These two textures will be the input of a depth of field shader where the 
mixing will occur. For this step, it is possible to create a mixture of 
out-of-focus and completely focused scenes depending on the distance of the 
objects to the camera.

#### Resources

- [Different DoF resources](https://casual-effects.blogspot.com/2013/09/the-skylanders-swap-force-depth-of.html)
- [Explanation of Bokeh effect](https://bartwronski.com/2014/04/07/bokeh-depth-of-field-going-insane-part-1/)

## Secondary Features

These features will be implemented once all primary features are completed.

### Skybox / Environment Map

An environment map will be used to render our metaballs against a more 
interesting backdrop. Our implementation will closely follow the lab.

### Dynamic Addition of Metaballs

Users will be able to interact with the scene to add new metaballs. UI buttons 
could allow users to add new metaballs, along with changing properties such as 
field area and radius.

## Program Flow

1. The scene is rendered in real-time on a fragment shader using ray marching 
to calculate the scene’s geometry, then shaded using a Phong model with 
additional stylization. At this stage, the scene geometry should be fully 
rendered, with all metaball interaction.
2. The first pass is rendered into a framebuffer, which will be sampled for 
processing with HDR, Bloom and Depth of Field.

## Division of Labor

The plan is to have one lead person for each of the outlined features, with 
input from other members as necessary. We’ll also share labor on setting up 
scene dynamics, aesthetics, and the final demo.

- **Bader**: Metaballs, ray marching, and UI for metaball addition
- **Daniel**: HDR, Bloom, and environment mapping
- **Jorge**: Depth of field, scene setup

## Plan of Action

1. Implement basic metaballs, including interactions, and GPU ray marching rendering.
2. Implement primary technical features:
    - HDR and Bloom
    - Depth of field
3. Implement environment map
4. Add the ability to dynamically add metaballs to the scene (via, e.g., a UI button).
5. Finalize stylization and scene dynamics

