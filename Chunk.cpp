#include "Chunk.hpp"
#include "Terrain.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <lodepng/lodepng.h>
#include <iostream>

using namespace std;
using namespace glm;

Chunk::Chunk() : vao(0), vbo(0),
        left( nullptr ), right( nullptr ),
        below( nullptr ), above( nullptr ),
        front( nullptr ), back( nullptr ) {

    std::memset(grid, 0, sizeof(grid));
    num_filled = 0;
    changed = true;
    vertex = new byte4[ SX * SY * SZ * 6 * 6 ];

}

Chunk::~Chunk() {
    glDeleteBuffers( 1, &vbo );
    glDeleteVertexArrays( 1, &vao );

    delete []vertex;
}

bool Chunk::isblocked( int x1, int y1, int z1, int x2, int y2, int z2 ) {
    if( get(x1, y1, z1) == BlockType::Empty )
        return true;

    if( get(x2, y2, z2) != BlockType::Empty )
        return true;
}
void Chunk::set(int x, int y, int z, BlockType type) {
    if(x < 0)   { left->set( x+SX, y, z, type); return; }
    if(x >= SX) { right->set( x-SX, y, z, type); return; }
    if(y < 0)   { below->set( x, y+SY, z, type); return; }
    if(y >= SY) { above->set( x, y-SY, z, type); return; }
    if(z < 0)   { front->set( x, y, z+SZ, type); return; }
    if(z >= SZ) { back->set( x, y, z-SZ, type); return; }

    grid[x][y][z] = type;
    changed = true;

    // need to rerender surronding blocks too
    if(x == 0 && left)          left->changed = true;
    if(x == SX - 1 && right)    right->changed = true;
    if(y == 0 && below)         below->changed = true;
    if(y == SY - 1 && above)    above->changed = true;
    if(z == 0 && front)         front->changed = true;
    if(z == SZ - 1 && back)     back->changed = true;
}

BlockType Chunk::get(int x, int y, int z) const {
    if(x < 0)   return left ? left->get( x+SX, y, z ) : BlockType::Empty;
    if(x >= SX) return right ? right->get( x-SX, y, z ) : BlockType::Empty;
    if(y < 0)   return below ? below->get( x, y+SY, z ) : BlockType::Empty;
    if(y >= SY) return above ? above->get( x, y-SY, z ) : BlockType::Empty;
    if(z < 0)   return front ? front->get( x, y, z+SZ ) : BlockType::Empty;
    if(z >= SZ) return back ? back->get( x, y, z-SZ ) : BlockType::Empty;

    return grid[x][y][z];
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

    shader = ChunkShader::getInstance();
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

/*************** super chunk *********************/
SuperChunk::SuperChunk() : minecraftTex(nullptr) {
    cout<<"allocating memory"<<endl;
    for(int x = 0; x < NCX; x++)
        for(int y = 0; y < NCY; y++)
            for(int z = 0; z < NCZ; z++)
                c[x][y][z] = new Chunk();
    cout<<"finish allocation"<<endl;

    for(int x = 0; x < NCX; x++) {
        for(int y = 0; y < NCY; y++) {
            for(int z = 0; z < NCZ; z++) {
                if(x > 0)       c[x][y][z]->left = c[x - 1][y][z];
                if(x < NCX-1)   c[x][y][z]->right = c[x + 1][y][z];
                if(y > 0)       c[x][y][z]->below = c[x][y - 1][z];
                if(y < NCY-1)   c[x][y][z]->above = c[x][y + 1][z];
                if(z > 0)       c[x][y][z]->front = c[x][y][z - 1];
                if(z < NCZ-1)   c[x][y][z]->back = c[x][y][z + 1];
            }
        }
    }
}
SuperChunk::~SuperChunk() {
    for(int x = 0; x < NCX; x++) {
        for(int y = 0; y < NCY; y++) {
            for(int z = 0; z < NCZ; z++) {
                delete c[x][y][z];
            }
        }
    }

    if ( minecraftTex != nullptr ) delete minecraftTex;
}

BlockType SuperChunk::get(int x, int y, int z, bool debug) const {
    int cx = x / Chunk::SX;
    int cy = y / Chunk::SY;
    int cz = z / Chunk::SZ;

    int xx = x % Chunk::SX;
    int yy = y % Chunk::SY;
    int zz = z % Chunk::SZ;

    //if ( debug ) {
        //cout<<"cx: "<<cx<<"  cy: "<<cy<<"  cz: "<<cz<<endl;
        //cout<<"xx: "<<xx<<"  yy: "<<yy<<"  zz: "<<zz<<endl;
    //}
    if( !( 0 <= cx && cx < NCX && 0 <= cy && cy < NCY && 0 <= cz && cz < NCZ ) )
        return BlockType::Empty;
    return c[cx][cy][cz]->get( xx, yy, zz );
}

void SuperChunk::set(int x, int y, int z, BlockType type) {
    int cx = x / Chunk::SX;
    int cy = y / Chunk::SY;
    int cz = z / Chunk::SZ;

    int xx = x % Chunk::SX;
    int yy = y % Chunk::SY;
    int zz = z % Chunk::SZ;

    if( !( 0 <= cx && cx < NCX && 0 <= cy && cy < NCY && 0 <= cz && cz < NCZ ) ) cout<<" setting invalid "<<endl;
    c[cx][cy][cz]->set( xx, yy, zz, type );
}

void SuperChunk::init() {
    shader = ChunkShader::getInstance();
    minecraftTex = new Texture( "terrain.png" );

    for(int x = 0; x < NCX; x++) {
        for(int y = 0; y < NCY; y++) {
            for(int z = 0; z < NCZ; z++) {
                c[x][y][z]->init();
            }
        }
    }
}

void SuperChunk::genTerrain() {
    for ( int x = 0; x < NCX*Chunk::SX; x++ ) {
        for( int z = 0; z < NCZ*Chunk::SZ; z++ ) {
            double noise = octiveNoise( float(x)/Chunk::SX/10, float(z)/Chunk::SZ/10, 5, 2);
            int h = (int)clamp( noise*2*double(NCY*Chunk::SY)-.7*(NCY*Chunk::SY), 1.0, (double)NCY*Chunk::SY );

            for ( int y = 0; y < h; y++ ) {
                set( x, y, z, BlockType::Grass );

            }
        }
    }
}

void SuperChunk::render() {

    shader->enable();
    {
        minecraftTex->bind( shader->texAttrib );
        for(int x = 0; x < NCX; x++) {
            for(int y = 0; y < NCY; y++) {
                for(int z = 0; z < NCZ; z++) {

                    mat4 M = translate(mat4(), vec3( x * Chunk::SX, y * Chunk::SY, z * Chunk::SZ));
                    glUniformMatrix4fv( shader->M, 1, GL_FALSE, value_ptr(M));
                    c[x][y][z]->render();
                }
            }
        }

    }
    shader->disable();

}
void SuperChunk::updateUniform( mat4 P, mat4 V ) {
    shader->enable();
    glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv( shader->V, 1, GL_FALSE, value_ptr(V));
    shader->disable();
}
