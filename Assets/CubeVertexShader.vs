#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec4 pos;
out vec4 texcoord;

void main() {
    texcoord = pos;
    gl_Position = P * V * M * vec4( pos.xyz, 1 );
}
