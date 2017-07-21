#include "../andbeyond.h"

void Player::process(State &state){
	// keep the player below the baseline
	bool going_up=yv<0.0f;
	if(y<PLAYER_BASELINE){
		// maybe spawn a new saw
		if(state.saw_list.size()<2&&onein(120)&&state.saw_list.size()<2)
			state.saw_list.push_back(new Saw(state));
		state.height+=(PLAYER_BASELINE-y)*1.1675f;
		y=PLAYER_BASELINE;
	}

	// affected by gravity
	yv+=GRAVITY;

	// find the apex of the jump
	if(going_up&&yv>0.0f) // player has reached the apex of the jump
		apex=y;

	if(yv>TERMINAL_VELOCITY)
		yv=TERMINAL_VELOCITY;
	y+=yv;

	// shadows Base::y while player is alive
	if(!dead)
		alive_y=y;

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
		if(!dead)
			dead=PLAYER_KILLED_BY_FALL;
#endif
	if(dead){
#ifdef INVINCIBLE
		dead=0;
#else
	if(dead){
		// handle the upward pan effect on player death
		if(dead_first){
			dead_first=false;
			if(dead!=PLAYER_KILLED_BY_FALL)
				alive_y=PLAYER_BASELINE-1.3f;
			else
				state.timer_game_over=1;
		}
		else{
			if(dead!=PLAYER_KILLED_BY_FALL){
				alive_y+=/*((PLAYER_BASELINE-alive_y)/40.0f)+0.01f;//*/0.04f;
				if(alive_y>PLAYER_BASELINE-0.1f)
					alive_y=PLAYER_BASELINE-0.1f;
				y+=PLAYER_BASELINE-alive_y;
			}

			// game over menu
			if(!--state.timer_game_over)
				state.show_gameover=true;
		}
	}
#endif
}

	// animation
	if(onein(250))
		timer_frame=15.0f;
	if(timer_frame>10.0f)
		texture=AID_PLAYER_MIDBLINK;
	else if(timer_frame>5.0)
		texture=AID_PLAYER_BLINK;
	else if(timer_frame>0.0f)
		texture=AID_PLAYER_MIDBLINK;
	else
		texture=AID_PLAYER_NORMAL;
	if(timer_frame>0.0f){
		timer_frame-=1.0f;
		if(timer_frame<0.0f)
			timer_frame=0.0f;
	}
}

void Player::render(const Renderer &renderer)const{
	// render player
	renderer.draw(*this,&renderer.atlas,false);
	if(x+PLAYER_WIDTH>renderer.rect.right){
		Base copy={renderer.rect.left-(renderer.rect.right-x),y,PLAYER_WIDTH,PLAYER_HEIGHT,0.0f,texture};
		renderer.draw(copy,&renderer.atlas,false);
	}
	else if(x<renderer.rect.left){
		Base copy={renderer.rect.right+(x-renderer.rect.left),y,PLAYER_WIDTH,PLAYER_HEIGHT,0.0f,texture};
		renderer.draw(copy,&renderer.atlas,false);
	}
}
