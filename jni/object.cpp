#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

platform_s::platform_s(const state_s &state,float highest,int type){
	bool first;
	if(highest==state.renderer.rect.bottom)
		first=true;
	else
		first=false;
	this->type=type;
	if(first)
		x=-PLATFORM_WIDTH/2.0f;
	else
		x=randomint(state.renderer.rect.left*10.0f,(state.renderer.rect.right-PLATFORM_WIDTH)*10.0f)/10.0f;
	y=highest-randomint(90,150)/100.0f;
	w=PLATFORM_WIDTH;
	h=PLATFORM_HEIGHT;
	rot=0.0f;
	count=1.0f;
	frame=type;
	xflip=randomint(0,1)==0;
	if(onein(7)&&!first)
		xv=PLATFORM_X_VELOCITY*(onein(2)?1.0f:-1.0f);
	else
		xv=0.0f;
}
