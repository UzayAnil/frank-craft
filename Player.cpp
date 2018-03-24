#include "Util.hpp"
#include "Player.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
using namespace glm;
using namespace std;

Player::Player()
    : angle( 0 ), move_speed( 0 ), turn_speed( 0 ), up_speed( 0 ), jump_cnts( 3 ),
    pos(2, 5, 2), dir( 0, 0, 1 ) {
    M = translate( M, pos );
}

void Player::init( std::string file ) {
    puppet.init( file );
}

void Player::move( Controls &ctrls, float delta_time, Chunk &terrain ) {
    checkInput( ctrls );

    // turn
    float d_angle = turn_speed * delta_time;
    angle += d_angle;
    M = rotate( M, degreesToRadians(d_angle), vec3(0,1,0) );

    // move
    float dis = move_speed * delta_time;
    dir = normalize(vec3(transpose(inverse(M)) * vec4(0,0,1,1) ));
    vec3 d_pos = dis * dir;
    pos += d_pos;
    M = translate( M, dis*vec3(0,0,1) );

    if ( fabs(move_speed) > ggEps ) {
        puppet.animate( move_speed > 0 ? delta_time : -delta_time );
    }

    // fall
    up_speed -= delta_time * GRAVITY;
    float new_y = pos.y + up_speed;
    if ( new_y < 0 ) new_y = 0;
    if ( new_y < pos.y && terrain.get( floor(pos.x), floor(new_y), floor(pos.z) ) ) {
        new_y = floor(new_y)+1;
        up_speed = 0;
        jump_cnts = 3;
    }
    M = translate( M, vec3(0, new_y-pos.y, 0) );
    pos.y = new_y;
}

void Player::checkInput( Controls &ctrls ) {
    if ( ctrls.W ) {
        move_speed = MOVE_SPEED;
    } else if ( ctrls.S ) {
        move_speed = -MOVE_SPEED;
    } else {
        move_speed = 0;
    }

    if ( ctrls.A ) {
        turn_speed = TURN_SPEED;
    } else if ( ctrls.D ) {
        turn_speed = -TURN_SPEED;
    } else {
        turn_speed = 0;
    }

    if ( ctrls.Space && jump_cnts > 0 ) {
        up_speed = JUMP_POWER;
        jump_cnts--;
        ctrls.Space = false;
    }

}

void Player::updateUniform( const glm::mat4 &P, const glm::mat4 &V, const LightSource &light, const vec3 &ambientIntensity ) {
    puppet.updateUniform( P, V*M, light, ambientIntensity );
}

void Player::render() {
    puppet.render();
}

//mat4 Player::getViewMatrix() {
    //m_view = glm::lookAt(
        //glm::vec3( 0.0f, 2.0f, -float(4)*2.0*M_SQRT1_2 ),
        //glm::vec3( 0.0f, 2.0f, 0.0f ),
        //glm::vec3( 0.0f, 1.0f, 0.0f ) );
//}
