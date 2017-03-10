#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

bool state_s::core(){
	// spawn new platforms
	float highest_y;
	do{
		if(platform_list.size()==0)
			highest_y=renderer.rect.bottom;
		else
			highest_y=platform_list[platform_list.size()-1]->y;
		if(highest_y>renderer.rect.top)
			platform_list.push_back(new platform_s(*this,highest_y,PLATFORM_NORMAL));
	}while(highest_y>renderer.rect.top);

	// proc platforms
	for(std::vector<platform_s*>::iterator iter=platform_list.begin();iter!=platform_list.end();){
		platform_s *platform=*iter;

		// move the platforms down if player is above the baseline
		if(player.y<PLAYER_BASELINE)
			platform->y+=PLAYER_BASELINE-player.y;

		// decide whether to delete a platform
		if(platform->y>renderer.rect.bottom){
			delete platform;
			iter=platform_list.erase(iter);
			continue;
		}

		// some platforms slide across the screen
		platform->x+=platform->xv;
		if(platform->x<renderer.rect.left){
			platform->x=renderer.rect.left;
			platform->xv=-platform->xv;
		}
		else if(platform->x+PLATFORM_WIDTH>renderer.rect.right){
			platform->x=renderer.rect.right-PLATFORM_WIDTH;
			platform->xv=-platform->xv;
		}

		// check for platforms colliding with player
		if(player.yv>0.0f&&platform->collide(player,0.0f)&&player.apex+PLAYER_HEIGHT<platform->y+0.15f){
			player.y=platform->y-PLAYER_HEIGHT;
			player.yv=-PLAYER_UPWARD_VELOCITY;
		}

		++iter;
	}

	// proc backdrops
	if(player.y<PLAYER_BASELINE){
		backdrop_1.y+=PLAYER_BASELINE-player.y;
		backdrop_2.y+=PLAYER_BASELINE-player.y;
		if(backdrop_1.y>renderer.rect.bottom){
			backdrop_1.y=backdrop_2.y-backdrop_1.h+0.05f;
			backdrop_1.tid=randomint(TID_BACKDROP_FIRST,TID_BACKDROP_LAST);
		}
		else if(backdrop_2.y>renderer.rect.bottom){
			backdrop_2.y=backdrop_1.y-backdrop_2.h+0.05f;
			backdrop_2.tid=randomint(TID_BACKDROP_FIRST,TID_BACKDROP_LAST);
		}
	}

	// proc player
	{
		// keep the player below the baseline
		bool going_up=player.yv<0.0f;
		if(player.y<PLAYER_BASELINE)
			player.y=PLAYER_BASELINE;
		player.yv+=GRAVITY;
		if(going_up&&player.yv>0.0f) // player has reached the apex of the jump
			player.apex=player.y;

		if(player.yv>TERMINAL_VELOCITY)
			player.yv=TERMINAL_VELOCITY;
		player.y+=player.yv;

		targetf(&tilt,0.7f,accel.x);
		player.x-=tilt/TILT_DIVISOR;

		// wrap the screen edges
		if(player.x+(PLAYER_WIDTH/2.0f)>renderer.rect.right)
			player.x=renderer.rect.left-(PLAYER_WIDTH/2.0f);
		else if(player.x<renderer.rect.left-(PLAYER_WIDTH/2.0f))
			player.x=renderer.rect.right-(PLAYER_WIDTH/2.0f);
		if(player.y>renderer.rect.bottom)
			reset();
	}

	return true;
}

void state_s::render(){
	// draw the sky
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_SKY].object);
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	renderer.draw(background,false);

	// draw the backdrops (if any)
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_1.tid].object);
	renderer.draw(backdrop_1,false);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_2.tid].object);
	renderer.draw(backdrop_2,false);

	// render platforms
	if(platform_list.size()!=0){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLATFORM].object);
		for(std::vector<platform_s*>::const_iterator iter=platform_list.begin();iter!=platform_list.end();++iter)
			renderer.draw(**iter,(*iter)->xflip);
	}

	// render player
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLAYER].object);
	renderer.draw(player,false);
	if(player.x+PLAYER_WIDTH>renderer.rect.right){
		base_s copy={renderer.rect.left-(renderer.rect.right-player.x),player.y,PLAYER_WIDTH,PLAYER_HEIGHT,0.0f,1.0f,0.0f};
		renderer.draw(copy,false);
	}
	else if(player.x<renderer.rect.left){
		base_s copy={renderer.rect.right+(player.x-renderer.rect.left),player.y,PLAYER_WIDTH,PLAYER_HEIGHT,0.0f,1.0f,0.0f};
		renderer.draw(copy,false);
	}

#ifdef SHOW_FPS
	// fps counter
	{
		glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
		glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,1.0f);
		static char msg[30];
		static int last_time=0;
		static int fps=0;
		int current_time=time(NULL);
		if(current_time!=last_time){
			last_time=current_time;
			sprintf(msg,"[and beyond] fps: %d",fps);
			fps=0;
		}
		else
			++fps;

		drawtext(renderer.font.main,renderer.rect.left+0.1f,renderer.rect.top+0.1f,msg);
	}
#endif // SHOW_FPS
}

state_s::state_s(){
	running=false;
	renderer.rect.right=4.5f;
	renderer.rect.left=-4.5f;
	renderer.rect.bottom=8.0f;
	renderer.rect.top=-8.0f;

	background.x=renderer.rect.left;
	background.y=renderer.rect.top;
	background.w=renderer.rect.right*2.0f;
	background.h=renderer.rect.bottom*2.0f;
	background.rot=0.0f;
	background.count=1.0f;
	background.frame=0.0f;

	player.w=PLAYER_WIDTH;
	player.h=PLAYER_HEIGHT;
	player.count=1.0f;
	player.frame=0.0f;
}

void state_s::reset(){
	// clear platforms
	for(std::vector<platform_s*>::iterator iter=platform_list.begin();iter!=platform_list.end();++iter)
		delete *iter;
	platform_list.clear();

	// player
	player.x=-PLAYER_WIDTH/2.0f;
	player.y=-PLAYER_HEIGHT/2.0f;
	player.xv=0.0f;
	player.yv=0.0f;
	player.rot=0.0f;
	player.apex=0.0f;

	// backdrops
	backdrop_1.tid=TID_BACKDROPGROUND;
	backdrop_1.x=renderer.rect.left;
	backdrop_1.y=renderer.rect.top;
	backdrop_1.w=renderer.rect.right*2.0f;
	backdrop_1.h=renderer.rect.bottom*2.0f;
	backdrop_1.rot=0.0f;
	backdrop_1.count=1.0f;
	backdrop_1.frame=1.0f;
	backdrop_2=backdrop_1;
	backdrop_2.tid=randomint(TID_BACKDROP_FIRST,TID_BACKDROP_LAST);
	backdrop_2.y=backdrop_1.y-backdrop_2.h;

	tilt=0.0f;
}
