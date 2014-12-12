///TODO List
/// - Collision Detection   x
/// - Bouncing Particles
/// - Sliding Particles
/// - Get Geometry Shader working
/// - Keybindings for dynamic stuff
///     - Position
///     - Direction
///     - Randomness
/// - Demo with permanent particles.


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
        float dsq;
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
        
        bool update(float* vertices, int size, const std::vector<glm::vec3>& model, int stride, bool grav); //< returns false if the particle is to be destroyed
        
        void setLifetime(float life) { lifetime = life; }
        void setGravity(float grav) { gravity = glm::vec3(0, grav, 0); }
        void setPosition(const glm::vec3& position) { pos = position; }
        void setVelocity(const glm::vec3& velocity) { vel = velocity; }
        void setColor(const glm::vec3& color) { col = color; }
        
        float getLifetime() const { return lifetime; }
        glm::vec3 getGravity() const { return gravity; }
        glm::vec3 getPosition() const { return pos; }
        glm::vec3 getVelocity() const { return vel; }
        glm::vec3 getColor() const { return col; }
    private:
        Intersection* intersect(const glm::vec3& pos, const glm::vec3& dir, glm::vec3 verts[]);
    
        float termVel;
        float curLife;
        float lifetime;
        glm::vec3 gravity;
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
        
        int render();
        void update(float* vertices, int size, const std::vector<glm::vec3>& model, int stride);
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
        void toggleGravity() { usegrav = !usegrav; }
        void setRadius(float radius) { rad = clamp(radius, 0.0f, 100.0f); }
        
        float getParticleRate() const { return numParticles; }
        glm::vec3 getPosition() const { return pos; }
        glm::vec3 getDirection() const { return dir; }
        glm::vec3 getColor() const { return col; }
        float getVelocity() const { return vel; }
        float getLifeTime() const { return plife; }
        float getLifeRand() const { return liferand; }
        float getColorRand() const { return colrand; }
        float getDirectionRand() const { return dirrand; }
        bool usingGravity() const { return usegrav; }
        float getRadius() const { return rad; }
        
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
        bool usegrav;
        float rad;
        
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
        float clamp(float value, float min, float max) { return (value < min) ? min : (value > max) ? max : value; }
};

#endif
