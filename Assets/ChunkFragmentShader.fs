#version 330

out vec4 fragColour;
uniform sampler2D tex;
in vec4 texcoord;

void main() {
    if ( texcoord.w < 0 ) {
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + texcoord.w)/16.0,
            (1-fract(texcoord.z))/16.0
            ));
    } else {
        fragColour = texture( tex, vec2(
            (fract(texcoord.x + texcoord.z) + texcoord.w)/16.0,
            (1-fract(texcoord.y))/16.0
            ));
    }
}
