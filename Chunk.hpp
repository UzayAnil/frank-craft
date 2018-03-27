#pragma once

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

typedef glm::tvec4<GLbyte> byte4;

enum BlockType {
    Empty = 0,
    Grass = 1,
    Dirt = 2,
    Trunk = 3,
    Leaf = 4,
    Water = 5
};

class Chunk {
public:

    static const int SX = 16;
    static const int SY = 16;
    static const int SZ = 16;

    Chunk();
    ~Chunk();

    void init();
    bool isblocked( int x1, int y1, int z1, int x2, int y2, int z2 );

    BlockType get(int x, int y, int z) const;
    void set(int x, int y, int z, BlockType type);

    void update();
    void render();

public:
    Chunk *left, *right, *below, *above, *front, *back;

private:
    byte4* vertex;
    ChunkShader *shader;
    BlockType grid[SX][SY][SZ];

    GLuint vao;
    GLuint vbo;
    GLuint ebo;


    int num_filled;
    bool changed;
};

class SuperChunk {
public:
    //TODO
    static const int NCX = 32; // number of chunk in the X direction
    static const int NCY = 6;
    static const int NCZ = 32;
    Chunk *c[NCX][NCY][NCZ];

    SuperChunk();
    ~SuperChunk();

    BlockType get(int x, int y, int z, bool debug=false) const;
    void set(int x, int y, int z, BlockType type);

    void init();
    void genTerrain();

    void updateUniform( glm::mat4 P, glm::mat4 V );
    void render();
    bool checkInBound( glm::vec3 pos );

private:
    ChunkShader *shader;
    Texture *minecraftTex;
};

