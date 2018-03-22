#pragma once

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "Util.hpp"
#include "Shader.hpp"
#include "Puppet.hpp"
#include "SceneNode.hpp"
#include "JointNode.hpp"
#include "GeometryNode.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class Puppet {
public:
    Puppet() : joint_t(0.5) {}
    void init( std::string file );
    void render();
    void updateShaderUniforms( const GeometryNode & node, const glm::mat4 & viewMatrix );
    void updateUniform( const glm::mat4 &P, const glm::mat4 &V,  const LightSource &m_light, const glm::vec3 &ambientIntensity );
    void animate( float delta );

private:

    //-- One time initialization methods:
    void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
    void mapVboDataToVertexShaderInputLocations();
    void loadPuppet(const std::string & filename);
    void loadJoints( SceneNode* node );


    // render methods
    void renderSceneGraph(const SceneNode *node, glm::mat4 M);
    void renderJointGraph(const SceneNode *node, glm::mat4 M);
    void renderGeometryGraph(const SceneNode *node, glm::mat4 M );

    //-- GL resources for mesh geometry data:
    GLuint m_vao_puppet_meshData;
    GLuint m_vbo_puppet_vertexPositions;
    GLuint m_vbo_puppet_vertexNormals;

    // BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
    // object. Each BatchInfo object contains an index offset and the number of indices
    // required to render the mesh with identifier MeshId.
    BatchInfoMap m_batchInfoMap;
    std::shared_ptr<SceneNode> m_puppet;

    // rotation control joints
    JointNode *left_hip, *right_hip;
    JointNode *left_shoulder, *right_shoulder;
    float joint_t;

    glm::mat4 V;
    PuppetShader* shader;
};
