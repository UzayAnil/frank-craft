#include "Puppet.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "scene_lua.hpp"
#include "Util.hpp"

using namespace glm;
using namespace std;

void Puppet::init( std::string file ) {
    shader = PuppetShader::getInstance();

    {
        glGenVertexArrays(1, &m_vao_puppet_meshData);
        glBindVertexArray(m_vao_puppet_meshData);
        glEnableVertexAttribArray( shader->posAttrib );
        glEnableVertexAttribArray( shader->normAttrib );
        CHECK_GL_ERRORS;
        glBindVertexArray(0);
        CHECK_GL_ERRORS;
    }

    {
        loadPuppet(file);

        // Load and decode all .obj files at once here.  You may add additional .obj files to
        // this list in order to support rendering additional mesh types.  All vertex
        // positions, and normals will be extracted and stored within the MeshConsolidator
        // class.
        unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
                CS488Window::getAssetFilePath("cube.obj"),
                CS488Window::getAssetFilePath("sphere.obj"),
                CS488Window::getAssetFilePath("suzanne.obj")
        });


        // Acquire the BatchInfoMap from the MeshConsolidator.
        meshConsolidator->getBatchInfoMap(m_batchInfoMap);

        // Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
        uploadVertexDataToVbos(*meshConsolidator);
        mapVboDataToVertexShaderInputLocations();
    }
    CHECK_GL_ERRORS;

    {
        string files[6] = { "right.png", "left.png", "top.png", "bottom.png", "back.png", "front.png"};
        envMapTex = new Texture( files );
    }
    CHECK_GL_ERRORS;
}

void Puppet::loadPuppet(const std::string & filename) {
    // This version of the code treats the Lua file as an Asset,
    // so that you'd launch the program with just the filename
    // of a puppet in the Assets/ directory.
    // std::string assetFilePath = getAssetFilePath(filename.c_str());
    // m_puppet = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

    // This version of the code treats the main program argument
    // as a straightforward pathname.
    m_puppet = std::shared_ptr<SceneNode>(import_lua(filename));
    if (!m_puppet) {
        std::cerr << "Could not open " << filename << std::endl;
    }

    loadJoints( m_puppet.get() );
}

void Puppet::loadJoints( SceneNode* node ) {
    if ( node->m_name == "left_shoulder_joint" ) {
        left_shoulder = (JointNode*)node;

    } else if ( node->m_name == "right_shoulder_joint" ) {
        right_shoulder = (JointNode*)node;

    } else if ( node->m_name == "left_hip_joint" ) {
        left_hip = (JointNode*)node;

    } else if ( node->m_name == "right_hip_joint" ) {
        right_hip = (JointNode*)node;
    }

    for ( auto child : node->children ) {
        loadJoints( child );
    }

}

void Puppet::updateUniform( const glm::mat4 &P, const glm::mat4 &V, const LightSource &light, const vec3 &ambientIntensity ) {
    shader->enable();
    //-- Set Perpsective matrix uniform for the scene:
    glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
    CHECK_GL_ERRORS;

    this->V = V;

    {
        glUniform3fv( shader->lightPosAttrib, 1, value_ptr(light.position));
        glUniform3fv( shader->lightRgbAttrib, 1, value_ptr(light.rgbIntensity));
        CHECK_GL_ERRORS;
    }

    //-- Set background light ambient intensity
    {
        vec3 ambientIntensity(0.05f);
        glUniform3fv( shader->ambientAttrib, 1, value_ptr(ambientIntensity));
        CHECK_GL_ERRORS;
    }
    shader->disable();

}

void Puppet::uploadVertexDataToVbos (
        const MeshConsolidator & meshConsolidator
) {
    // Generate VBO to store all vertex position data
    {
        glGenBuffers(1, &m_vbo_puppet_vertexPositions);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_puppet_vertexPositions);

        glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
                meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }

    // Generate VBO to store all vertex normal data
    {
        glGenBuffers(1, &m_vbo_puppet_vertexNormals);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_puppet_vertexNormals);

        glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
                meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }

}

void Puppet::mapVboDataToVertexShaderInputLocations()
{
    // Bind VAO in order to record the data mapping.
    glBindVertexArray(m_vao_puppet_meshData);

    // Tell GL how to map data from the vertex buffer "m_vbo_puppet_vertexPositions" into the
    // "position" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_puppet_vertexPositions);
    glVertexAttribPointer( shader->posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from the vertex buffer "m_vbo_puppet_vertexNormals" into the
    // "normal" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_puppet_vertexNormals);
    glVertexAttribPointer( shader->normAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

void Puppet::render() {
    shader->enable();
    {
        envMapTex->bind( shader->envMapTexAttrib );

        glBindVertexArray(m_vao_puppet_meshData);

        renderSceneGraph( &(*m_puppet), V );

        glBindVertexArray(0);
        CHECK_GL_ERRORS;
    }
    shader->disable();
}

void Puppet::renderSceneGraph(const SceneNode *root, glm::mat4 M) {

    M = M * root->trans;
    for (const SceneNode * node : root->children) {
        if (node->m_nodeType == NodeType::SceneNode) {
            renderSceneGraph( node, M );
        } else if (node->m_nodeType == NodeType::JointNode) {
            renderJointGraph( node, M );
        } else if (node->m_nodeType == NodeType::GeometryNode) {
            renderGeometryGraph( node, M );
        }
    }
}

void Puppet::renderJointGraph(const SceneNode *root, glm::mat4 M ) {

    M = M * root->trans;
    for (const SceneNode * node : root->children) {
        if (node->m_nodeType == NodeType::SceneNode) {
            renderSceneGraph( node, M );
        } else if (node->m_nodeType == NodeType::JointNode) {
            renderJointGraph( node, M );
        } else if (node->m_nodeType == NodeType::GeometryNode) {
            renderGeometryGraph( node, M );
        }
    }
}

void Puppet::renderGeometryGraph(const SceneNode *root, glm::mat4 M ) {
    const GeometryNode * geometryNode = static_cast<const GeometryNode *>(root);

    updateShaderUniforms(*geometryNode, M);


    // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
    BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

    //-- Now render the mesh:
    shader->enable();
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    shader->disable();

    M = M * root->trans;
    for (const SceneNode * node : root->children) {
        if (node->m_nodeType == NodeType::SceneNode) {
            renderSceneGraph( node, M );
        } else if (node->m_nodeType == NodeType::JointNode) {
            renderJointGraph( node, M );
        } else if (node->m_nodeType == NodeType::GeometryNode) {
            renderGeometryGraph( node, M );
        }
    }
}

void Puppet::updateShaderUniforms(
        const GeometryNode & node,
        const glm::mat4 & viewMatrix
) {
    PuppetShader *shader = PuppetShader::getInstance();

    shader->enable();
    {
        //-- Set ModelView matrix:
        mat4 modelView = viewMatrix * node.trans;
        glUniformMatrix4fv( shader->MV, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;

        {
            //-- Set NormMatrix:
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv( shader->normMatrixAttrib, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;


            //-- Set Material values:
            vec3 kd = node.material.kd;
            if ( node.isSelected ) {
                kd = vec3(0.9, 0.2, 0.2);
            }
            glUniform3fv( shader->kdAttrib, 1, value_ptr(kd));
            CHECK_GL_ERRORS;

            vec3 ks = node.material.ks;
            glUniform3fv( shader->ksAttrib, 1, value_ptr(ks));
            CHECK_GL_ERRORS;

            glUniform1f( shader->shineAttrib, node.material.shininess);
            CHECK_GL_ERRORS;
        }

    }
    shader->disable();

}

void Puppet::animate( float delta ) {
    if ( fabs(delta) < ggEps ) return;
    joint_t += delta * 1.5;
    while ( joint_t < 0 ) joint_t += 2;
    while ( joint_t > 2 ) joint_t -= 2;

    double t = joint_t < 1 ? joint_t : 2-joint_t;
    {
        float new_angle = lerp( t, left_hip->m_joint_x.min, left_hip->m_joint_x.max);
        float delta_angle = new_angle - left_hip->m_x_angle;
        left_hip->rotate( 'x', delta_angle );
    }
    {
        float new_angle = lerp( 1-t, right_hip->m_joint_x.min, right_hip->m_joint_x.max);
        float delta_angle = new_angle - right_hip->m_x_angle;
        right_hip->rotate( 'x', delta_angle );
    }
    {
        float new_angle = lerp( 1-t, left_shoulder->m_joint_x.min, left_shoulder->m_joint_x.max);
        float delta_angle = new_angle - left_shoulder->m_x_angle;       left_shoulder->rotate( 'x', delta_angle );
        left_shoulder->rotate( 'x', delta_angle );
    }
    {
        float new_angle = lerp( t, right_shoulder->m_joint_x.min, right_shoulder->m_joint_x.max);
        float delta_angle = new_angle - right_shoulder->m_x_angle;       right_shoulder->rotate( 'x', delta_angle );
        right_shoulder->rotate( 'x', delta_angle );
    }


}
