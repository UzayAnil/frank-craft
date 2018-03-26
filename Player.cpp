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
    pos( SuperChunk::NCX*Chunk::SX/2, 300, SuperChunk::NCZ*Chunk::SZ/2), dir( 0, 0, 1 ),
    boundingBox( vec3(-.5, 1, -.5), vec3(.5, 3.2, .5), true ) {
    M = translate( M, pos );
}

void Player::init( std::string file ) {
    puppet.init( file );
    boundingBox.init();
}

bool collide( vec3 min, vec3 max, SuperChunk &terrain ) {
    int x_min = glm::min( min.x, max.x );
    int y_min = glm::min( min.y, max.y );
    int z_min = glm::min( min.z, max.z );
    int x_max = glm::max( min.x, max.x ) + 1;
    int y_max = glm::max( min.y, max.y ) + 1;
    int z_max = glm::max( min.z, max.z ) + 1;

    for ( int x = x_min; x<x_max; x++ ) {
        for ( int z = z_min; z<z_max; z++ ) {
            for ( int y = y_min; y<y_max; y++ ) {
                if ( terrain.get( x, y, z ) != BlockType::Empty ) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool downCollide( float old_y, float &new_y, vec3 min, vec3 max, SuperChunk &terrain ) {

    if ( new_y < 0 ) {
        new_y = 0;
        return true;
    }

    int x_min = glm::min( min.x, max.x );
    int z_min = glm::min( min.z, max.z );
    int x_max = glm::max( min.x, max.x ) + 1;
    int z_max = glm::max( min.z, max.z ) + 1;

    for ( int y = old_y; y >= new_y; y-- ) {
        for ( int x = x_min; x<x_max; x++ ) {
            for ( int z = z_min; z<z_max; z++ ) {
                if ( terrain.get( x, y, z ) != BlockType::Empty ) {
                    new_y = y + ggEps;
                    return true;
                }
            }
        }
    }
    return false;
}

void Player::move( Controls &ctrls, float delta_time, SuperChunk &terrain ) {
    checkInput( ctrls );

    // turn
    float d_angle = turn_speed * delta_time;
    angle += d_angle;
    M = rotate( M, degreesToRadians(d_angle), vec3(0,1,0) );

    // move
    float dis = move_speed * delta_time;
    dir = normalize(vec3(transpose(inverse(M)) * vec4(0,0,1,1) ));

    mat4 newM = translate( M, dis*vec3(0,0,1) );
    vec3 min( newM * vec4(boundingBox.min, 1) );
    vec3 max( newM * vec4(boundingBox.max, 1) );
    if ( !collide( min, max, terrain ) ) {
        pos += dis * dir;
        M = newM;
    }

    if ( fabs(move_speed) > ggEps ) {
        puppet.animate( move_speed > 0 ? delta_time : -delta_time );
    }

    // fall
    up_speed -= delta_time * 0.5 * GRAVITY;
    float new_y = pos.y + up_speed;

    newM = translate( M, vec3(0, new_y-pos.y, 0) );
    if ( downCollide( pos.y, new_y, min, max, terrain ) ) {
        newM = translate( M, vec3(0, new_y-pos.y, 0) );
        up_speed = 0;
        jump_cnts = 3;
    }
    pos.y = new_y;
    M = newM;
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
    {
        mat4 MM = translate(mat4(), vec3(M[3][0], M[3][1], M[3][2]) );
        boundingBox.updateUniform( P, V*MM );
    }
}

void Player::render() {
    puppet.render();
    boundingBox.render();
}
