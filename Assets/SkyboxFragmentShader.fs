#version 330

uniform samplerCube tex;

in vec3 texcoord;
out vec4 fragColour;

void main() {
    fragColour = texture( tex, texcoord );
}
