#include "Winter.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <imgui/imgui.h>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>

using namespace std;
using namespace glm;

static bool show_gui = true;
static bool show_particles = false;

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

    shadow_texture = new Texture( 1024, 1024, Texture::Type::TEXDEPTH );
    shadow_buffer = new FrameBuffer( shadow_texture );

    last_frame_time = getTime();
}


void Winter::initPerspectiveMatrix() {
    float aspect = ((float)m_windowWidth) / m_windowHeight;
    //P = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
    P = glm::perspective(degreesToRadians(60.0f), aspect, 1.0f, 1000.0f);
}

void Winter::initViewMatrix() {
    //V = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            //vec3(0.0f, 1.0f, 0.0f));
    V = glm::lookAt(
        glm::vec3( 0.0f, 2.0f, -float(4)*2.0*M_SQRT1_2 ),
        glm::vec3( 0.0f, 2.0f, 0.0f ),
        glm::vec3( 0.0f, 1.0f, 0.0f ) );
    //V = V * rotate( mat4(), -float(degreesToRadians(110)), vec3( 1, 0, 0 ) );
    //V = V * translate( mat4(), vec3(-20, -80, 0) );
}

void Winter::initLightSources() {
    // World-space position
    m_light.position = vec3(290, 490, -220);
    m_light.rgbIntensity = vec3(0.8f); // White light
}

void Winter::uploadShadowUniforms() {

    float near_plane = 300.0f, far_plane = 700.0f;
    mat4 PP = ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

    mat4 VV = lookAt( m_light.position,
          player.pos,
          vec3( 0.0f, 1.0f,  0.0f));

    vec3 ambientIntensity(0.5f);

    player.updateUniform( PP, VV, m_light, ambientIntensity );
    terrain.updateUniform( PP, VV );
    particle_system.updateUniform( PP, VV );
    skybox.updateUniform( PP, VV );
}

void Winter::uploadCommonSceneUniforms() {
    vec3 ambientIntensity(0.5f);

    player.updateUniform( P, V, m_light, ambientIntensity );
    terrain.updateUniform( P, V );
    particle_system.updateUniform( P, V );
    skybox.updateUniform( P, V );
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


    if ( show_particles ) {
        for ( int i = 0; i < rand()%5; i++ ) {
            float vx = (rand()%5+1)* (2*float(rand())/RAND_MAX-1);
            float vy = (rand()%5+1)* (2*float(rand())/RAND_MAX-1);
            float vz = (rand()%5+1) * (2*float(rand())/RAND_MAX-1);
            particle_system.addParticle( player.pos+vec3(0, 5, 0), vec3(vx, vy, vz) );
        }
    }

    // get new view matrix, upload P, V to all shaders
    V = camera.getViewMatrix();
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
        if( ImGui::Button( "Toggle Bounding Box" ) ) { 
            player.show_bounding = !player.show_bounding;
        }
        if( ImGui::Button( "Toggle Particle Effect" ) ) { 
            show_particles = !show_particles;
        }

    ImGui::End();
}

/*
 * Called once per frame, after guiLogic().
 */
void Winter::draw() {
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE);

    // shadow framebuffer filling
    {
        glViewport( 0, 0, 1024, 1024 );
        shadow_buffer->bind();
            glClear( GL_DEPTH_BUFFER_BIT );
            uploadShadowUniforms();
            terrain.render();
            player.render();
        shadow_buffer->unbind();
    }


    // regular rendering
    {
        glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.getViewMatrix();

        uploadCommonSceneUniforms();
        terrain.render();
        player.render();
        particle_system.render();
        skybox.render();
    }
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
