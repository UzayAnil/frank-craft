#include "Cube.hpp"
#include "Terrain.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <lodepng/lodepng.h>
#include <vector>
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

        shader = CubeShader::getInstance();
    }
    CHECK_GL_ERRORS;

    // texture
    {
        std::vector<unsigned char> image;
        unsigned int width, height;
        unsigned error = lodepng::decode(image, width, height, CS488Window::getAssetFilePath("terrain.png").c_str());
        if(error != 0) {
            std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
            return;
        }

        glGenTextures(1, &texture);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    }
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

                if( !grid[x][y][z] ) continue;
                //if(isblocked(x, y, z, x - 1, y, z)) {
                    //is_visible = false;
                    //continue;
                //}


                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                //if(is_visible && z != 0 && grid[x][y][z] == grid[x][y][z - 1]) {
                    //vertex[i - 5] = byte4(x, y, z + 1, side);
                    //vertex[i - 2] = byte4(x, y, z + 1, side);
                    //vertex[i - 1] = byte4(x, y + 1, z + 1, side);
                //// Otherwise, add a new quad.
                //} else {
                    vertex[i++] = byte4(x, y, z, side);
                    vertex[i++] = byte4(x, y, z + 1, side);
                    vertex[i++] = byte4(x, y + 1, z, side);
                    vertex[i++] = byte4(x, y + 1, z, side);
                    vertex[i++] = byte4(x, y, z + 1, side);
                    vertex[i++] = byte4(x, y + 1, z + 1, side);
                //}

                //is_visible = true;
            }
        }
    }

    // View from positive x

    for(int x = 0; x < SX; x++) {
        for(int y = 0; y < SY; y++) {
            for(int z = 0; z < SZ; z++) {

                if( !grid[x][y][z] ) continue;
                //if(isblocked(x, y, z, x + 1, y, z)) {
                    //is_visible = false;
                    //continue;
                //}

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                //if(is_visible && z != 0 && grid[x][y][z] == grid[x][y][z - 1]) {
                    //vertex[i - 4] = byte4(x + 1, y, z + 1, side);
                    //vertex[i - 2] = byte4(x + 1, y + 1, z + 1, side);
                    //vertex[i - 1] = byte4(x + 1, y, z + 1, side);
                //} else {
                    vertex[i++] = byte4(x + 1, y, z, side);
                    vertex[i++] = byte4(x + 1, y + 1, z, side);
                    vertex[i++] = byte4(x + 1, y, z + 1, side);
                    vertex[i++] = byte4(x + 1, y + 1, z, side);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
                    vertex[i++] = byte4(x + 1, y, z + 1, side);
                //}
                //is_visible = true;
            }
        }
    }

    // View from negative y

    for(int x = 0; x < SX; x++) {
        for(int y = SY - 1; y >= 0; y--) {
            for(int z = 0; z < SZ; z++) {

                if( !grid[x][y][z] ) continue;
                //if(isblocked(x, y, z, x, y - 1, z)) {
                    //is_visible = false;
                    //continue;
                //}

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                //if(is_visible && z != 0 && grid[x][y][z] == grid[x][y][z - 1]) {
                    //vertex[i - 4] = byte4(x, y, z + 1, bottom + 128);
                    //vertex[i - 2] = byte4(x + 1, y, z + 1, bottom + 128);
                    //vertex[i - 1] = byte4(x, y, z + 1, bottom + 128);
                //} else {
                    vertex[i++] = byte4(x, y, z, bottom);
                    vertex[i++] = byte4(x + 1, y, z, bottom);
                    vertex[i++] = byte4(x, y, z + 1, bottom);
                    vertex[i++] = byte4(x + 1, y, z, bottom);
                    vertex[i++] = byte4(x + 1, y, z + 1, bottom);
                    vertex[i++] = byte4(x, y, z + 1, bottom);
                //}
                //is_visible = true;
            }
        }
    }

    // View from positive y

    for(int x = 0; x < SX; x++) {
        for(int y = 0; y < SY; y++) {
            for(int z = 0; z < SZ; z++) {

                if( !grid[x][y][z] ) continue;
                //if(isblocked(x, y, z, x, y + 1, z)) {
                    //is_visible = false;
                    //continue;
                //}

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                //if(is_visible && z != 0 && grid[x][y][z] == grid[x][y][z - 1]) {
                    //vertex[i - 5] = byte4(x, y + 1, z + 1, top + 128);
                    //vertex[i - 2] = byte4(x, y + 1, z + 1, top + 128);
                    //vertex[i - 1] = byte4(x + 1, y + 1, z + 1, top + 128);
                //} else {
                    vertex[i++] = byte4(x, y + 1, z, top);
                    vertex[i++] = byte4(x, y + 1, z + 1, top);
                    vertex[i++] = byte4(x + 1, y + 1, z, top);
                    vertex[i++] = byte4(x + 1, y + 1, z, top);
                    vertex[i++] = byte4(x, y + 1, z + 1, top);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, top);
                //}
                //is_visible = true;
            }
        }
    }

    // View from negative z

    for(int x = 0; x < SX; x++) {
        for(int z = SZ - 1; z >= 0; z--) {
            for(int y = 0; y < SY; y++) {

                if( !grid[x][y][z] ) continue;
                //if(isblocked(x, y, z, x, y, z - 1)) {
                    //is_visible = false;
                    //continue;
                //}

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                //if(is_visible && y != 0 && grid[x][y][z] == grid[x][y - 1][z]) {
                    //vertex[i - 5] = byte4(x, y + 1, z, side);
                    //vertex[i - 3] = byte4(x, y + 1, z, side);
                    //vertex[i - 2] = byte4(x + 1, y + 1, z, side);
                //} else {
                    vertex[i++] = byte4(x, y, z, side);
                    vertex[i++] = byte4(x, y + 1, z, side);
                    vertex[i++] = byte4(x + 1, y, z, side);
                    vertex[i++] = byte4(x, y + 1, z, side);
                    vertex[i++] = byte4(x + 1, y + 1, z, side);
                    vertex[i++] = byte4(x + 1, y, z, side);
                //}
                //is_visible = true;
            }
        }
    }

    // View from positive z

    for(int x = 0; x < SX; x++) {
        for(int z = 0; z < SZ; z++) {
            for(int y = 0; y < SY; y++) {

                if( !grid[x][y][z] ) continue;
                //if(isblocked(x, y, z, x, y, z + 1)) {
                    //is_visible = false;
                    //continue;
                //}

                uint8_t top;
                uint8_t bottom;
                uint8_t side;
                if ( grid[x][y][z] == BlockType::Grass ) {
                    top = 0-16;
                    side = 3;
                    bottom = 2-16;
                }

                //if(is_visible && y != 0 && grid[x][y][z] == grid[x][y - 1][z]) {
                    //vertex[i - 4] = byte4(x, y + 1, z + 1, side);
                    //vertex[i - 3] = byte4(x, y + 1, z + 1, side);
                    //vertex[i - 1] = byte4(x + 1, y + 1, z + 1, side);
                //} else {
                    vertex[i++] = byte4(x, y, z + 1, side);
                    vertex[i++] = byte4(x + 1, y, z + 1, side);
                    vertex[i++] = byte4(x, y + 1, z + 1, side);
                    vertex[i++] = byte4(x, y + 1, z + 1, side);
                    vertex[i++] = byte4(x + 1, y, z + 1, side);
                    vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
                //}
                //is_visible = true;
            }
        }
    }

    changed = false;
    num_filled = i;

    //// If this chunk is empty, no need to allocate a chunk slot.
    //if(!num_filled)
        //return;

    // If we don't have an active slot, find one
    //if(chunk_slot[slot] != this) {
        //int lru = 0;
        //for(int i = 0; i < CHUNKSLOTS; i++) {
            //// If there is an empty slot, use it
            //if(!chunk_slot[i]) {
                //lru = i;
                //break;
            //}
            //// Otherwise try to find the least recently used slot
            //if(chunk_slot[i]->lastused < chunk_slot[lru]->lastused)
                //lru = i;
        //}

        //// If the slot is empty, create a new VBO
        //if(!chunk_slot[lru]) {
            //glGenBuffers(1, &vbo);
        //// Otherwise, steal it from the previous slot owner
        //} else {
            //vbo = chunk_slot[lru]->vbo;
            //chunk_slot[lru]->changed = true;
        //}

        //slot = lru;
        //chunk_slot[slot] = this;
    //}

    // Upload vertices
    //int ii = 0;
    //char* b = (char*)vertex;
    //while( ii < i ) {
        //cout<<"ii: "<<ii<<" b: "<<int(*b)<<endl;
        //ii++;
        //b++;
    //}
    //cout<<"done"<<endl;


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

    shader->enable();

    glBindVertexArray( vao );
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray( shader->posAttrib );
    glVertexAttribPointer( shader->posAttrib, 4, GL_BYTE, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, num_filled);

    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    CHECK_GL_ERRORS;

    shader->disable();
}

void Chunk::updateUniform( mat4 P, mat4 V ) {
    shader->enable();
    glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv( shader->V, 1, GL_FALSE, value_ptr(V));
    shader->disable();
}
