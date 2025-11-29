attribute vec3 aPos;
attribute vec2 aTex;
uniform mat4 uMVP;
varying vec2 vTex;
void main(){
    vTex = aTex;
    gl_Position = uMVP * vec4(aPos,1.0);
}
