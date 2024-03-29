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
    termVel = 25.0f;
    col = glm::vec3(.5);
    bounce = 0.7f;
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
    termVel = 25.0f;
    col = color;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
    bounce = 0.7f;
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
    termVel = 25.0f;
    col = color;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
    bounce = 0.7f;
}

Particle::Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
         float life, float liferand, float colrand, float dirrand, const glm::vec3& grav, float bouncy, float termvel)  {
    
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
    termVel = termvel;
    col = color + (random*2.0f - 1.0f) * colrand;
    col.x = glm::clamp(col.x, 0.0f, 1.0f);
    col.y = glm::clamp(col.y, 0.0f, 1.0f);
    col.z = glm::clamp(col.z, 0.0f, 1.0f);
    bounce = glm::clamp(bouncy, 0.0f, 1.0f);
}

bool Particle::update(float* vertices, int size, const std::vector<glm::mat4>& model, int stride, bool grav, const glm::vec3& force) {
    //Nothing to check collisions with
    if(vertices == 0 || size < 1) {
        pos += (vel*difTime);
        #ifdef DEBUG
        printf("Pos: (%f %f %f)\n", pos.x, pos.y, pos.z);
        #endif
        if(grav) {
            vel += gravity * difTime;
        }
        vel += force * difTime;
        #ifdef DEBUG
        printf("Updating Vel With (%f %f %f)\n", (gravity*difTime).x, (gravity*difTime).y, (gravity*difTime).z);
        printf("New Vel: (%f %f %f)\n", vel.x, vel.y, vel.z);
        #endif
    }
    //Check collisions
    else {
        //get projected position
        glm::vec3 newpos = pos + (vel * difTime);
        glm::vec3 newvel = vel;
        if(grav) {
            newvel = vel + (gravity * difTime);
        }
        newvel += force * difTime;
        //Check ray to projected position for collision
        glm::vec3 raypos = pos;
        glm::vec3 raydir = newpos - pos;
        Intersection *is = 0, *closest = 0;
        glm::vec3 tri[3];
        int mindex = 0, tmpindex = 0;
        for(int i = 0; i < size*(signed)model.size(); i+=9) {
            if(tmpindex >= stride) {
                tmpindex = 0;
                mindex++;
            }
            tmpindex+=3;
            tri[0] = glm::vec3(model[mindex] * glm::vec4(vertices[i%size], vertices[(i+1)%size], vertices[(i+2)%size], 1.0f));
            tri[1] = glm::vec3(model[mindex] * glm::vec4(vertices[(i+3)%size], vertices[(i+4)%size], vertices[(i+5)%size], 1.0f));
            tri[2] = glm::vec3(model[mindex] * glm::vec4(vertices[(i+6)%size], vertices[(i+7)%size], vertices[(i+8)%size], 1.0f));
            if((is = intersect(raypos, raydir, tri))) {
                if(closest == 0) {
                    closest = new Intersection();
                    closest->pos = is->pos;
                    closest->norm = is->norm;
                    closest->dsq = is->dsq;
                }
                else if(is->dsq < closest->dsq){
                    closest->pos = is->pos;
                    closest->norm = is->norm;
                    closest->dsq = is->dsq;
                }
            }
            if(is) {
                delete is;
            }
        }
        if(closest) {
            newpos = pos + (closest->pos - pos) * (1 - 0.2f*glm::length(closest->pos));
            glm::vec3 bvel = glm::reflect(newvel, closest->norm) * bounce;
            if(glm::length(bvel) < 1.0f) {
                //Cross normal and gravity
                //Cross that with normal
                //project velocity onto it
                glm::vec3 slidevec = glm::cross(closest->norm, gravity);
                if(slidevec.x == 0 && slidevec.y == 0 && slidevec.z == 0) {
                    newvel = glm::vec3(0);
                }
                else {
                    slidevec = glm::cross(slidevec, closest->norm);
                    newvel = (glm::dot(newvel, slidevec) / glm::dot(slidevec, slidevec)) * slidevec;
                }
            }
            else {
                newvel = bvel;
            }
            delete closest;
        }
        pos = newpos;
        vel = newvel; 
    }
    
    if(glm::length(vel) > termVel) {
        vel = glm::normalize(vel) * termVel;
        if(force.x != 0.0f && force.y != 0.0f && force.z != 0.0f) {
            vel += force * difTime;
        }
    }
    if(lifetime >= 0) {
        curLife += difTime;
        if(curLife >= lifetime) {
            return false;
        }
    }
    return true;
}

Intersection* Particle::intersect(const glm::vec3& pos, const glm::vec3& dir, glm::vec3 verts[]) {
    Intersection* i = 0;
    
    glm::vec3 t1 = verts[0];
    glm::vec3 t2 = verts[1];
    glm::vec3 t3 = verts[2];
    glm::vec3 dirn = glm::normalize(dir);
    glm::vec3 ae = t1 - pos;
    glm::vec3 ab = t1 - t2;
    glm::vec3 ac = t1 - t3;
    float detA = ab.x*(ac.y*dirn.z-dirn.y*ac.z) + ab.y*(ac.z*dirn.x-dirn.z*ac.x) + ab.z*(ac.x*dirn.y-ac.y*dirn.x);
    float dett = ac.z*(ab.x*ae.y-ab.y*ae.x) + ac.y*(ae.x*ab.z-ab.x*ae.z) + ac.x*(ab.y*ae.z-ab.z*ae.y);
    float tval = dett / detA;
    if(tval > 0.001f) {
        return 0;
    }
    float detb = ae.x*(ac.y*dirn.z-dirn.y*ac.z) + ae.y*(ac.z*dirn.x-dirn.z*ac.x) + ae.z*(ac.x*dirn.y-ac.y*dirn.x);
    float detg = dirn.z*(ab.x*ae.y-ae.x*ab.y) + dirn.y*(ab.z*ae.x-ae.z*ab.x) + dirn.x*(ab.y*ae.z-ae.y*ab.z);
    float beta = detb / detA;
    float gamma = detg / detA;
    if(beta >= 0 && beta <= 1 && gamma >= 0  && gamma <= 1 && beta + gamma <= 1) {
        glm::vec3 point = t1 + (((t2 - t1) * beta) + ((t3 - t1) * gamma));
        glm::vec3 tmp = point - pos;
        float dist = glm::dot(tmp, tmp);
        if(dist < 0.0f || dist >= glm::dot(dir, dir)) {
            return 0;
        }
        glm::vec3 normal = glm::cross(t2-t1, t3-t1);
        normal = glm::normalize(normal);
        float ang = glm::dot(normal, dirn);
        if(ang > 0) {
            normal = normal * -1.0f;
        }
        i = new Intersection();
        i->pos = point;
        i->norm = normal;
        i->dsq = dist;
    }
    return i;
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
    bratio = 0.0f;
    tvel = 25.0f;
    grav = glm::vec3(0, -9.8f, 0);
    
    //// Allocate Texture 0 (Snow) ///////
	SDL_Surface* surface = SDL_LoadBMP("particleTexture.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return;
    }
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    ///What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ///Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    useTex = false;
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
    bratio = 0.0f;
    tvel = 25.0f;
    grav = glm::vec3(0, -9.8f, 0);
    
    //// Allocate Texture 0 (Snow) ///////
	SDL_Surface* surface = SDL_LoadBMP("particleTexture.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return;
    }
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    ///What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ///Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    useTex = false;
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
    bratio = 0.0f;
    tvel = 25.0f;
    grav = glm::vec3(0, -9.8f, 0);
    
    //// Allocate Texture 0 (Snow) ///////
	SDL_Surface* surface = SDL_LoadBMP("particleTexture.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return;
    }
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    ///What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ///Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    useTex = false;
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
    bratio = 0.0f;
    tvel = 25.0f;
    grav = glm::vec3(0, -9.8f, 0);
    
    //// Allocate Texture 0 (Snow) ///////
	SDL_Surface* surface = SDL_LoadBMP("particleTexture.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return;
    }
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    ///What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ///Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    useTex = false;
}

void Emitter::update(float* vertices, int size, const std::vector<glm::mat4>& model, int stride, const glm::vec3& force) {
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
    #pragma omp parallel for schedule(static, 125)
    for(int i = 0; i < len; i++) {
        if(curParticles[i]) {
            //printf("Updating Particle at Pos: (%f %f %f)\n", particles[i].getPosition().x, particles[i].getPosition().y, particles[i].getPosition().z);
            if(!particles[i].update(vertices, size, model, stride, usegrav, force)) {
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
            particles.push_back(Particle(newpos, dir*vel, col, plife, liferand, colrand, dirrand, grav, bratio, tvel));
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
    //Bind texture for particles
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
    if(useTex) {
        glUniform1i(uniTexID, 0);
    }
    else {
        glUniform1i(uniTexID, -1);
    }
    //Draw Particles
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
    glUniform1i(uniTexID, -1);
    glm::vec3 axis = glm::cross(glm::vec3(0.f, 1.f, 0.f), dir);
    if(axis.x == 0 && axis.y == 0 && axis.z == 0) {
        axis = glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    model = glm::mat4();
    model = glm::translate(model, pos);
    model = glm::rotate(model, (float)acos(glm::dot(glm::vec3(0.f, 1.f, 0.f), dir)), axis);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(uniColor, col.x, col.y, col.z);
    glDrawArrays(GL_TRIANGLES, 1, vNum);
    //printf("Rendered %d particles\n", totpart);
    return totpart;
}
