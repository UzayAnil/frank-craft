#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
in vec3 pos;

void main() {
    gl_Position = P * V * M * vec4(pos, 1.0);
}
