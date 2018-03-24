#pragma once

#include <cstring>
#include <glm/glm.hpp>
#include <cassert>
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "Shader.hpp"

typedef glm::tvec4<GLbyte> byte4;

enum BlockType {
    Grass = 1
};

class Chunk {
public:

    static const int SX = 64;
    static const int SY = 64;
    static const int SZ = 64;

    Chunk() : vao(0), vbo(0) {
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
        if( !grid[x1][y1][z1] )
            return true;

        if( grid[x2][y2][z2] )
            return true;
    }

    int get(int x, int y, int z) {
        assert( 0 <= x && x < SX
            && 0 <= y && y < SY
            && 0 <= z && z < SZ );
        return grid[x][y][z];
    }

    void set(int x, int y, int z, BlockType type) {
        assert( 0 <= x && x < SX
            && 0 <= y && y < SY
            && 0 <= z && z < SZ );
        grid[x][y][z] = type;
        changed = true;
    }

    void genTerrain();
    void update();
    void render();
    void updateUniform( glm::mat4 P, glm::mat4 V );

private:
    byte4* vertex;
    CubeShader *shader;
    BlockType grid[SX][SY][SZ];

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;

    int num_filled;
    bool changed;
};
