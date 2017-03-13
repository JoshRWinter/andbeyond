#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <math.h>

#include "defs.h"

platform_s::platform_s(const state_s &state,float highest,int platform_type){
	// choose spacing
	int closest;
	int farthest;
	if(state.height<40){
		closest=100;
		farthest=170;
	}
	else if(state.height<80){
		closest=150;
		farthest=235;
	}
	else if(state.height<150){
		closest=255;
		farthest=330;
	}
	else{
		closest=340;
		farthest=370;
	}

	// first platform of the level
	bool first;
	if(highest==state.renderer.rect.bottom)
		first=true;
	else
		first=false;

	// set platform type
	if(platform_type==PLATFORM_DONTCARE){
		if(onein(7)&&!first)
			type=PLATFORM_SLIDING;
		else
			type=PLATFORM_NORMAL;
	}
	else
		type=platform_type;

	if(first){
		x=-PLATFORM_WIDTH/2.0f;
		y=7.3f;
	}
	else{
		x=randomint(state.renderer.rect.left*10.0f,(state.renderer.rect.right-PLATFORM_WIDTH)*10.0f)/10.0f;
		y=highest-randomint(closest,farthest)/100.0f;
	}
	w=PLATFORM_WIDTH;
	h=PLATFORM_HEIGHT;
	rot=0.0f;
	count=2.0f;
	frame=type;
	xflip=randomint(0,1)==0;
	// sliding platforms move side to side
	if(type==PLATFORM_SLIDING)
		xv=PLATFORM_X_VELOCITY*(onein(2)?1.0f:-1.0f);
	else
		xv=0.0f;

	// potentially give this platform a spring
	if((type==PLATFORM_NORMAL||type==PLATFORM_SLIDING)&&!first){
		has_spring=onein(6)==1;
		spring.x=x+spring.xoffset;
		spring.y=y-SPRING_HEIGHT;
	}
	else
		has_spring=false;
}

spring_s::spring_s(){
	w=SPRING_WIDTH;
	h=SPRING_HEIGHT;
	rot=0.0f;
	frame=0.0f;
	count=1.0f;
	xoffset=randomint(0,(PLATFORM_WIDTH-SPRING_WIDTH)*10.0f)/10.0f;
}

obstacle_s::obstacle_s(const state_s &state){
	w=OBSTACLE_SIZE;
	h=OBSTACLE_SIZE;
	x=state.renderer.rect.left;
	y=state.renderer.rect.top-OBSTACLE_SIZE;
	xv=OBSTACLE_VELOCITY;
	rot=0.0f;
	count=1.0f;
	frame=0.0f;
	
	// the saw will slide down a rail
	rail.w=state.renderer.rect.right*2.0f;
	rail.h=OBSTACLERAIL_HEIGHT;
	rail.rot=0.0f;
	rail.x=state.renderer.rect.left;
	rail.y=y+(OBSTACLE_SIZE/2.0f)-(OBSTACLERAIL_HEIGHT/2.0f);
	rail.count=1.0f;
	rail.frame=0.0f;
}

particle_s::particle_s(const state_s &state,float x,float y,bool right){
	w=PARTICLE_WIDTH;
	h=PARTICLE_HEIGHT;
	this->x=x-(PARTICLE_WIDTH/2.0f);
	this->y=y-(PARTICLE_HEIGHT/2.0f);
	rot=(right?M_PI+0.6f:(M_PI+2.5f))+(randomint(-20,20)*(M_PI/180.0f));//randomint(1,360)*(M_PI/180.0f);
	xv=-cosf(rot)*PARTICLE_SPEED;
	yv=-sinf(rot)*PARTICLE_SPEED;
	count=1.0f;
	frame=0.0f;
	ttl=onein(20)?150.0f:randomint(PARTICLE_TTL);
}
