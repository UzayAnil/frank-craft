#pragma once
#include <glm/glm.hpp>
#include "Player.hpp"

class PlayerCamera {
public:
    PlayerCamera( Player &player );

    void updateZoom( float delta );
    void updatePitch( float delta );
    void updateYaw( float delta );

    float getHorDis();
    float getVertDis();
    glm::vec3 getCamPos( float horDis, float vertDis );
    glm::mat4 getViewMatrix();

private:

    static constexpr float MIN_ZOOM = 10;
    static constexpr float MAX_ZOOM = 40;
    static constexpr float DEFAULT_ZOOM = 20;
    static constexpr float ZOOM_SPEED = 1.4;

    static constexpr float MIN_PITCH = -89;
    static constexpr float MAX_PITCH = 89;
    static constexpr float DEFAULT_PITCH = 20;
    static constexpr float PITCH_SPEED = 0.1;

    static constexpr float DEFAULT_YAW = 0;
    static constexpr float YAW_SPEED = 0.3;

    glm::vec3 pos;
    float zoomLevel;
    float dis_from_player;
    float angle_around_player;
    float pitch;
    float yaw;
    float roll;

    Player &player;
};
