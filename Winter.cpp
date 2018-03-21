#include "Winter.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <imgui/imgui.h>

#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

using namespace std;
using namespace glm;

static bool show_gui = true;
const size_t CIRCLE_PTS = 48;
static int interaction_radio = 0;




// Constructor
Winter::Winter(const std::string & luaSceneFile) : m_luaSceneFile(luaSceneFile){ }
// Destructor
Winter::~Winter() { }

void Winter::init() {
    // Set the background colour.
    glClearColor(0.35, 0.35, 0.35, 1.0);

    player.init( m_luaSceneFile );
    terrain.init();
    terrain.genTerrain();

    initPerspectiveMatrix();
    initViewMatrix();
    initLightSources();

    last_frame_time = getTime();
}


void Winter::initPerspectiveMatrix() {
    float aspect = ((float)m_windowWidth) / m_windowHeight;
    //m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
    m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 1.0f, 1000.0f);
}

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

void Winter::initLightSources() {
    // World-space position
    m_light.position = vec3(-2.0f, 5.0f, 0.5f);
    m_light.rgbIntensity = vec3(0.8f); // White light
}

void Winter::uploadCommonSceneUniforms() {
    vec3 ambientIntensity(0.5f);
    player.updateUniform( m_perpsective, m_view, m_light, ambientIntensity );
    terrain.updateUniform( m_perpsective, m_view );
}

/*
 * Called once per frame, before guiLogic().
 */
void Winter::appLogic() {
    // Place per frame, application logic here ...

    double curr_frame_time = getTime();
    delta_time = (curr_frame_time - last_frame_time)/1000;
    last_frame_time = curr_frame_time;

    player.move( ctrls, delta_time, terrain );

    uploadCommonSceneUniforms();
}

/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void Winter::guiLogic() {
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

/*
 * Called once per frame, after guiLogic().
 */
void Winter::draw() {
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE);

    terrain.render();
    player.render();
}

/*
 * Called once, after program is signaled to terminate.
 */
void Winter::cleanup() {

}

bool Winter::cursorEnterWindowEvent ( int entered) {
    bool eventHandled(false);
    return eventHandled;
}

bool Winter::mouseMoveEvent ( double xPos, double yPos) {
    bool eventHandled(false);

    if ( !ImGui::IsMouseHoveringAnyWindow() ) {
    }

    return eventHandled;
}

bool Winter::mouseButtonInputEvent ( int button, int actions, int mods) {
    bool eventHandled(false);

    if ( !ImGui::IsMouseHoveringAnyWindow() ) {
    }

    return eventHandled;
}

bool Winter::mouseScrollEvent ( double xOffSet, double yOffSet) {
    bool eventHandled(false);

    return eventHandled;
}

bool Winter::windowResizeEvent ( int width, int height) {
    bool eventHandled(false);
    initPerspectiveMatrix();
    return eventHandled;
}

bool Winter::keyInputEvent ( int key, int action, int mods) {
    bool eventHandled(false);

    if( action == GLFW_PRESS ) {
        if( key == GLFW_KEY_W ) {
            ctrls.W = true;
            eventHandled = true;
        }
        if( key == GLFW_KEY_A ) {
            ctrls.A = true;
            eventHandled = true;
        }
        if( key == GLFW_KEY_S ) {
            ctrls.S = true;
            eventHandled = true;
        }
        if( key == GLFW_KEY_D ) {
            ctrls.D = true;
            eventHandled = true;
        }
        if( key == GLFW_KEY_SPACE ) {
            ctrls.Space = true;
            eventHandled = true;
        }

        if ( key == GLFW_KEY_Q ) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            eventHandled = true;
        }
    }

    if( action == GLFW_RELEASE ) {
        if( key == GLFW_KEY_W ) {
            ctrls.W = false;
            eventHandled = true;
        }
        if( key == GLFW_KEY_A ) {
            ctrls.A = false;
            eventHandled = true;
        }
        if( key == GLFW_KEY_S ) {
            ctrls.S = false;
            eventHandled = true;
        }
        if( key == GLFW_KEY_D ) {
            ctrls.D = false;
            eventHandled = true;
        }
        if( key == GLFW_KEY_SPACE ) {
            ctrls.Space = false;
            eventHandled = true;
        }
    }

    return eventHandled;
}
