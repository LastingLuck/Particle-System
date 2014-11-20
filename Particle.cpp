#include "Particle.h"

Particle::Particle() {
    lifetime = 5.0f;
    curLife = 0.0f;
    ltrand = 0.05;
    gravity = 9.8f;
    pos = glm::vec3(0);
    vel = glm::vec3(1);
    col = glm::vec3(0);
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, 
        const glm::vec3& color) {
            
    lifetime = 5.0f;
    curLife = 0.0f;
    ltrand = 0.05;
    gravity = 9.8f;
    pos = position;
    vel = velocity;
    col = color;
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
        float life, float liferand, float grav) {
            
    lifetime = life;
    curLife = 0.0f;
    ltrand = liferand;
    gravity = grav;
    pos = position;
    vel = velocity;
    col = color;
}

bool Particle::update() {
    pos += vel;
    
}

Emitter::Emitter() {
    numParticles = 20;
    run = true;
}

Emitter::Emitter(bool start) {
    numParticles = 20;
    run = start;
}

Emitter::Emitter(int numPerSec) {
    numParticles = numPerSec;
    run = true;
}

Emitter::Emitter(int numPerSec, bool start) {
    numParticles = numPerSec;
    run = start;
}

void Emitter::update() {
    //Update current particles
    int len = particles.size();
    for(int i = 0; i < len; i++) {
        if(curParticles[i]) {
            if(particles[i].update());
        }
    }
    //Spawn new particles
    if(run) {
        
    }
}

void Emitter::pause() {
    run = false;
}

void Emitter::resume() {
    run = true;
}
