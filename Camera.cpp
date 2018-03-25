#include "Camera.hpp"
#include "cs488-framework/MathUtils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
using namespace glm;
using namespace std;

PlayerCamera::PlayerCamera( Player &player ) :
    player( player ),
    dis_from_player( DEFAULT_ZOOM),
    yaw( DEFAULT_YAW ),
    pitch( DEFAULT_PITCH ) {}

void PlayerCamera::updateZoom( float delta_time ) {
    dis_from_player -= delta_time * ZOOM_SPEED;
    dis_from_player = clamp( dis_from_player, MIN_ZOOM, MAX_ZOOM );
}

void PlayerCamera::updatePitch( float delta_time ) {
    pitch += delta_time * PITCH_SPEED;
    pitch = clamp( pitch, MIN_PITCH, MAX_PITCH );
}

void PlayerCamera::updateYaw( float delta_time ) {
    yaw -= delta_time * YAW_SPEED;
    yaw = fmod( yaw, 360 );
}

float PlayerCamera::getHorDis() {
    return dis_from_player * cos(degreesToRadians(pitch));

}

float PlayerCamera::getVertDis() {
    return dis_from_player * sin(degreesToRadians(pitch));
}

glm::vec3 PlayerCamera::getCamPos( float horDis, float vertDis ) {

    float theta = degreesToRadians(player.angle + yaw);
    float dx = horDis * sin(theta);
    float dz = horDis * cos(theta);
    float dy = vertDis;

    pos.x = player.pos.x - dx;
    pos.z = player.pos.z - dz;
    pos.y = player.pos.y + 3 + vertDis;

    return pos;
}

mat4 PlayerCamera::getViewMatrix() {
    float horDis = getHorDis();
    float vertDis = getVertDis();
    vec3 from = getCamPos( horDis, vertDis );
    vec3 to = player.pos + vec3(0, 3, 0);
    vec3 up = vec3(0, 1, 0);
    return glm::lookAt( from, to, up );
}
