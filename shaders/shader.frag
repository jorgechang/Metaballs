#version 400

#define METABALL 0
#define PLANE 1
#define NO_INTERSECT 2
#define MAX_NUM_BALLS 7
#define MAX_NUM_LIGHTS 1

uniform float timeElapsed;
uniform float timeDelta;
uniform vec3 resolution;
uniform vec3 metaballBaseTintColor;

uniform int nLights;
uniform vec3 lightDirections[MAX_NUM_LIGHTS];
uniform vec3 lightColors[MAX_NUM_LIGHTS];

uniform bool useReflections;

//in vec2 inUV;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
layout (location = 2) out vec4 normalizeddepth;

struct Metaball
{
    vec3 pos;
    float f;
    vec3 color;
};

uniform int nBalls;
uniform Metaball balls[MAX_NUM_BALLS];

const float epsilon = 0.001;
vec2 e = vec2(epsilon, 0.0);

// Data structure for raymarching results
struct PrimitiveDist {
    float dist;
    int primitive;
    vec3 color;
};


PrimitiveDist metaballs(vec3 p) {
    float o = 0.0;
    vec3 oc = vec3(0.0);

    int num_balls = min(nBalls, MAX_NUM_BALLS);
    for (int i = 0 ; i < num_balls ; i++) {
        float influence = (balls[i].f / length(p - balls[i].pos));
        float d = ((i+4.0)/num_balls) - influence;
        o += d;
        oc = mix(oc, balls[i].color, influence);
    }
    return PrimitiveDist(o, METABALL, oc);
}

PrimitiveDist sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  float d = length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
  return PrimitiveDist(-d, PLANE, vec3(.0));
}


vec3 texCube( sampler2D sam, in vec3 p, in vec3 n )
{
    vec4 x = texture(sam, p.yz);
    vec4 y = texture(sam, p.xz);
    vec4 z = texture(sam, p.xy);
    vec4 o = x*abs(n.x) + y*abs(n.y) + z*abs(n.z);
    return vec3(o.x, o.y, o.z);
}

PrimitiveDist sdFloor(vec3 p) {
    return PrimitiveDist(p.y, PLANE, vec3(0.0));
}

PrimitiveDist map(vec3 p) {
    vec3 box_bound = vec3(5.22, 5.96, 7.44);
    PrimitiveDist container = sdBox(p, box_bound);
    PrimitiveDist mb = metaballs(p);

    if (mb.dist <= container.dist)
        return mb;
    else
        return container;
}

vec3 calcNormal(vec3 p) {
    vec3 n = vec3(0.0);
    n.x = map(p+e.xyy).dist - map(p-e.xyy).dist;
    n.y = map(p+e.yxy).dist - map(p-e.yxy).dist;
    n.z = map(p+e.yyx).dist - map(p-e.yyx).dist;
    return normalize(n);
}

PrimitiveDist raymarchImpl(vec3 ro, vec3 rd, int iterations) {
    float t = 0.001;
    float boundingDist = 50.0;
    float threshold = 0.001;

    for (int i = 0; i < iterations; i++) {

        vec3 pos = ro + (rd * t);
        PrimitiveDist obj = map(pos);

        if (obj.dist < threshold)
            return PrimitiveDist(t, obj.primitive, obj.color);

        t += obj.dist * 0.1;
        if (t > boundingDist) break;
    }

    return PrimitiveDist(-1.0, NO_INTERSECT, vec3(0.0));
}


PrimitiveDist raymarch(vec3 ro, vec3 rd) {
    return raymarchImpl(ro, rd, 1000);
}

PrimitiveDist raymarchReflect(vec3 ro, vec3 rd) {
    // Use fewer iterations for reflected rays
    return raymarchImpl(ro, rd, 100);
}

int nearestball(vec3 p)
{
    int idx = 0;
    float best_dist = 1e9;
    int num_balls = min(nBalls, MAX_NUM_BALLS);
    for (int i = 0 ; i < num_balls ; i++) {
        float len = length(p - balls[i].pos);
        if (len < best_dist){
            best_dist = len;
            idx = i;
        }
    }

    return idx;
}

vec3 metaball_color(vec3 p, float dist)
{
    vec3 oc = vec3(0.0);
    int num_balls = min(nBalls, MAX_NUM_BALLS);
    for (int i = 0 ; i < num_balls ; i++) {
        float d = length(balls[i].pos - p);
        if (d < dist)
        {
            float influence = (balls[i].f/ d);
            oc = mix(oc, balls[i].color, influence/(dist*0.1));
        }
    }
    return oc;
}

vec3 render(vec3 ro, vec3 rd, PrimitiveDist obj) {

    float t = obj.dist;
    int which = obj.primitive;
    vec3 col = obj.color;

    vec3 pos = ro + rd * t;

    // Normal vector
    vec3 nor = calcNormal(pos);

    for (int i = 0; i < nLights; i++) {
        vec3 lightDirection = normalize(lightDirections[i]);
        vec3 lightColor = lightColors[i];
        float ambient = 0.1;
        float diffuse = clamp(dot(nor, lightDirection), 0.0, 1.0);
        float shineness = 32.0;
        float specular = pow(clamp(dot(rd, reflect(lightDirection, nor)), 0.0, 1.0), 32.0);

        float darkness = 1.0;//shadow(pos, lightDirection, 18.0);
        // Applying the phong lighting model to the pixel.
        vec3 ambientColor = vec3(ambient * darkness);
        vec3 diffuseColor = diffuse * darkness * lightColor;
        vec3 specularColor = specular * darkness * lightColor;

        col += ambientColor + diffuseColor + specularColor;
    }

    vec3 material = vec3(0.0);
    if (which == PLANE) {
        material = vec3(0.0, 0.0, 0.0);
    } else if (which == METABALL) {
        material = metaballBaseTintColor;
    }

    // Blend the material color with the original color.
    col = mix(col, material, 0.5);

    return col;
}

vec3 render_box(vec3 ro, vec3 rd, PrimitiveDist obj)
{
    float t = obj.dist;
    int which = obj.primitive;
    vec3 col = obj.color;
    vec3 pos = ro + rd * t;
    vec3 nor = calcNormal(pos);

    // TODO: we could artificially exagerate bottom plane.
//    if (pos.y < -3)
//        return col + vec3(0.1);

    float ambient = 0.1;
    vec3 ambientColor = vec3(ambient);
    Metaball closest_ball = balls[nearestball(pos)];
    vec3 closest_vec = closest_ball.pos - pos;
    vec3 mc = metaball_color(pos, length(closest_vec)*1.1);
    col += ambientColor;
    col = mix(col, pow(mc, vec3(1.1))*0.3, 0.3);
    return col;

}

vec3 computeRayDirection(vec2 uv, vec3 target, vec3 look, vec3 up)
{
    float focalLength = 2.0;
    vec3 cameraForward = -look;
    vec3 cameraRight = normalize(cross(cameraForward, up));
    vec3 cameraUp = normalize(cross(cameraRight, cameraForward));
    return normalize(uv.x * cameraRight + uv.y * cameraUp + focalLength * cameraForward);
}

void main(){

    vec3 rayOrigin = vec3(0.0, 4.8, 7.0);
    float far = 35;
    float near = 0.5;
    vec4 raydistace = vec4(far);
    vec3 target = vec3(0.0);
    vec3 look = normalize(rayOrigin - target);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec2 uv = (gl_FragCoord.xy / resolution.xy) * 2.0 - 1.0;
    uv.x = uv.x *(resolution.x / resolution.y);
    vec3 rayDirection = computeRayDirection(uv, target, look, up);

    PrimitiveDist rayMarchResult = raymarch(rayOrigin, rayDirection);
    vec3 col = vec3(0.0);
    if (rayMarchResult.primitive != NO_INTERSECT) {
        if (useReflections)
        {
            if (rayMarchResult.primitive == METABALL)
                col = render(rayOrigin, rayDirection, rayMarchResult);
            if (rayMarchResult.primitive == PLANE)
                col = render_box(rayOrigin, rayDirection, rayMarchResult);
        } else {
            col = render(rayOrigin, rayDirection, rayMarchResult);
        }

        raydistace = vec4(rayMarchResult.dist);
    } else {
        //fragColor = vec4(sin(0.3*timeElapsed), cos(0.3*timeElapsed), 0.5, 1.0);
        col = vec3(0., 0., 0.);
        raydistace = vec4(far);
    }

    if (useReflections) {
        // Second pass for reflected ray
        vec3 refo = rayOrigin + rayDirection * rayMarchResult.dist;
        vec3 normal = calcNormal(refo);
        vec3 refd = reflect(rayDirection, normal);
        // Shift the origin away from the surface to avoid self-intersection
        PrimitiveDist refResult = raymarchReflect(refo + normal*0.01, refd);
        if (refResult.primitive == METABALL) {
            col += render(refo, refd, refResult);
        }
    }

    fragColor = vec4(col, 1.0);
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (rayMarchResult.primitive == METABALL)
    {
        brightColor = brightness > 0.1 ? fragColor : vec4(0.0, 0.0, 0.0, 1.0);
    }

    normalizeddepth = raydistace / far;

}
