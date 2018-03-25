#version 330

uniform mat4 V;
uniform mat4 P;

in vec3 pos;
out vec3 texcoord;

void main() {
    texcoord = pos;
    gl_Position = P * V * vec4( pos, 1 );
}
