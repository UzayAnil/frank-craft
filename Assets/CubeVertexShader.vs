#version 330

uniform mat4 V;
uniform mat4 P;

in vec4 pos;
out vec4 texcoord;

void main() {
    texcoord = pos;
    gl_Position = P * V * vec4( pos.xyz, 1 );
}
