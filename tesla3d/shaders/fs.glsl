precision mediump float;
varying vec2 vTex;
uniform sampler2D uTex;
uniform vec3 uLightDir;
void main(){
    vec3 base = texture2D(uTex, vTex).rgb;
    float NdotL = max(dot(vec3(0.0,0.0,1.0), normalize(uLightDir)), 0.0);
    vec3 color = base * (0.2 + 0.8*NdotL);
    gl_FragColor = vec4(color, 1.0);
}

