#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "../defs.h"

void player_s::process(state_s &state){
	// keep the player below the baseline
	bool going_up=yv<0.0f;
	if(y<PLAYER_BASELINE){
		// maybe spawn a new saw
		if(state.saw_list.size()<2&&onein(120)&&(state.saw_list.size()==0||state.saw_list[0]->y-state.renderer.rect.top>4.0f))
			state.saw_list.push_back(new saw_s(state));
		state.height+=PLAYER_BASELINE-y;
		y=PLAYER_BASELINE;
	}
	yv+=GRAVITY;
	if(going_up&&yv>0.0f) // player has reached the apex of the jump
		apex=y;

	if(yv>TERMINAL_VELOCITY)
		yv=TERMINAL_VELOCITY;
	y+=yv;

	if(state.timer_game>60.0f){
		targetf(&state.tilt,0.7f,state.accel.x);
		x-=state.tilt/TILT_DIVISOR;
	}
	// wrap the screen edges
	if(x+(PLAYER_WIDTH/2.0f)>state.renderer.rect.right)
		x=state.renderer.rect.left-(PLAYER_WIDTH/2.0f);
	else if(x<state.renderer.rect.left-(PLAYER_WIDTH/2.0f))
		x=state.renderer.rect.right-(PLAYER_WIDTH/2.0f);
	if(y>state.renderer.rect.bottom)
#ifdef INVINCIBLE
		yv=-PLAYER_SUPER_UPWARD_VELOCITY*1.5f;
#else
		dead=true;
#endif
	if(dead)
#ifdef INVINCIBLE
		dead=false;
#else
		state.reset();
#endif

	// animation
	if(onein(250))
		timer_frame=15.0f;
	if(timer_frame>10.0f)
		frame=1.0f;
	else if(timer_frame>5.0)
		frame=2.0f;
	else if(timer_frame>0.0f)
		frame=1.0f;
	else
		frame=0.0f;
	if(timer_frame>0.0f){
		timer_frame-=1.0f;
		if(timer_frame<0.0f)
			timer_frame=0.0f;
	}
}

void player_s::render(const renderer_s &renderer){
	// render player
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLAYER].object);
	renderer.draw(*this,false);
	if(x+PLAYER_WIDTH>renderer.rect.right){
		base_s copy={renderer.rect.left-(renderer.rect.right-x),y,PLAYER_WIDTH,PLAYER_HEIGHT,0.0f,3.0f,frame};
		renderer.draw(copy,false);
	}
	else if(x<renderer.rect.left){
		base_s copy={renderer.rect.right+(x-renderer.rect.left),y,PLAYER_WIDTH,PLAYER_HEIGHT,0.0f,3.0f,frame};
		renderer.draw(copy,false);
	}
}
