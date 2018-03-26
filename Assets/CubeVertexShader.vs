#version 330

uniform mat4 P;
uniform mat4 V;

in vec4 pos;

void main() {
    gl_Position = P * V * vec4( pos.xyz, 1 );
}
