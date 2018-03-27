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
    pitch( DEFAULT_PITCH ),
    third_person_perspective( true ){}

void PlayerCamera::updateZoom( float delta ) {
    dis_from_player -= delta * ZOOM_SPEED;
    dis_from_player = clamp( dis_from_player, MIN_ZOOM, MAX_ZOOM );
}

void PlayerCamera::updatePitch( float delta ) {
    pitch += delta * PITCH_SPEED;
    pitch = clamp( pitch, MIN_PITCH, MAX_PITCH );
}

void PlayerCamera::updateYaw( float delta ) {
    yaw -= delta * YAW_SPEED;
    yaw = fmod( yaw, 360 );
}

float PlayerCamera::getHorDis() {
    return dis_from_player * cos(degreesToRadians(pitch));

}

float PlayerCamera::getVertDis() {
    return dis_from_player * sin(degreesToRadians(pitch));
}

glm::vec3 PlayerCamera::getCamPos( float horDis, float vertDis ) {

    float theta = degreesToRadians(player.yaw + yaw);
    float dx = horDis * sin(theta);
    float dz = horDis * cos(theta);
    float dy = vertDis;

    pos.x = player.pos.x - dx;
    pos.z = player.pos.z - dz;
    pos.y = player.pos.y + 3 + vertDis;

    return pos;
}

mat4 PlayerCamera::getViewMatrix() {
    if ( third_person_perspective ) {
        float horDis = getHorDis();
        float vertDis = getVertDis();
        vec3 from = getCamPos( horDis, vertDis );
        vec3 to = player.pos + vec3(0, 3, 0);
        vec3 up = vec3(0, 1, 0);
        return glm::lookAt( from, to, up );
    } else {

        float horDis = 1. * cos(degreesToRadians(player.pitch));
        float vertDis = 1. * sin(degreesToRadians(player.pitch));
        float dx = horDis * sin(degreesToRadians(player.yaw));
        float dz = horDis * cos(degreesToRadians(player.yaw));
        float dy = vertDis;
        vec3 dir(dx, dy, dz);


        vec3 from = player.pos + vec3(0, 5, 0);
        vec3 to = player.pos + vec3(0, 5, 0 ) + dir;
        vec3 up = vec3(0, 1, 0);
        return glm::lookAt( from, to, up );
    }
}
