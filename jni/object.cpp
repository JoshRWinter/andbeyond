#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

platform_s::platform_s(const state_s &state,float highest,int type){
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

	bool first;
	if(highest==state.renderer.rect.bottom)
		first=true;
	else
		first=false;
	this->type=type;
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
	count=1.0f;
	frame=type;
	xflip=randomint(0,1)==0;

	// potentially give this platform a spring
	if(type==PLATFORM_NORMAL&&!first){
		has_spring=onein(6)==1;
		spring.x=x+spring.xoffset;
		spring.y=y-SPRING_HEIGHT;
	}
	else
		has_spring=false;

	// possibly make the spring move side to side
	if(onein(7)&&!first)
		xv=PLATFORM_X_VELOCITY*(onein(2)?1.0f:-1.0f);
	else
		xv=0.0f;
}

spring_s::spring_s(){
	w=SPRING_WIDTH;
	h=SPRING_HEIGHT;
	rot=0.0f;
	frame=0.0f;
	count=1.0f;
	xoffset=randomint(0,(PLATFORM_WIDTH-SPRING_WIDTH)*10.0f)/10.0f;
}
