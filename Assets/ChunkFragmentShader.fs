#version 330

out vec4 fragColour;
uniform sampler2D tex;
in vec4 texcoord;

float ggEps = 0.001;

float fabs(float n) {
    return n > 0 ? n : -n;
}

void main() {
    // grass
    if ( fabs(texcoord.w - 1) < ggEps ) {
        int off = 0;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + off)/16.0,
            (1-fract(texcoord.z))/16.0
            ));
    } else if ( fabs(texcoord.w - 2) < ggEps ) {
        int off = 2;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + off)/16.0,
            (1-fract(texcoord.z))/16.0
            ));
    } else if ( fabs(texcoord.w - 3) < ggEps ) {
        int off = 3;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x + texcoord.z) + off)/16.0,
            (1-fract(texcoord.y))/16.0
            ));
    }

    // Dirt
    if ( fabs(texcoord.w - 4) < ggEps ) {
        int off = 2;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + off)/16.0,
            (1-fract(texcoord.z))/16.0
            ));
    } else if ( fabs(texcoord.w - 5) < ggEps ) {
        int off = 2;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + off)/16.0,
            (1-fract(texcoord.z))/16.0
            ));
    } else if ( fabs(texcoord.w - 6) < ggEps ) {
        int off = 2;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x + texcoord.z) + off)/16.0,
            (1-fract(texcoord.y))/16.0
            ));
    }

    // trunk
    if ( fabs(texcoord.w - 7) < ggEps ) {
        int ioff = 5;
        int joff = 1;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + ioff)/16.0,
            ((1-fract(texcoord.z))+joff)/16.0
            ));
    } else if ( fabs(texcoord.w - 8) < ggEps ) {
        int ioff = 5;
        int joff = 1;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + ioff)/16.0,
            ((1-fract(texcoord.z))+joff)/16.0
            ));
    } else if ( fabs(texcoord.w - 9) < ggEps ) {
        int ioff = 4;
        int joff = 1;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x + texcoord.z) + ioff)/16.0,
            ((1-fract(texcoord.y))+joff)/16.0
            ));
    }

    // leaf
    if ( fabs(texcoord.w - 10) < ggEps ) {
        int ioff = 4;
        int joff = 3;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + ioff)/16.0,
            ((1-fract(texcoord.z))+joff)/16.0
            ));
    } else if ( fabs(texcoord.w - 11) < ggEps ) {
        int ioff = 4;
        int joff = 3;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + ioff)/16.0,
            ((1-fract(texcoord.z))+joff)/16.0
            ));
    } else if ( fabs(texcoord.w - 12) < ggEps ) {
        int ioff = 4;
        int joff = 3;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x + texcoord.z) + ioff)/16.0,
            ((1-fract(texcoord.y))+joff)/16.0
            ));
    }

    // Water
    if ( fabs(texcoord.w - 13) < ggEps ) {
        int ioff = 14;
        int joff = 13;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + ioff)/16.0,
            ((1-fract(texcoord.z))+joff)/16.0
            ));
    } else if ( fabs(texcoord.w - 14) < ggEps ) {
        int ioff = 14;
        int joff = 13;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x) + ioff)/16.0,
            ((1-fract(texcoord.z))+joff)/16.0
            ));
    } else if ( fabs(texcoord.w - 15) < ggEps ) {
        int ioff = 14;
        int joff = 13;
        fragColour = texture( tex, vec2(
            (fract(texcoord.x + texcoord.z) + ioff)/16.0,
            ((1-fract(texcoord.y))+joff)/16.0
            ));
    }

}
