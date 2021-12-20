#version 100
#define TAU 6.28318530718
precision mediump float;
const vec2 sunPosition = vec2(0.2,0.4);

const vec2 iResolution = vec2(800,600);

const vec3 BackColor	= vec3(0.0, 0.4, 0.58);
const vec3 CloudColor	= vec3(0.18,0.70,0.87);

void main(){
    //vec2 pos = gl_FragCoord.xy/ iResolution.yy;
    //vec3 canvas = BackColor;
    //float dist_squared = dot(sunPosition,sunPosition);
    //float circle= 0.0;
    //circle = step(1.0, dist_squared);
    //circle = smoothstep(0.1,0.103,circle);
    //canvas = mix(BackColor,CloudColor,circle);
    //gl_FragColor = vec4(canvas,1.0);
    vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
    float dist_squared = dot(pos, pos);
    gl_FragColor = mix(vec4(.90, .90, .90, 1.0), vec4(.20, .20, .40, 1.0),
                         step(400.0, dist_squared));
}
