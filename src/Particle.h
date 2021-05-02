// Particle.h
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>

#pragma once

#define MAX_PARTICLES 10000

typedef float fVector[3];

enum dimensions{X,Y,Z};

struct Particles
{
   int type; //used only for "fire with smoke" to tell between fire and smoke 0=fire, 1=smoke, 2=lava
   float lifespan, age, scale, direction; // how long the particle will exist for, how old is it.
   fVector position; // initial on screen position
   fVector movement; // movement vector
   fVector color; // color values
   fVector pull; // compounding directional pull in the x,y,z directions
};

class Particle
{
   fVector particlePull;
   Particles particles[MAX_PARTICLES]; //initializes and array of type Particle
public:
   Particle(void);
   virtual ~Particle(void);
   void createParticle(Particles *p); //creates and initializes a single particle
   void createParticles(); //calls createParticle() to initialize all particles in PArticle system
   void updateParticles(); //updates particles according to forces being used
   void turnToSmoke(Particles *p); //called to turn dead fire into smoke
   void flowDown(Particles *p);
   void turnRed(Particles *p);
   void turnYellow(Particles *p);
   void turnGold(Particles *p);
   void turnLightYellow(Particles *p);

   int getNumOfParticles();
   float getXPos(int i);
   float getYPos(int i);
   float getZPos(int i);
   float getR(int i);
   float getG(int i);
   float getB(int i);
   float getScale(int i);
   float getDirection(int i);
   float getAlpha(int i); //returns how faded (according to age) the particle should be

   void modifyParticlePull(float x, float y, float z); //used to modify x,y,z pull magnitudes
};
