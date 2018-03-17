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
      m_puppet_positionAttribLocation(0),
      m_puppet_normalAttribLocation(0),
      m_vao_puppet_meshData(0),
      m_vbo_puppet_vertexPositions(0),
      m_vbo_puppet_vertexNormals(0),
      m_vao_cube(0),
      m_vbo_cube(0),
      m_ebo_cube(0)
{
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
    // Set the background colour.
    glClearColor(0.35, 0.35, 0.35, 1.0);

    createShaderProgram();

    // A3 puppet manipulation
    {
        glGenVertexArrays(1, &m_vao_puppet_meshData);
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

        float cube_verts[] = {
            0.0f, 0.0f, 0.0f, // y=0, top left
            1.0f, 0.0f, 0.0f, // y=0, top right
            1.0f, 0.0f, 1.0f, // y=0, bot right
            0.0f, 0.0f, 1.0f, // y=0, bot left

            0.0f, 1.0f, 0.0f, // y=1, top left
            1.0f, 1.0f, 0.0f, // y=1, top right
            1.0f, 1.0f, 1.0f, // y=1, bot right
            0.0f, 1.0f, 1.0f  // y=1, bot left
        };
        GLuint indices[] = {
            0, 1, 2, //bottom square
            2, 3, 0,

            4, 5, 6, //bottom square
            6, 7, 4,

            0, 4, 7, //side square 1
            7, 3, 0,

            1, 5, 6, //side square 2
            6, 2, 1,

            0, 1, 5, //side square 3
            5, 4, 0,

            3, 2, 6, //side square 4
            6, 7, 3,
        };

        glGenVertexArrays(1, &m_vao_cube);
        glBindVertexArray( m_vao_cube );

        // Create the cube vertex buffer
        glGenBuffers( 1, &m_vbo_cube );
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo_cube );
        glBufferData( GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW );

        // Create the cube vertex element buffer
        glGenBuffers( 1, &m_ebo_cube );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ebo_cube );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
            indices, GL_STATIC_DRAW );


        // Specify the means of extracting the position values properly.
        GLint posAttrib = m_cube_shader.getAttribLocation( "position" );
        glEnableVertexAttribArray( posAttrib );
        glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
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
void Winter::createShaderProgram()
{
    m_puppet_shader.generateProgramObject();
    m_puppet_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
    m_puppet_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
    m_puppet_shader.link();

    m_cube_shader.generateProgramObject();
    m_cube_shader.attachVertexShader( getAssetFilePath("CubeVertexShader.vs").c_str() );
    m_cube_shader.attachFragmentShader( getAssetFilePath("CubeFragmentShader.fs").c_str() );
    m_cube_shader.link();
}

//----------------------------------------------------------------------------------------
void Winter::enableVertexShaderInputSlots()
{
    //-- Enable input slots for m_vao_puppet_meshData:
    {
        glBindVertexArray(m_vao_puppet_meshData);

        // Enable the vertex shader attribute location for "position" when rendering.
        m_puppet_positionAttribLocation = m_puppet_shader.getAttribLocation("position");
        glEnableVertexAttribArray(m_puppet_positionAttribLocation);

        // Enable the vertex shader attribute location for "normal" when rendering.
        m_puppet_normalAttribLocation = m_puppet_shader.getAttribLocation("normal");
        glEnableVertexAttribArray(m_puppet_normalAttribLocation);

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
    glVertexAttribPointer(m_puppet_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from the vertex buffer "m_vbo_puppet_vertexNormals" into the
    // "normal" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_puppet_vertexNormals);
    glVertexAttribPointer(m_puppet_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

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
    m_perpsective = glm::perspective(degreesToRadians(45.0f), aspect, 1.0f, 1000.0f);
}


//----------------------------------------------------------------------------------------
void Winter::initViewMatrix() {
    //m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            //vec3(0.0f, 1.0f, 0.0f));
    m_view = glm::lookAt(
        glm::vec3( 0.0f, 2.0f, -float(4)*2.0*M_SQRT1_2 ),
        glm::vec3( 0.0f, 2.0f, 0.0f ),
        glm::vec3( 0.0f, 1.0f, 0.0f ) );
}

//----------------------------------------------------------------------------------------
void Winter::initLightSources() {
    // World-space position
    m_light.position = vec3(-2.0f, 5.0f, 0.5f);
    m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void Winter::uploadCommonSceneUniforms() {
    m_puppet_shader.enable();
    {
        //-- Set Perpsective matrix uniform for the scene:
        GLint location = m_puppet_shader.getUniformLocation("Perspective");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
        CHECK_GL_ERRORS;

        {
            location = m_puppet_shader.getUniformLocation("light.position");
            glUniform3fv(location, 1, value_ptr(m_light.position));
            location = m_puppet_shader.getUniformLocation("light.rgbIntensity");
            glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
            CHECK_GL_ERRORS;
        }

        //-- Set background light ambient intensity
        {
            location = m_puppet_shader.getUniformLocation("ambientIntensity");
            vec3 ambientIntensity(0.05f);
            glUniform3fv(location, 1, value_ptr(ambientIntensity));
            CHECK_GL_ERRORS;
        }
    }
    m_puppet_shader.disable();

    m_cube_shader.enable();
    {
        cout<<"P"<<m_perpsective<<endl;
        //-- Set Perpsective matrix uniform for the scene:
        GLint location = m_cube_shader.getUniformLocation("P");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
        CHECK_GL_ERRORS;

        cout<<"V"<<m_view<<endl;
        location = m_cube_shader.getUniformLocation("V");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_view));
        CHECK_GL_ERRORS;

    }
    m_cube_shader.disable();
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
        const ShaderProgram & shader,
        const GeometryNode & node,
        const glm::mat4 & viewMatrix
) {

    shader.enable();
    {
        //-- Set ModelView matrix:
        GLint location = shader.getUniformLocation("ModelView");
        mat4 modelView = viewMatrix * node.trans;
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;

        {
            //-- Set NormMatrix:
            location = shader.getUniformLocation("NormalMatrix");
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;


            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            vec3 kd = node.material.kd;

            if ( node.isSelected ) {
                kd = vec3(0.9, 0.2, 0.2);
            }

            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.ks");
            vec3 ks = node.material.ks;
            glUniform3fv(location, 1, value_ptr(ks));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.shininess");
            glUniform1f(location, node.material.shininess);
            CHECK_GL_ERRORS;
        }

    }
    shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void Winter::draw() {
    //glEnable( GL_DEPTH_TEST );
    //glCullFace( GL_BACK );
    //glEnable(GL_CULL_FACE);


    {
        m_cube_shader.enable();
        glBindVertexArray( m_vao_cube );

        GLint location = m_cube_shader.getUniformLocation("M");
        mat4 M;
        glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( M ) );

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray( 0 );
        CHECK_GL_ERRORS;
        m_cube_shader.disable();
    }



    {
        m_puppet_shader.enable();
        //renderSceneGraph(*m_puppet);
        m_puppet_shader.disable();
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

    updateShaderUniforms(m_puppet_shader, *geometryNode, M);


    // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
    BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

    //-- Now render the mesh:
    m_puppet_shader.enable();
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    m_puppet_shader.disable();

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
