#include "Cube.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

Cube::Cube( const vec3 &min, const vec3 &max, bool wireframe )
    : min( min ), max( max ), wireframe( wireframe ) {}

void Cube::init() {
    shader = CubeShader::getInstance();

    {
        float cube_verts[] = {
            min.x, min.y, min.z, // y=0, top left
            max.x, min.y, min.z, // y=0, top right
            max.x, min.y, max.z, // y=0, bot right
            min.x, min.y, max.z, // y=0, bot left

            min.x, max.y, min.z, // y=1, top left
            max.x, max.y, min.z, // y=1, top right
            max.x, max.y, max.z, // y=1, bot right
            min.x, max.y, max.z  // y=1, bot left
        };
        GLuint indices[] = {
            0, 1, 2, //bottom square
            2, 3, 0,

            6, 5, 4, //top square
            4, 7, 6,

            7, 4, 0, //side square 1
            0, 3, 7,

            1, 5, 6, //side square 2
            6, 2, 1,

            5, 1, 0, //side square 3
            0, 4, 5,

            3, 2, 6, //side square 4
            6, 7, 3,
        };
        // Set up new VAO "environment" for cubes
        glGenVertexArrays( 1, &vao );
        glBindVertexArray( vao );

        // Create the cube vertex buffer
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW );

        // Create the cube vertex element buffer
        glGenBuffers( 1, &ebo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );


        // Specify the means of extracting the position values properly.
        glEnableVertexAttribArray( shader->posAttrib );
        glVertexAttribPointer( shader->posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
    }
}

void Cube::updateUniform( const mat4 &P, const mat4 &V ) {
    shader->enable();

    glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv( shader->V, 1, GL_FALSE, value_ptr(V));

    shader->disable();

}

void Cube::render() {
    shader->enable();
    {
        glBindVertexArray( vao );
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        if ( wireframe ) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
    }
    shader->disable();
}
