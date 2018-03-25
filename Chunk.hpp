#pragma once

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <cassert>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

typedef glm::tvec4<GLbyte> byte4;

enum BlockType {
    Empty = 0,
    Grass = 1
};

class Chunk {
public:

    static const int SX = 16;
    static const int SY = 32;
    static const int SZ = 16;

    Chunk() : vao(0), vbo(0),
            left( nullptr ), right( nullptr ),
            below( nullptr ), above( nullptr ),
            front( nullptr ), back( nullptr ) {

        std::memset(grid, 0, sizeof(grid));
        num_filled = 0;
        changed = true;
        vertex = new byte4[ SX * SY * SZ * 6 * 6 ];

    }

    void init();

    ~Chunk() {
        glDeleteBuffers( 1, &vbo );
        glDeleteVertexArrays( 1, &vao );

        delete []vertex;
    }

    int isblocked( int x1, int y1, int z1, int x2, int y2, int z2 ) {
        if( get(x1, y1, z1) == BlockType::Empty )
            return true;

        if( get(x2, y2, z2) != BlockType::Empty )
            return true;
    }

    BlockType get(int x, int y, int z) {
        if(x < 0)   return left ? left->get( x+SX, y, z ) : BlockType::Empty;
        if(x >= SX) return right ? right->get( x-SX, y, z ) : BlockType::Empty;
        if(y < 0)   return below ? below->get( x, y+SY, z ) : BlockType::Empty;
        if(y >= SY) return above ? above->get( x, y-SY, z ) : BlockType::Empty;
        if(z < 0)   return front ? front->get( x, y, z+SZ ) : BlockType::Empty;
        if(z >= SZ) return back ? back->get( x, y, z-SZ ) : BlockType::Empty;

        return grid[x][y][z];
    }

    void set(int x, int y, int z, BlockType type) {
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

    void genTerrain();
    void update();
    void render();

public:
    Chunk *left, *right, *below, *above, *front, *back;

private:
    byte4* vertex;
    CubeShader *shader;
    BlockType grid[SX][SY][SZ];

    GLuint vao;
    GLuint vbo;
    GLuint ebo;


    int num_filled;
    bool changed;
};

class SuperChunk {
public:
    static const int NSX = 32; // number of chunk in the X direction
    static const int NSY = 2;
    static const int NSZ = 32;
    Chunk *c[NSX][NSY][NSZ];

    SuperChunk() : minecraftTex(nullptr) {
        cout<<"a"<<endl;
        for(int x = 0; x < NSX; x++)
            for(int y = 0; y < NSY; y++)
                for(int z = 0; z < NSZ; z++)
                    c[x][y][z] = new Chunk();
        cout<<"b"<<endl;

        for(int x = 0; x < NSX; x++) {
            for(int y = 0; y < NSY; y++) {
                for(int z = 0; z < NSZ; z++) {
                    if(x > 0)       c[x][y][z]->left = c[x - 1][y][z];
                    if(x < NSX-1)   c[x][y][z]->right = c[x + 1][y][z];
                    if(y > 0)       c[x][y][z]->below = c[x][y - 1][z];
                    if(y < NSY-1)   c[x][y][z]->above = c[x][y + 1][z];
                    if(z > 0)       c[x][y][z]->front = c[x][y][z - 1];
                    if(z < NSZ-1)   c[x][y][z]->back = c[x][y][z + 1];
                }
            }
        }
    }
    ~SuperChunk() {
        for(int x = 0; x < NSX; x++) {
            for(int y = 0; y < NSY; y++) {
                for(int z = 0; z < NSZ; z++) {
                    delete c[x][y][z];
                }
            }
        }

        if ( minecraftTex != nullptr ) delete minecraftTex;
    }

    BlockType get(int x, int y, int z) const {
        int cx = x / Chunk::SX;
        int cy = y / Chunk::SY;
        int cz = z / Chunk::SZ;

        int xx = x % Chunk::SX;
        int yy = y % Chunk::SY;
        int zz = z % Chunk::SZ;

        assert( 0 <= cx && cx < NSX && 0 <= cy < NSY && 0 <= cz && cz < NSZ );
        return c[cx][cy][cz]->get( xx, yy, zz );
    }

    void set(int x, int y, int z, BlockType type) {
        int cx = x / Chunk::SX;
        int cy = y / Chunk::SY;
        int cz = z / Chunk::SZ;

        int xx = x % Chunk::SX;
        int yy = y % Chunk::SY;
        int zz = z % Chunk::SZ;

        assert( 0 <= cx && cx < NSX && 0 <= cy < NSY && 0 <= cz && cz < NSZ );
        c[xx][yy][zz]->set( x, y, z, type );
    }

    void init() {
        shader = CubeShader::getInstance();
        minecraftTex = new Texture( "terrain.png" );

        for(int x = 0; x < NSX; x++) {
            for(int y = 0; y < NSY; y++) {
                for(int z = 0; z < NSZ; z++) {
                    c[x][y][z]->init();
                }
            }
        }
    }

    void genTerrain() {
        for(int x = 0; x < NSX; x++) {
            for(int y = 0; y < NSY; y++) {
                for(int z = 0; z < NSZ; z++) {
                    c[x][y][z]->genTerrain();
                }
            }
        }
    }

    void updateUniform( mat4 P, mat4 V ) {
        shader->enable();
        glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
        glUniformMatrix4fv( shader->V, 1, GL_FALSE, value_ptr(V));
        shader->disable();
    }

    void render() {

        shader->enable();
        {
            minecraftTex->bind( shader->texAttrib );
            for(int x = 0; x < NSX; x++) {
                for(int y = 0; y < NSY; y++) {
                    for(int z = 0; z < NSZ; z++) {

                        mat4 M = translate(glm::mat4(1.0f), glm::vec3( x * Chunk::SX, y * Chunk::SY, z * Chunk::SZ));
                        glUniformMatrix4fv( shader->M, 1, GL_FALSE, value_ptr(M));
                        c[x][y][z]->render();
                    }
                }
            }

        }
        shader->disable();

}

private:
    CubeShader *shader;
    Texture *minecraftTex;
};

