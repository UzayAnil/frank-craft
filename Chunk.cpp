#include "Chunk.hpp"
#include "Terrain.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <lodepng/lodepng.h>
#include <iostream>

using namespace std;
using namespace glm;

void Chunk::genTerrain() {
    //set(0, 0, 2, BlockType::Grass);
    //set(2, 0, 2, BlockType::Grass);
    //set(2, 4, 2, BlockType::Grass);
    for ( int x = 0; x < SX; x++ ) {
        for( int z = 0; z < SZ; z++ ) {
            double noise = octiveNoise( float(x)/SX, float(z)/SZ, 5, 3 );
            int h = (int)clamp( noise*double(SY), 1.0, double(SY) );

            h = 0;
            set( x, h, z, BlockType::Grass );

            //for( int y = 0; y < h; y++ ) {
                //set( x, y, z, BlockType::Grass );
            //}
        }
    }
}

void Chunk::init() {

    // general vao, vbo
    {
        glGenVertexArrays( 1, &vao );
        glBindVertexArray( vao );

        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    CHECK_GL_ERRORS;

    shader = CubeShader::getInstance();
    CHECK_GL_ERRORS;
}

void Chunk::update() {
    // 6 faces, each needs 2 triangles( 3 vertices )
    //byte4 vertex[SX * SY * SZ * 6 * 6];
    int i = 0;
    bool is_visible = false;;

    // View from negative x
    for(int x = SX - 1; x >= 0; x--) {
        for(int y = 0; y < SY; y++) {
            for(int z = 0; z < SZ; z++) {

                //if( !grid[x][y][z] ) continue;
                if(isblocked(x, y, z, x - 1, y, z)) {
                    is_visible = false;
                    continue;
                }


                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                vertex[i++] = byte4(x, y, z, side);
                vertex[i++] = byte4(x, y, z + 1, side);
                vertex[i++] = byte4(x, y + 1, z, side);
                vertex[i++] = byte4(x, y + 1, z, side);
                vertex[i++] = byte4(x, y, z + 1, side);
                vertex[i++] = byte4(x, y + 1, z + 1, side);
                is_visible = true;
            }
        }
    }

    // View from positive x

    for(int x = 0; x < SX; x++) {
        for(int y = 0; y < SY; y++) {
            for(int z = 0; z < SZ; z++) {

                //if( !grid[x][y][z] ) continue;
                if(isblocked(x, y, z, x + 1, y, z)) {
                    is_visible = false;
                    continue;
                }

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                vertex[i++] = byte4(x + 1, y, z, side);
                vertex[i++] = byte4(x + 1, y + 1, z, side);
                vertex[i++] = byte4(x + 1, y, z + 1, side);
                vertex[i++] = byte4(x + 1, y + 1, z, side);
                vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
                vertex[i++] = byte4(x + 1, y, z + 1, side);
                is_visible = true;
            }
        }
    }

    // View from negative y

    for(int x = 0; x < SX; x++) {
        for(int y = SY - 1; y >= 0; y--) {
            for(int z = 0; z < SZ; z++) {

                //if( !grid[x][y][z] ) continue;
                if(isblocked(x, y, z, x, y - 1, z)) {
                    is_visible = false;
                    continue;
                }

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                vertex[i++] = byte4(x, y, z, bottom);
                vertex[i++] = byte4(x + 1, y, z, bottom);
                vertex[i++] = byte4(x, y, z + 1, bottom);
                vertex[i++] = byte4(x + 1, y, z, bottom);
                vertex[i++] = byte4(x + 1, y, z + 1, bottom);
                vertex[i++] = byte4(x, y, z + 1, bottom);
                is_visible = true;
            }
        }
    }

    // View from positive y

    for(int x = 0; x < SX; x++) {
        for(int y = 0; y < SY; y++) {
            for(int z = 0; z < SZ; z++) {

                //if( !grid[x][y][z] ) continue;
                if(isblocked(x, y, z, x, y + 1, z)) {
                    is_visible = false;
                    continue;
                }

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                vertex[i++] = byte4(x, y + 1, z, top);
                vertex[i++] = byte4(x, y + 1, z + 1, top);
                vertex[i++] = byte4(x + 1, y + 1, z, top);
                vertex[i++] = byte4(x + 1, y + 1, z, top);
                vertex[i++] = byte4(x, y + 1, z + 1, top);
                vertex[i++] = byte4(x + 1, y + 1, z + 1, top);
                is_visible = true;
            }
        }
    }

    // View from negative z

    for(int x = 0; x < SX; x++) {
        for(int z = SZ - 1; z >= 0; z--) {
            for(int y = 0; y < SY; y++) {

                //if( !grid[x][y][z] ) continue;
                if(isblocked(x, y, z, x, y, z - 1)) {
                    is_visible = false;
                    continue;
                }

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                vertex[i++] = byte4(x, y, z, side);
                vertex[i++] = byte4(x, y + 1, z, side);
                vertex[i++] = byte4(x + 1, y, z, side);
                vertex[i++] = byte4(x, y + 1, z, side);
                vertex[i++] = byte4(x + 1, y + 1, z, side);
                vertex[i++] = byte4(x + 1, y, z, side);
                is_visible = true;
            }
        }
    }

    // View from positive z

    for(int x = 0; x < SX; x++) {
        for(int z = 0; z < SZ; z++) {
            for(int y = 0; y < SY; y++) {

                //if( !grid[x][y][z] ) continue;
                if(isblocked(x, y, z, x, y, z + 1)) {
                    is_visible = false;
                    continue;
                }

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                vertex[i++] = byte4(x, y, z + 1, side);
                vertex[i++] = byte4(x + 1, y, z + 1, side);
                vertex[i++] = byte4(x, y + 1, z + 1, side);
                vertex[i++] = byte4(x, y + 1, z + 1, side);
                vertex[i++] = byte4(x + 1, y, z + 1, side);
                vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
                is_visible = true;
            }
        }
    }

    changed = false;
    num_filled = i;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, i * sizeof *vertex, vertex, GL_STATIC_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    CHECK_GL_ERRORS;
}


void Chunk::render() {
    if(changed)
        update();

    //lastused = now;

    if(!num_filled)
        return;

    {
        glBindVertexArray( vao );
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray( shader->posAttrib );
        glVertexAttribPointer( shader->posAttrib, 4, GL_BYTE, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, num_filled);

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    CHECK_GL_ERRORS;
}
