/* to initalize, update and manage and array of particles   */

#include "Particle.h"

Particle::Particle(void)
{
}

Particle::~Particle(void)
{

}

void Particle::turnRed(Particles *p)
{
	p->color[X] = 1.0f;
	p->color[Y] = 0.25f;
	p->color[Z] = 0.0f;
}

void Particle::turnYellow(Particles *p){
	p->color[X] = 1.0f;
	p->color[Y] = 1.0f;
	p->color[Z] = 0.0f;
}

void Particle::turnGold(Particles *p)
{
	p->color[X] = 1.0f;
	p->color[Y] = 0.9f;
	p->color[Z] = 0.0f;
}

void Particle::turnLightYellow(Particles *p)
{
	p->color[X] = 1.0f;
	p->color[Y] = 0.95f;
	p->color[Z] = 0.8f;
}

/*
 * initalizes a single particle according to its type
 */
void Particle::createParticle(Particles *p)
{
	p->lifespan = (((rand()%10+10)))/10.0f;
    p->type = 0;

	p->age = 0.0f;
	p->scale = 0.15f;
	p->direction = 0;
    p->position[X] = 0;
    p->position[Y] = 0;
    p->position[Z] = 0;

    //    p->movement[X] = 0;
    //	p->movement[Y] = 0;
    //	p->movement[Z] = 0;


	p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007);
	p->movement[Y] = ((((((2) * rand()%11)  + 5)) * rand()%11) + 1) * + 0.0005;
	p->movement[Z] = (((((((1) * rand()%11) + 1)) * rand()%11) + 1) * 0.007) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007);

	p->color[X] = 1.0f;
	p->color[Y] = 0.95f;
	p->color[Z] = 0.8f;
	p->pull[X] = 0.0f;
	p->pull[Y] = 0.0f;
	p->pull[Z] = 0.0f;
}

/*
  initializes a particle system according to its type
  calls create particle() to initialize individual particles
 */
void Particle::createParticles(void)
{
	particlePull[Y] = 0.000005;
	particlePull[X] = particlePull[Z] = 0.0f;
	int i = 0;
	for(i; i < MAX_PARTICLES; i++)
	{
	  createParticle(&particles[i]);
	}
}

/*
  updates required particle attributes for all particles in a system

 */
void Particle::updateParticles(void)
{
   int i = 0;
   for(i; i < MAX_PARTICLES; i++)
   {
      particles[i].age = particles[i].age + 0.02;

      if(particles[i].type == 1 || particles[i].type == 2)
         particles[i].scale = particles[i].scale + 0.001; //increasing scale makes textures bigger over lifetime



      particles[i].direction = particles[i].direction + ((((((int)(0.05) * rand()%11))) * rand()%11));

      particles[i].position[X] = particles[i].position[X] + particles[i].movement[X] + particles[i].pull[X];

     // particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] + particles[i].pull[Y];

      particles[i].position[Z] = particles[i].position[Z] + particles[i].movement[Z] + particles[i].pull[Z];

      if(particles[i].type == 2){
                	  particles[i].pull[Y] = particles[i].pull[Y] + particlePull[Y]; //
                	  if(particlePull[Y] < - 0.005 ){
                		particlePull[Y] = 0.0000005;
                        particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] - particles[i].pull[Y];
                	  }else{
                        particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] + (particles[i].pull[Y]);
                	  }
            }
	  else
	  {
            	if(particlePull[Y] > 0.002 )
				{
            		particles[i].pull[Y] = particles[i].pull[Y] - 0.0005;
            		particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] + particles[i].pull[Y];
            	}
				else
				{
            		particles[i].pull[Y] = particles[i].pull[Y] + 0.0005;
            		particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] + particles[i].pull[Y];
            	}

            }



      particles[i].pull[X] = particles[i].pull[X] + 0.0f;
     // particles[i].pull[Y] = particles[i].pull[Y] + 0.0005;
      particles[i].pull[Z] = particles[i].pull[Z] + 0.0f;

      // color changing for fire particles light yellow -> red with age
      if((particles[i].type == 0 || particles[i].type == 2) && rand()%100<10)
      {
//       float temp = particles[i].lifespan/particles[i].age;
    	 float temp = rand()%100;
      
		 if(temp < 75)
         {//red
            turnRed(&particles[i]);
         }
         else if(temp > 75 && temp < 90)
         {//gold
        	turnGold(&particles[i]);
         }
         else if(temp > 90 && temp < 100)
         {//yellow
        	turnYellow(&particles[i]);
         }
//         else
//         {// initial light yellow
//        	turnLightYellow(&particles[i]);
//         }
      }

		if(particles[i].type == 0)
		{
			if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 20 || particles[i].position[Y] < -20 || particles[i].position[X] > 10 || particles[i].position[X] < -10)
			{
			   int temp = rand()%100;
			   
			   if(temp < 20)
				  turnToSmoke(&particles[i]);

			   else if(temp > 20 && temp < 90)
				   flowDown(&particles[i]);
			   else
				  createParticle(&particles[i]);
			}
		}

		else if(particles[i].type == 2)
			{
					if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 10 || particles[i].position[Y] < -15 || particles[i].position[X] > 20 || particles[i].position[X] < -20)
						createParticle(&particles[i]);
			}

		else if(particles[i].type == 1)
		{
			if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 45 || particles[i].position[Y] < -35 || particles[i].position[X] > 10 || particles[i].position[X] < -10)
			   createParticle(&particles[i]);
		}
		
   	}
}//

/*
 used to turn selected fire particles into smoke
 */

void Particle::turnToSmoke(Particles *p)
{
   p->lifespan = (((rand()%125+1)/10.0f)+80);
   p->age = 0.0f;
   p->scale = 0.35f;
   p->direction = 0;
   p->type = 1;

   p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0035) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0005);
   p->movement[Y] = ((((((4) * rand()%11) + 3)) * rand()%11) + 7) * 0.0002;
   p->movement[Z] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0015) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0005);

   p->color[X] = 0.7f;
   p->color[Y] = 0.7f;
   p->color[Z] = 0.7f;
}

/*
 * used to create fire particles and flow towards down side : Useful
 */

void Particle::flowDown(Particles *p)
{
   p->lifespan = (((rand()%125+1)/2.0f)+30);
   p->age = 0.0f;
   p->scale = 0.35f;
   p->direction = 0;
   p->type = 2;

   p->movement[X] = (((((((9) * rand()%11) + 1)) * rand()%11) + 1) * 0.0000003) - (((((((1) * rand()%11) + 1)) * rand()%50) + 1) * 0.00000005);
   p->movement[Y] = ((((((2) * rand()%11) + 4)) * rand()%11) + 5) * 0.000001;
   p->movement[Z] = (((((((9) * rand()%11) + 1)) * rand()%11) + 1) * -0.0000001) - (((((((1) * rand()%11) + 1)) * rand()%50) + 1) * 0.00000005);

   int temp = rand()%100;
   if(temp < 30)
   {
	   turnGold(p);
   }
   else
   {
	   turnRed(p);
   }

   modifyParticlePull(0.0f, -0.0009f, 0.0f);

}


int Particle::getNumOfParticles(void)
{
   return MAX_PARTICLES;
}

float Particle::getXPos(int i)
{
   return particles[i].position[X];
}

float Particle::getYPos(int i)
{
   return particles[i].position[Y];
}
float Particle::getZPos(int i)
{
   return particles[i].position[Z];
}

//colors
float Particle::getR(int i)
{
   return particles[i].color[X];
}

float Particle::getG(int i)
{
   return particles[i].color[Y];
}
float Particle::getB(int i)
{
   return particles[i].color[Z];
}

float Particle::getScale(int i)
{
   return particles[i].scale;
}

float Particle::getDirection(int i)
{
   return particles[i].direction;
}

float Particle::getAlpha(int i)
{
   return (1 - particles[i].age/particles[i].lifespan);
}

void Particle::modifyParticlePull(float x, float y, float z)
{
	particlePull[X] += x;
	particlePull[Y] += y;
	particlePull[Z] += z;
}

