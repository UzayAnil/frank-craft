#include "Winter.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

static int interaction_radio = 0;




//----------------------------------------------------------------------------------------
// Constructor
Winter::Winter(const std::string & luaSceneFile)
    : m_luaSceneFile(luaSceneFile),
      //m_puppet_positionAttribLocation(0),
      //m_puppet_normalAttribLocation(0),
      m_vao_puppet_meshData(0),
      m_vbo_puppet_vertexPositions(0),
      m_vbo_puppet_vertexNormals(0) {
}

//----------------------------------------------------------------------------------------
// Destructor
Winter::~Winter()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void Winter::init()
{
    shader = PuppetShader::getInstance();
    // Set the background colour.
    glClearColor(0.35, 0.35, 0.35, 1.0);

    // A3 puppet manipulation
    {
        glGenVertexArrays(1, &m_vao_puppet_meshData);
    shader = PuppetShader::getInstance();
        enableVertexShaderInputSlots();

        processLuaSceneFile(m_luaSceneFile);

        // Load and decode all .obj files at once here.  You may add additional .obj files to
        // this list in order to support rendering additional mesh types.  All vertex
        // positions, and normals will be extracted and stored within the MeshConsolidator
        // class.
        unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
                getAssetFilePath("cube.obj"),
                getAssetFilePath("sphere.obj"),
                getAssetFilePath("suzanne.obj")
        });


        // Acquire the BatchInfoMap from the MeshConsolidator.
        meshConsolidator->getBatchInfoMap(m_batchInfoMap);

        // Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
        uploadVertexDataToVbos(*meshConsolidator);
        mapVboDataToVertexShaderInputLocations();
    }

    {
        chunk.init();
        chunk.genTerrain();
    }

    initPerspectiveMatrix();
    initViewMatrix();
    initLightSources();


    // Exiting the current scope calls delete automatically on meshConsolidator freeing
    // all vertex data resources.  This is fine since we already copied this data to
    // VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
    // this point.
}

//----------------------------------------------------------------------------------------
void Winter::processLuaSceneFile(const std::string & filename) {
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
}


//----------------------------------------------------------------------------------------
void Winter::enableVertexShaderInputSlots()
{
    //-- Enable input slots for m_vao_puppet_meshData:
    {
        glBindVertexArray(m_vao_puppet_meshData);

        // Enable the vertex shader attribute location for "position" when rendering.
        glEnableVertexAttribArray( shader->posAttrib );

        // Enable the vertex shader attribute location for "normal" when rendering.
        glEnableVertexAttribArray( shader->normAttrib );

        CHECK_GL_ERRORS;
    }

    // Restore defaults
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void Winter::uploadVertexDataToVbos (
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

//----------------------------------------------------------------------------------------
void Winter::mapVboDataToVertexShaderInputLocations()
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

//----------------------------------------------------------------------------------------
void Winter::initPerspectiveMatrix() {
    float aspect = ((float)m_windowWidth) / m_windowHeight;
    //m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
    m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 1.0f, 1000.0f);
}


//----------------------------------------------------------------------------------------
void Winter::initViewMatrix() {
    //m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            //vec3(0.0f, 1.0f, 0.0f));
    m_view = glm::lookAt(
        glm::vec3( 0.0f, 2.0f, -float(4)*2.0*M_SQRT1_2 ),
        glm::vec3( 0.0f, 2.0f, 0.0f ),
        glm::vec3( 0.0f, 1.0f, 0.0f ) );
    //m_view = m_view * rotate( mat4(), -float(degreesToRadians(110)), vec3( 1, 0, 0 ) );
    //m_view = m_view * translate( mat4(), vec3(-20, -80, 0) );
}

//----------------------------------------------------------------------------------------
void Winter::initLightSources() {
    // World-space position
    m_light.position = vec3(-2.0f, 5.0f, 0.5f);
    m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void Winter::uploadCommonSceneUniforms() {
    shader->enable();
    {
        //-- Set Perpsective matrix uniform for the scene:
        glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(m_perpsective));
        CHECK_GL_ERRORS;

        {
            glUniform3fv( shader->lightPosAttrib, 1, value_ptr(m_light.position));
            glUniform3fv( shader->lightRgbAttrib, 1, value_ptr(m_light.rgbIntensity));
            CHECK_GL_ERRORS;
        }

        //-- Set background light ambient intensity
        {
            vec3 ambientIntensity(0.05f);
            glUniform3fv( shader->ambientAttrib, 1, value_ptr(ambientIntensity));
            CHECK_GL_ERRORS;
        }
    }
    shader->disable();

    chunk.updateUniform( m_perpsective, m_view );
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void Winter::appLogic() {
    // Place per frame, application logic here ...

    uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void Winter::guiLogic()
{
    if( !show_gui ) {
        return;
    }

    static bool firstRun(true);
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(50, 50));
        firstRun = false;
    }

    static bool showDebugWindow(true);
    ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
    float opacity(0.5f);

    ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
            windowFlags);

        ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

    ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
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

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void Winter::draw() {
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE);

    {
        chunk.render();
    }

    {
        shader->enable();
        renderSceneGraph(*m_puppet);
        shader->disable();
    }

}

void Winter::renderSceneGraph(const SceneNode &root) {

    glBindVertexArray(m_vao_puppet_meshData);

    renderSceneGraph( &root, m_view );

    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

void Winter::renderSceneGraph(const SceneNode *root, glm::mat4 M) {

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

void Winter::renderJointGraph(const SceneNode *root, glm::mat4 M ) {

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

void Winter::renderGeometryGraph(const SceneNode *root, glm::mat4 M ) {
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


//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void Winter::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool Winter::cursorEnterWindowEvent (
        int entered
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool Winter::mouseMoveEvent (
        double xPos,
        double yPos
) {
    bool eventHandled(false);

    if ( !ImGui::IsMouseHoveringAnyWindow() ) {
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool Winter::mouseButtonInputEvent (
        int button,
        int actions,
        int mods
) {
    bool eventHandled(false);

    if ( !ImGui::IsMouseHoveringAnyWindow() ) {
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool Winter::mouseScrollEvent (
        double xOffSet,
        double yOffSet
) {
    bool eventHandled(false);

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool Winter::windowResizeEvent (
        int width,
        int height
) {
    bool eventHandled(false);
    initPerspectiveMatrix();
    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool Winter::keyInputEvent (
        int key,
        int action,
        int mods
) {
    bool eventHandled(false);

    if( action == GLFW_PRESS ) {
    }

    return eventHandled;
}
