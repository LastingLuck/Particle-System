#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <vector>
#include "glm/glm.hpp"

class Particle {
    public:
        Particle();
        Particle(const glm::vec3& position, const glm::vec3& velocity, 
                 const glm::vec3& color);
        Particle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color,
                 float life, float liferand, float grav);
        
        bool update(); //< returns false if the particle is to be destroyed
        
        void setLifetime(float life) { lifetime = life; }
        void setLifeRandomness(float rand) { ltrand = rand; }
        void setGravity(float grav) { gravity = grav; }
        void setPosition(const glm::vec3& position) { pos = position; }
        void setVelocity(const glm::vec3& velocity) { vel = velocity; }
        void setColor(const glm::vec3& color) { col = color; }
        
        float getLifetime() const { return lifetime; }
        float getLifeRandomness() const { return ltrand; }
        float getGravity() const { return gravity; }
        glm::vec3 getPosition() const { return pos; }
        glm::vec3 getVelocity() const { return vel; }
        glm::vec3 getColor() const { return col; }
    private:
        float curLife;
        float lifetime;
        float ltrand; //< randomness in lifespan. 0-1. Percent to check lifespan for ending
        float gravity;
        glm::vec3 pos;
        glm::vec3 vel;
        glm::vec3 col;
};

class Emitter {
    public:
        Emitter();
        Emitter(bool start);
        Emitter(int numPerSec);
        Emitter(int numPerSec, bool start);
        
        void update();
        void pause();
        void resume();
        
        void setParticleRate(int numPerSec) { numParticles = numPerSec; }
        
        int getParticleRate() { return numParticles; }
    private:
        std::vector<bool> curParticles;
        std::vector<Particle> particles;
        int numParticles;
        bool run;
        float lastTime;
        
        void refactor();
};

#endif
