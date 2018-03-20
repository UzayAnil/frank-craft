#pragma once

#include <cstring>
#include <glm/glm.hpp>
#include <cassert>
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "Shader.hpp"

#define CX 16
#define CY 16
#define CZ 16

typedef glm::tvec4<GLbyte> byte4;

enum BlockType {
    Grass = 1
};

class Chunk {
public:

    Chunk() : vao(0), vbo(0) {
        std::memset(grid, 0, sizeof(grid));
        num_filled = 0;
        changed = true;
    }

    void init( std::string vert_path, std::string frag_path );

    ~Chunk() {
        glDeleteBuffers( 1, &vbo );
        glDeleteVertexArrays( 1, &vao );

        delete shader;
    }

    int isblocked( int x1, int y1, int z1, int x2, int y2, int z2 ) {
        if( !grid[x1][y1][z1] )
            return true;

        if( grid[x2][y2][z2] )
            return true;
    }

    int get(int x, int y, int z) {
        assert( x >= 0 && y >= 0 && z >= 0 );
        return grid[x][y][z];
    }

    void set(int x, int y, int z, BlockType type) {
        assert( x >= 0 && y >= 0 && z >= 0 && type >= 0);
        grid[x][y][z] = type;
        changed = true;
    }

    void update();
    void render();
    void updateUniform( glm::mat4 P, glm::mat4 V );

private:
    CubeShader *shader;
    BlockType grid[CX][CY][CZ];

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;

    int num_filled;
    bool changed;
};
