#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

bool state_s::core(){
	// spawn new platforms
	float highest_y;
	if(platform_list.size()==0)
		highest_y=renderer.rect.bottom;
	else
		highest_y=platform_list[platform_list.size()-1]->y;
	if(highest_y>renderer.rect.top)
		platform_list.push_back(new platform_s(*this,highest_y,PLATFORM_NORMAL));

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

		// check for platforms colliding with player
		if(player.yv>0.0f&&platform->collide(player,0.0f)&&player.y+(PLAYER_WIDTH/1.5f)<platform->y+platform->h){
			player.y=platform->y-PLAYER_HEIGHT;
			player.yv=-PLAYER_UPWARD_VELOCITY;
		}

		++iter;
	}

	// proc player
	// keep the player below the baseline
	if(player.y<PLAYER_BASELINE)
		player.y=PLAYER_BASELINE;
	player.yv+=GRAVITY;
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

	return true;
}

void state_s::render(){
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_SKY].object);
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	renderer.draw(background,false);

	// render platforms
	if(platform_list.size()!=0){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLATFORM].object);
		for(std::vector<platform_s*>::const_iterator iter=platform_list.begin();iter!=platform_list.end();++iter)
			renderer.draw(**iter,(*iter)->xflip);
	}

	// render player
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLAYER].object);
	renderer.draw(player,false);

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

	player.x=-PLAYER_WIDTH/2.0f;
	player.y=-PLAYER_HEIGHT/2.0f;
	player.xv=0.0f;
	player.yv=0.0f;
	player.rot=0.0f;

	tilt=0.0f;
}
