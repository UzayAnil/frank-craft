#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "Camera.hpp"
#include "Chunk.hpp"
#include "Util.hpp"
#include "Player.hpp"
#include "Skybox.hpp"
#include "FrameBuffer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <set>


class Winter : public CS488Window {
public:
    Winter(const std::string & luaSceneFile);
    virtual ~Winter();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    //-- Virtual callback methods
    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

    void initPerspectiveMatrix();
    void initViewMatrix();
    void initLightSources();

    void uploadShadowUniforms();
    void uploadCommonSceneUniforms();

    glm::mat4 P;
    glm::mat4 V;

    double last_frame_time;
    double delta_time;

    LightSource m_light;
    const std::string &m_luaSceneFile;

    Player player;
    SuperChunk terrain;
    PlayerCamera camera;
    Controls ctrls;
    ParticleSystem particle_system;
    Skybox skybox;
    Texture *shadow_texture;
    FrameBuffer *shadow_buffer;
};
