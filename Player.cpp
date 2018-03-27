#include "Util.hpp"
#include "Player.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
using namespace glm;
using namespace std;

Player::Player()
    : dead( false ),
    yaw( 0 ), pitch( 0 ), move_speed( 0 ), turn_speed( 0 ), up_speed( 0 ), jump_cnts( 3 ),
    pos( SuperChunk::NCX*Chunk::SX/2, 300, SuperChunk::NCZ*Chunk::SZ/2), dir( 0, 0, 1 ),
    boundingBox( vec3(-.5, 1, -.5), vec3(.5, 3.2, .5), true ), show_bounding( false ) {
    M = translate( M, pos );
}

Player::~Player() {
    delete audio;
}

void Player::init( std::string file ) {
    puppet.init( file );
    boundingBox.init();
    audio = new Audio();
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

void Player::updatePitch( float delta ) {
    pitch += delta * PITCH_SPEED;
    pitch = clamp( pitch, MIN_PITCH, MAX_PITCH );
}

void Player::updateYaw( float delta ) {
    float d_angle = YAW_SPEED * delta;
    yaw += d_angle;
    M = rotate( M, degreesToRadians(d_angle), vec3(0,1,0) );
}

void Player::move( Controls &ctrls, float delta_time, SuperChunk &terrain ) {
    checkInput( ctrls );

    // turn
    float d_angle = turn_speed * delta_time;
    yaw += d_angle;
    M = rotate( M, degreesToRadians(d_angle), vec3(0,1,0) );

    // move
    float dis = move_speed * delta_time;
    dir = normalize(vec3(transpose(inverse(M)) * vec4(0,0,1,1) ));

    mat4 newM = translate( M, dis*vec3(0,0,1) );
    vec3 min( newM * vec4(boundingBox.min, 1) );
    vec3 max( newM * vec4(boundingBox.max, 1) );
    bool walking = true;
    if ( !collide( min, max, terrain ) ) {
        pos += dis * dir;
        M = newM;
    } else {
        walking = false;
    }

    if ( fabs(move_speed) > ggEps && walking ) {
        puppet.animate( move_speed > 0 ? delta_time : -delta_time );
    } else {
        walking = false;
    }

    // fall
    up_speed -= delta_time * 0.5 * GRAVITY;
    float new_y = pos.y + up_speed;

    newM = translate( M, vec3(0, new_y-pos.y, 0) );
    if ( downCollide( pos.y, new_y, min, max, terrain ) ) {

        if ( dead ) throw "game over";
        newM = translate( M, vec3(0, new_y-pos.y, 0) );
        if ( abs(new_y-pos.y) > ggEps )
            audio->land();
        up_speed = 0;
        jump_cnts = 3;
    }

    if( walking && abs(new_y-pos.y) < ggEps )
        audio->walk();

    pos.y = new_y;

    if ( !dead && fabs(pos.y-0)<ggEps && !terrain.checkInBound(pos) ) {
        dead = true;
        audio->death();
        up_speed = JUMP_POWER*2;
    }
    M = newM;
}

void Player::attack( SuperChunk &terrain, ParticleSystem &particle_system ) {

    if ( dead ) return;

    vec3 dirs[3];
    dirs[0] = normalize(vec3(transpose(inverse(M)) * vec4(0,0,1,1) ));
    dirs[1] = normalize(vec3( rotate(mat4(), degreesToRadians(2.5f), vec3(0,1,0)) * vec4(dirs[0],1)));
    dirs[2] = normalize(vec3( rotate(mat4(), degreesToRadians(-2.5f), vec3(0,1,0)) * vec4(dirs[0],1)));
    bool found = false;
    for ( float i = 0; i < 2 && !found; i++ ) {
        for ( float j = 1; j < 3 && !found; j++ ) {
            for ( int k = 0; k<3 && !found; k++ ) {
                vec3 dir = dirs[k];
                vec3 new_pos = pos+ j*vec3(0,1,0) + i*dir;

                float x = new_pos.x;
                float y = new_pos.y;
                float z = new_pos.z;

                if ( terrain.get(x,y,z) != BlockType::Empty ) {
                    terrain.set( x, y, z, BlockType::Empty );
                    for ( int i = 0; i < 100; i++ ) {
                        float vx = (rand()%8+1)* (2*float(rand())/RAND_MAX-1);
                        float vy = (rand()%8+1)* (2*float(rand())/RAND_MAX-1);
                        float vz = (rand()%8+1) * (2*float(rand())/RAND_MAX-1);
                        particle_system.addParticle( new_pos+vec3(.5,.5,.5), vec3(vx, vy, vz) );
                    }
                    found = true;
                }
            }
        }
    }
}

void Player::checkInput( Controls &ctrls ) {

    if ( dead ) return;

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
        audio->jump();
    }

}

void Player::updateUniform( const glm::mat4 &P, const glm::mat4 &V, LightSource light, const vec3 &ambientIntensity ) {
    light.position = vec3( V * vec4(light.position, 1) );
    puppet.updateUniform( P, V*M, light, ambientIntensity );
    {
        mat4 MM = translate(mat4(), vec3(M[3][0], M[3][1], M[3][2]) );
        boundingBox.updateUniform( P, V*MM );
    }
}

void Player::render() {
    puppet.render();
    if ( show_bounding ) {
        boundingBox.render();
    }
}
