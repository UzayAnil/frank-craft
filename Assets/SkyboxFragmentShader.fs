#version 330

uniform samplerCube tex;

in vec3 texcoord;
out vec4 fragColour;

void main() {
    if ( texcoord.x == texcoord.y && texcoord.x == texcoord.z )
        fragColour = vec4(1.0, 0.7, 0.7, 0.7);
    else
        fragColour = texture( tex, texcoord );
}
