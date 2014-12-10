#include "Particle.h"
#include <cstdio>

#define MSEC_TO_SEC .001f
#define SEC_TO_MSEC 1000

extern float lastTime;
extern float curTime;
extern float difTime;
extern int vNum;

Particle::Particle() {
    curLife = 0.0f;
    lifetime = -1;
    //printf("Lifetime: %f\n", lifetime);
    gravity = glm::vec3(0, -9.8f, 0);
    pos = glm::vec3(0);
    vel = glm::vec3(0, 5.0f, 0);
    termVel = -25.0f;
    col = glm::vec3(.5);
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, 
        const glm::vec3& color) {
            
    curLife = 0.0f;
    lifetime = -1;
    gravity = glm::vec3(0, -9.8f, 0);
    pos = position;
    vel = velocity;
    #ifdef DEBUG
    printf("Starting Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    #endif
    termVel = -25.0f;
    col = color;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
        float life, float liferand, const glm::vec3& grav) {
            
    curLife = 0.0f;
    float random = (float)rand() / RAND_MAX;
    lifetime = life + (random * 2.0f - 1.0f) * liferand;
    gravity = grav;
    pos = position;
    vel = velocity;
    #ifdef DEBUG
    printf("Starting Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    #endif
    termVel = -25.0f;
    col = color;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
         float life, float liferand, float colrand, float dirrand, const glm::vec3& grav)  {
    
    curLife = 0.0f;
    float random = (float)rand() / RAND_MAX;
    lifetime = life + (random * 2.0f - 1.0f) * liferand;
    gravity = grav;
    pos = position;
    float rtheta = ((random * 4 * M_PI) - (2 * M_PI)) * dirrand;
    //Make a random vector and take the cross product of that and velocity. This gives
    //us a random vector that's perpendicular to velocity. Rotate by a random amount around that axis
    vel = velocity;
    vel = glm::rotate(vel, rtheta, glm::cross(velocity, glm::vec3((float)rand()/RAND_MAX*2-1, (float)rand()/RAND_MAX*2-1, (float)rand()/RAND_MAX*2-1)));
    #ifdef DEBUG
    printf("Starting Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
    #endif
    termVel = -25.0f;
    col = color + (random*2.0f - 1.0f) * colrand;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
}

bool Particle::update(float* vertices, int size, bool grav) {
    //Nothing to check collisions with
    if(vertices == 0) {
        pos += (vel*difTime);
        #ifdef DEBUG
        printf("Pos: (%f %f %f)\n", pos.x, pos.y, pos.z);
        #endif
        if(grav) {
            vel += gravity * difTime;
        }
        #ifdef DEBUG
        printf("Updating Vel With (%f %f %f)\n", (gravity*difTime).x, (gravity*difTime).y, (gravity*difTime).z);
        printf("New Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
        #endif
    }
    //Check collisions
    else {
        //get projected position
        glm::vec3 newpos = pos + (vel * difTime);
        glm::vec3 newvel = vel + (gravity * difTime);
        //Check ray to projected position for collision
        //glm::vec3 raypos = pos;
        //glm::vec3 raydir = newpos - pos;
        pos = newpos;
        vel = newvel;
    }
    
    if(vel.y < termVel) {
        vel.y = termVel;
    }
    if(lifetime >= 0) {
        curLife += difTime;
        if(curLife >= lifetime) {
            //printf("Life: %f\nCur Life: %f\n", lifetime, curLife);
            return false;
        }
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
    plife = 5.0f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
    usegrav = true;
    rad = 0.0f;
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
    plife = 5.0f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
    usegrav = true;
    rad = 0.0f;
}

Emitter::Emitter(int numPerSec) {
    numParticles = numPerSec;
    run = true;
    refTime = 3.0f;
    time = 0;
    pos = glm::vec3(0);
    dir = glm::vec3(0, 1, 0);
    col = glm::vec3(.5, .5, .5);
    vel = 5.0f;
    plimit = numParticles * refTime + (int)(numParticles * 0.1f);
    tottime = 0;
    plife = 5.0f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
    usegrav = true;
    rad = 0.0f;
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
    plife = 5.0f;
    liferand = 0.0f;
    colrand = 0.0f;
    dirrand = 0.0f;
    usegrav = true;
    rad = 0.0f;
}

void Emitter::update(float* vertices, int size) {
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
            if(!particles[i].update(0, 0, usegrav)) {
                curParticles[i] = false;
                //printf("Killed\n");
            }
        }
    }
    //Spawn new particles
    glm::vec3 newpos;
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
            newpos = pos + glm::cross(dir, glm::vec3((float)rand()/RAND_MAX*2-1, (float)rand()/RAND_MAX*2-1, (float)rand()/RAND_MAX*2-1)) * rad;
            //newpos = pos;
            particles.push_back(Particle(newpos, dir*vel, col, plife, liferand, colrand, dirrand, glm::vec3(0, -9.8f, 0)));
            curParticles.push_back(true);
        }
        tottime += difTime;
    }
    time += difTime;
    
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

int Emitter::render() {
    int len = particles.size();
    int totpart = 0;
    Particle p;
    glm::vec3 color;
    glm::mat4 model;
    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    
    for(int i = 0; i < len; i++) {
        if(curParticles[i]) {
            p = particles[i];
            color = p.getColor();
            glUniform3f(uniColor, color.x, color.y, color.z);
            
            model = glm::mat4();
            model = glm::translate(model, p.getPosition());
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_POINTS, 0, 1);
            totpart++;
        }
    }
    glm::vec3 axis = glm::cross(glm::vec3(0.f, 1.f, 0.f), dir);
    if(axis.x == 0 && axis.y == 0 && axis.z == 0) {
        axis = glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    model = glm::mat4();
    model = glm::rotate(model, (float)acos(glm::dot(glm::vec3(0.f, 1.f, 0.f), dir)), axis);
    model = glm::translate(model, pos+glm::vec3(0.0f, -0.05f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(uniColor, col.x, col.y, col.z);
    glDrawArrays(GL_TRIANGLES, 1, vNum);
    //printf("Rendered %d particles\n", totpart);
    return totpart;
}
