#include "Skybox.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
using namespace std;
using namespace glm;

void Skybox::init() {
    {
        shader = SkyboxShader::getInstance();
        string files[6] = { "right.png", "left.png", "top.png", "bottom.png", "back.png", "front.png"};
        skyboxTex = new Texture( files );
    }
    CHECK_GL_ERRORS;

    {
        float SIZE = 500;
        float cube_verts[] = {
            -SIZE, -SIZE, -SIZE, // y=0, top left
            SIZE, -SIZE, -SIZE, // y=0, top right
            SIZE, -SIZE, SIZE, // y=0, bot right
            -SIZE, -SIZE, SIZE, // y=0, bot left

            -SIZE, SIZE, -SIZE, // y=1, top left
            SIZE, SIZE, -SIZE, // y=1, top right
            SIZE, SIZE, SIZE, // y=1, bot right
            -SIZE, SIZE, SIZE  // y=1, bot left
        };
        GLuint indices[] = {
            2, 1, 0, //bottom square
            0, 3, 2,

            4, 5, 6, //top square
            6, 7, 4,

            0, 4, 7, //side square 1
            7, 3, 0,

            6, 5, 1, //side square 2
            1, 2, 6,

            0, 1, 5, //side square 3
            5, 4, 0,

            6, 2, 3, //side square 4
            3, 7, 6,
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
    CHECK_GL_ERRORS;


}

void Skybox::updateUniform( mat4 P, mat4 V ) {
    shader->enable();
    glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
    V[3][0] = 0;
    V[3][1] = 0;
    V[3][2] = 0;
    glUniformMatrix4fv( shader->V, 1, GL_FALSE, value_ptr(V));
    shader->disable();
}

void Skybox::render() {
    shader->enable();
    {
        skyboxTex->bind( shader->texAttrib );

        glBindVertexArray( vao );
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // actual drawing
        glEnableVertexAttribArray( shader->posAttrib );
        glVertexAttribPointer( shader->posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
        glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    CHECK_GL_ERRORS;
    shader->disable();
}
