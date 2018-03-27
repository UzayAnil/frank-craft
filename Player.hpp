#pragma once
#include <glm/glm.hpp>
#include "Util.hpp"
#include "Puppet.hpp"
#include "Chunk.hpp"
#include "Particle.hpp"
#include "Cube.hpp"
#include "Sound.hpp"

class Player {
public:
    Player();
    ~Player();
    void init( std::string file );

    void render();
    void updateUniform( const glm::mat4 &P, const glm::mat4 &V, LightSource m_light, const glm::vec3 &ambientIntensity );

    void move( Controls &ctrls, float delta_time, SuperChunk &terrain );
    void checkInput( Controls &ctrls );

    void updatePitch( float delta );
    void updateYaw( float delta );

public:
    float yaw;
    float pitch;
    glm::vec3 pos;
    glm::vec3 dir;
    bool show_bounding;

private:
    static constexpr float MOVE_SPEED = 20;
    static constexpr float TURN_SPEED = 150;
    static constexpr float JUMP_POWER = 2;

    static constexpr float MIN_PITCH = -89;
    static constexpr float MAX_PITCH = 89;
    static constexpr float PITCH_SPEED = -0.1;
    static constexpr float YAW_SPEED = 0.1;

    float move_speed;
    float turn_speed;
    float up_speed;
    int jump_cnts;
    glm::mat4 M;
    Puppet puppet;
    Cube boundingBox;
    Audio *audio;
};
