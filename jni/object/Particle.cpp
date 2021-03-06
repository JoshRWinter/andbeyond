#include "../andbeyond.h"

Particle::Particle(const State &state,float x,float y,bool right){
	w=PARTICLE_WIDTH;
	h=PARTICLE_HEIGHT;
	this->x=x-(PARTICLE_WIDTH/2.0f);
	this->y=y-(PARTICLE_HEIGHT/2.0f);
	rot=(right?M_PI+0.6f:(M_PI+2.5f))+(randomint(-20,20)*(M_PI/180.0f));//randomint(1,360)*(M_PI/180.0f);
	xv=-cosf(rot)*PARTICLE_SPEED;
	yv=-sinf(rot)*PARTICLE_SPEED;
	texture=AID_PARTICLE;
	ttl=onein(20)?150.0f:randomint(PARTICLE_TTL);
}

void Particle::process(State &state){
	// proc particles
	for(std::vector<Particle*>::iterator iter=state.particle_list.begin();iter!=state.particle_list.end();){
		Particle *particle=*iter;

		particle->ttl-=1.0f;
		if(particle->ttl<0.0f){
			delete particle;
			iter=state.particle_list.erase(iter);
			continue;
		}

		if(state.player.alive_y<PLAYER_BASELINE)
			particle->y+=PLAYER_BASELINE-state.player.alive_y;

		particle->x+=particle->xv;
		particle->y+=particle->yv;
		// gravity
		particle->yv+=GRAVITY;
		if(particle->yv>PARTICLE_TERMINAL_VELOCITY)
			particle->yv=PARTICLE_TERMINAL_VELOCITY;
		// faster particles are elongated
		const float SPEED=sqrtf(particle->xv*particle->xv+particle->yv*particle->yv);
		particle->w=SPEED;
		particle->rot=atan2f((particle->y+(PARTICLE_HEIGHT/2.0f))-((particle->y+particle->yv)+(PARTICLE_HEIGHT/2.0f)),
			(particle->x+(PARTICLE_WIDTH/2.0f))-((particle->x+particle->xv)+(PARTICLE_WIDTH/2.0f)));
		// air drag reduces x velocity
		zerof(&particle->xv,PARTICLE_DRAG);

		// check for particles colliding with platforms
		for(const Platform *p:state.platform_list){
			const Platform &platform=*p;

			int side;
			if((side=particle->correct(platform))){
				switch(side){
				case COLLIDE_TOP:
					particle->yv=-particle->yv/2.0f;
					if(fabs(particle->yv)<0.02f)
						particle->yv=0.0f;
					zerof(&particle->xv,PARTICLE_DRAG);
					break;
				case COLLIDE_LEFT:
				case COLLIDE_RIGHT:
					particle->xv=-particle->xv;
					break;
				}
			}
		}

		// bounce particles off side walls
		if(particle->x+PARTICLE_HEIGHT>state.renderer.rect.right){
			particle->x=state.renderer.rect.right-PARTICLE_HEIGHT;
			particle->xv=-particle->xv;
		}
		else if(particle->x<state.renderer.rect.left){
			particle->x=state.renderer.rect.left;
			particle->xv=-particle->xv;
		}

		++iter;
	}
}

void Particle::render(const Renderer &renderer,const std::vector<Particle*> &particle_list){
	for(const Particle *particle:particle_list)
		renderer.draw(*particle,&renderer.atlas,false);
}
