#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <vector>
//#include "glm/glm.hpp"

#include <GL/glew.h>   //Include order can matter here
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Intersection {
    public:
        glm::vec3 pos;
        glm::vec3 norm;
};

class Particle {
    public:
        Particle();
        Particle(const glm::vec3& position, const glm::vec3& velocity, 
                 const glm::vec3& color);
        Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
                 float life, float liferand, const glm::vec3& grav);
        Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
                 float life, float liferand, float colrand, float dirrand, const glm::vec3& grav);
        
        bool update(float* vertices, int size); //< returns false if the particle is to be destroyed
        
        void setLifetime(float life) { lifetime = life; }
        //void setLifeRandomness(float rand) { ltrand = rand; }
        void setGravity(float grav) { gravity = glm::vec3(0, grav, 0); }
        void setPosition(const glm::vec3& position) { pos = position; }
        void setVelocity(const glm::vec3& velocity) { vel = velocity; dir = glm::normalize(velocity); }
        void setColor(const glm::vec3& color) { col = color; }
        
        float getLifetime() const { return lifetime; }
        //float getLifeRandomness() const { return ltrand; }
        glm::vec3 getGravity() const { return gravity; }
        glm::vec3 getPosition() const { return pos; }
        glm::vec3 getDirection() const { return dir; }
        glm::vec3 getVelocity() const { return vel; }
        glm::vec3 getColor() const { return col; }
    private:
        Intersection* intersect(const glm::vec3& pos, const glm::vec3& dir, float* verts, int size);
    
        float termVel;
        float curLife;
        float lifetime;
        glm::vec3 gravity;
        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 vel;
        glm::vec3 col;
};

class Emitter {
    public:
        Emitter();
        Emitter(bool start);
        Emitter(int numPerSec);
        Emitter(int numPerSec, bool start);
        
        void render();
        void update(float* vertices, int size);
        void pause();
        void resume();
        void stop();
        void start();
        bool isPaused() { return paused; }
        bool isRunning() { return run; }
        
        void setParticleRate(float numPerSec) { numParticles = numPerSec; }
        void setDirection(const glm::vec3& direction) { dir = glm::normalize(direction); }
        void setPosition(const glm::vec3& position) { pos = position; }
        void setColor(const glm::vec3& color) { col = color; }
        void setVelocity(float velocity) { vel = velocity; }
        void setParticleLife(float life) { plife = life; }
        void setLifeRand(float rand) { liferand = rand; }
        void setColorRand(float rand) { colrand = rand; }
        void setDirectionRand(float rand) { dirrand = rand; }
        void setShaderProgram(int prog) { shaderProgram = prog; }
        
        float getParticleRate() const { return numParticles; }
        glm::vec3 getPosition() const { return pos; }
        glm::vec3 getDirection() const { return dir; }
        glm::vec3 getColor() const { return col; }
        float getVelocity() const { return vel; }
        float getLifeTime() const { return plife; }
        float getLifeRand() const { return liferand; }
        float getColorRand() const { return colrand; }
        float getDirectionRand() const { return dirrand; }
        
        std::vector<Particle> getParticles() const { return particles; }
        std::vector<bool> getLiveList() const { return curParticles; }
    private:
        std::vector<bool> curParticles;
        std::vector<Particle> particles;
        float plife;
        float numParticles;
        bool run;
        bool paused;
        float lastTime;
        
        int shaderProgram;
        
        //emitter data
        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 col;
        float vel;
        
        //Particle refactor
        int plimit;
        float refTime;
        float time;
        float tottime;
        
        //Randomness
        float liferand;
        float colrand;
        float dirrand;
        
        void refactor();
        void printParticlePos();
};

#endif
