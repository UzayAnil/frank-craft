#pragma once
#include <glm/glm.hpp>
#include "Util.hpp"
#include "Puppet.hpp"
#include "Cube.hpp"

class Player {
public:
    Player();
    void init( std::string file );

    void render();
    void updateUniform( const glm::mat4 &P, const glm::mat4 &V,  const LightSource &m_light, const glm::vec3 &ambientIntensity );

    void move( Controls &ctrls, float delta_time, Chunk &terrain );
    void checkInput( Controls &ctrls );

    //glm::mat4 getViewMatrix();

public:
    float angle;
    glm::vec3 pos;
    glm::vec3 dir;

private:
    static constexpr float MOVE_SPEED = 20;
    static constexpr float TURN_SPEED = 150;
    static constexpr float GRAVITY = -1.5;
    static constexpr float JUMP_POWER = 0.25;

    float move_speed;
    float turn_speed;
    float up_speed;
    int jump_cnts;
    glm::mat4 M;
    Puppet puppet;
};
