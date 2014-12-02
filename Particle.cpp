#include "Particle.h"
#include <cstdio>

#define MSEC_TO_SEC .001f
#define SEC_TO_MSEC 1000

extern float lastTime;
extern float curTime;
extern float difTime;

Particle::Particle() {
    curLife = 0.0f;
    float random = (float)rand() / RAND_MAX;
    //random *= ltrand;
    lifetime = -1;
    //printf("Lifetime: %f\n", lifetime);
    //lifetime = 5.0f;
    gravity = glm::vec3(0, -9.8f, 0);
    pos = glm::vec3(0);
    //dir = glm::vec3(0, 1, 0);
    vel = glm::vec3(0, 5.0f, 0);
    termVel = -5.0f;
    col = glm::vec3(.5) + (random - 0.5f) * 0.3f;
    //col = glm::vec3(.5);
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, 
        const glm::vec3& color) {
            
    curLife = 0.0f;
    float random = (float)rand() / RAND_MAX;
    lifetime = -1;
    //printf("Lifetime: %f\n", lifetime);
    //lifetime = 5.0f;
    gravity = glm::vec3(0, -9.8f, 0);
    pos = position;
    //dir = glm::normalize(velocity);
    vel = velocity;
    #ifdef DEBUG
    printf("Starting Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    //printf("Starting Dir: (%f %f %f)\n", dir.x, dir.y, dir.z);
    #endif
    termVel = -5.0f;
    col = color + (random - 0.5f) * 0.3f;
    //col = color;
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
        float life, float liferand, const glm::vec3& grav) {
            
    curLife = 0.0f;
    float random = (float)rand() / RAND_MAX;
    lifetime = 5.0f + (random - 0.5f) * liferand;
    //printf("Lifetime: %f\n", lifetime);
    //lifetime = life;
    gravity = grav;
    pos = position;
    //dir = glm::normalize(velocity);
    vel = velocity;
    #ifdef DEBUG
    printf("Starting Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    //printf("Starting Dir: (%f %f %f)\n", dir.x, dir.y, dir.z);
    #endif
    termVel = -5.0f;
    col = color;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
    //printf("Color: (%f %f %f)\n", col.x, col.y, col.z);
    //col = color;
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
         float life, float liferand, float colrand, float dirrand, const glm::vec3& grav)  {
    
    curLife = 0.0f;
    float random = (float)rand() / RAND_MAX;
    lifetime = 5.0f + (random - 0.5f) * liferand;
    //printf("Lifetime: %f\n", lifetime);
    //lifetime = life;
    gravity = grav;
    pos = position;
    //dir = glm::normalize(velocity);
    vel = velocity;
    #ifdef DEBUG
    printf("Starting Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    //printf("Starting Dir: (%f %f %f)\n", dir.x, dir.y, dir.z);
    #endif
    termVel = -5.0f;
    col = color + (random - 0.5f) * colrand;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
    //printf("Color: (%f %f %f)\n", col.x, col.y, col.z);
    //col = color;
}

bool Particle::update() {
    //pos += dir*(vel*difTime);
    pos += (vel*difTime);
    #ifdef DEBUG
    printf("Pos: (%f %f %f)\n", pos.x, pos.y, pos.z);
    #endif
    vel += gravity * difTime;
    #ifdef DEBUG
    printf("Updating Vel With (%f %f %f)\n", (gravity*difTime).x, (gravity*difTime).y, (gravity*difTime).z);
    printf("New Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    #endif
    if(vel.y < termVel) {
        vel.y = termVel;
    }
    curLife += difTime;
    if(curLife >= lifetime) {
        return false;
    }
    return true;
}

Emitter::Emitter() {
    numParticles = 20;
    run = true;
    refTime = 1.0f;
    time = 0;
    pos = glm::vec3(0);
    dir = glm::vec3(0, 1, 0);
    col = glm::vec3(.5, .5, .5);
    vel = 5.0f;
    plimit = numParticles * refTime + (int)(numParticles * 0.1f);
    tottime = 0;
    plife = 0.5f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
}

Emitter::Emitter(bool start) {
    numParticles = 20;
    run = start;
    refTime = 1.0f;
    time = 0;
    pos = glm::vec3(0);
    dir = glm::vec3(0, 1, 0);
    col = glm::vec3(.5, .5, .5);
    vel = 5.0f;
    plimit = numParticles * refTime + (int)(numParticles * 0.1f);
    tottime = 0;
    plife = 0.5f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
}

Emitter::Emitter(int numPerSec) {
    numParticles = numPerSec;
    run = true;
    refTime = 1.0f;
    time = 0;
    pos = glm::vec3(0);
    dir = glm::vec3(0, 1, 0);
    col = glm::vec3(.5, .5, .5);
    vel = 5.0f;
    plimit = numParticles * refTime + (int)(numParticles * 0.1f);
    tottime = 0;
    plife = 0.5f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
}

Emitter::Emitter(int numPerSec, bool start) {
    numParticles = numPerSec;
    run = start;
    refTime = 1.0f;
    time = 0;
    pos = glm::vec3(0);
    dir = glm::vec3(0, 1, 0);
    col = glm::vec3(.5, .5, .5);
    vel = 5.0f;
    plimit = numParticles * refTime + (int)(numParticles * 0.1f);
    tottime = 0;
    plife = 0.5f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
}

void Emitter::update() {
    if(paused) {
        return;
    }
    #ifdef DEBUG
    printf("Emitter Update\n");
    printf("Current Number of Particles: %d\n", (signed)particles.size());
    #endif
    //Update current particles
    int len = particles.size();
    ///TODO - Possibly Parallelize?
    for(int i = 0; i < len; i++) {
        if(curParticles[i]) {
            if(!particles[i].update()) {
                curParticles[i] = false;
            }
        }
    }
    //Spawn new particles
    if(run) {
        int newPartNum = 0;
        #ifdef DEBUG
        printf("tottime: %f\n", tottime);
        printf("numPart: %f\n", numParticles*MSEC_TO_SEC);
        #endif
        if(tottime >= 1.0f/numParticles) {
            #ifdef DEBUG
            printParticlePos();
            #endif
            newPartNum = (int)(tottime / (1.0f/numParticles));
            tottime = fmod(tottime, (1.0f/numParticles));
        }
        for(int i = 0; i < newPartNum; i++) {
            particles.push_back(Particle(pos, dir*vel, col, plife, liferand, colrand, dirrand, glm::vec3(0, -9.8f, 0)));
            curParticles.push_back(true);
        }
    }
    time += difTime;
    tottime += difTime;
    if(time >= refTime || (signed)particles.size() > plimit) {
        refactor();
        time = 0;
    }
}

void Emitter::pause() {
    paused = true;
}

void Emitter::resume() {
    paused = false;
}

void Emitter::start() {
    run = true;
}

void Emitter::stop() {
    run = false;
}

void Emitter::refactor() {
    int pn = particles.size();
    std::vector<Particle> newparts;
    newparts.reserve(pn);
    for(int i = 0; i < pn; i++) {
        if(curParticles[i]) {
            newparts.push_back(particles[i]);
        }
    }
    std::vector<bool> newactive(newparts.size(), true);
    particles = newparts;
    curParticles = newactive;
}

void Emitter::printParticlePos() {
    for(int i = 0; i < (signed)particles.size(); i++) {
        if(curParticles[i]) {
            glm::vec3 p = particles[i].getPosition();
            printf("Pos %d: (%f %f %f)\n", i, p.x, p.y, p.z);
        }
    }
    printf("\n");
}
