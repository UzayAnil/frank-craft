#include "Winter.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <imgui/imgui.h>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>

using namespace std;
using namespace glm;

static bool show_gui = true;

// Constructor
Winter::Winter(const std::string & luaSceneFile) : m_luaSceneFile(luaSceneFile), camera(player){ }
// Destructor
Winter::~Winter() { }

void Winter::init() {
    // Set the background colour.
    glClearColor(0.35, 0.35, 0.35, 1.0);

    player.init( m_luaSceneFile );
    terrain.init();
    terrain.genTerrain();
    particle_system.init();
    skybox.init();

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
    m_light.position = vec3(290, 490, -220);
    m_light.rgbIntensity = vec3(0.8f); // White light
}

void Winter::uploadCommonSceneUniforms() {
    vec3 ambientIntensity(0.5f);
    player.updateUniform( m_perpsective, m_view, m_light, ambientIntensity );
    terrain.updateUniform( m_perpsective, m_view );
    particle_system.updateUniform( m_perpsective, m_view );
    skybox.updateUniform( m_perpsective, m_view );
}

/*
 * Called once per frame, before guiLogic().
 */
void Winter::appLogic() {
    // Place per frame, application logic here ...

    // calculate time passed in seconds
    double curr_frame_time = getTime();
    delta_time = (curr_frame_time - last_frame_time)/1000;
    last_frame_time = curr_frame_time;

    // update movements with time
    player.move( ctrls, delta_time, terrain );
    particle_system.update( delta_time );


    for ( int i = 0; i < rand()%5; i++ ) {
        float vx = (rand()%5+1)* (2*float(rand())/RAND_MAX-1);
        float vy = (rand()%5+1)* (2*float(rand())/RAND_MAX-1);
        float vz = (rand()%5+1) * (2*float(rand())/RAND_MAX-1);
        particle_system.addParticle( player.pos+vec3(0, 5, 0), vec3(vx, vy, vz) );
    }

    // get new view matrix, upload P, V to all shaders
    m_view = camera.getViewMatrix();
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

    camera.getViewMatrix();
    terrain.render();
    player.render();
    particle_system.render();
    skybox.render();
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

        float dx = xPos - ctrls.xPos;
        float dy = yPos - ctrls.yPos;

        if ( ctrls.mouse_left_down ) {
            camera.updateYaw( dx );
            camera.updatePitch( dy );
        }

        ctrls.xPos = xPos;
        ctrls.yPos = yPos;
    }

    return eventHandled;
}

bool Winter::mouseButtonInputEvent ( int button, int actions, int mods) {
    bool eventHandled(false);

    if ( !ImGui::IsMouseHoveringAnyWindow() ) {
        if ( actions == GLFW_PRESS ) {
            if ( button == GLFW_MOUSE_BUTTON_LEFT ) {
                ctrls.mouse_left_down = true;
                eventHandled = true;
            }
        }

        if ( actions == GLFW_RELEASE ) {
            if ( button == GLFW_MOUSE_BUTTON_LEFT ) {
                ctrls.mouse_left_down = false;
                eventHandled = true;
            }
        }
    }

    return eventHandled;
}

bool Winter::mouseScrollEvent ( double xOffSet, double yOffSet) {
    bool eventHandled(true);
    camera.updateZoom(yOffSet);
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
