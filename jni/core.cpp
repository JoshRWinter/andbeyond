#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

bool state_s::core(){
	timer_game+=1.0f;
	if(timer_game>200.0f)
		timer_game=60.0f;

	// proc platforms
	platform_s::process(*this);

	// proc particles
	particle_s::process(*this);

	// proc saws
	saw_s::process(*this);

	// proc electros
	electro_s::process(*this);

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
		if(lower_backdrop.y<renderer.rect.bottom)
			lower_backdrop.y+=(PLAYER_BASELINE-player.y)/25.0f;
	}

	// needs to be last
	// proc player
	player.process(*this);


	return true;
}

void state_s::render()const{
	// draw the sky
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw the lower backdrop
	if(lower_backdrop.y<renderer.rect.bottom){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_LOWERBACKDROP].object);
		renderer.draw(lower_backdrop,false);
	}
	// draw the upper backdrop
	else{
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_UPPERBACKDROP].object);
		const float alpha=(height-UPPER_BACKDROP_START_TRANSITION)/UPPER_BACKDROP_FULL_TRANSITION;
		glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,alpha>1.0f?1.0f:alpha);
		renderer.draw(upper_backdrop,false);
		glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	}

	// draw the backdrops (if any)
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_1.tid].object);
	renderer.draw(backdrop_1,false);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_2.tid].object);
	renderer.draw(backdrop_2,false);

	// render particles
	if(particle_list.size()!=0){
		particle_s::render(renderer,particle_list);
	}

	// render electros
	if(electro_list.size()!=0){
		electro_s::render(renderer,electro_list);
	}

	// render saws
	if(saw_list.size()!=0){
		saw_s::render(renderer,saw_list);
	}

	// render platforms
	if(platform_list.size()!=0){
		platform_s::render(renderer,platform_list);
	}

	// render player
	player.render(renderer);

	// render hud
	glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
	char height_string[20];
	sprintf(height_string,"%um",(unsigned)height);
	drawtextcentered(renderer.font.main,0.0f,renderer.rect.top+0.1f,height_string);

#ifdef SHOW_FPS
	// fps counter
	{
		glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
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

	// world rectangle
	renderer.rect.right=4.5f;
	renderer.rect.left=-4.5f;
	renderer.rect.bottom=8.0f;
	renderer.rect.top=-8.0f;

	// lower backdrop
	lower_backdrop.x=renderer.rect.left;
	lower_backdrop.w=renderer.rect.right*2.0f;
	lower_backdrop.h=renderer.rect.bottom*2.0f;
	lower_backdrop.rot=0.0f;
	lower_backdrop.count=1;
	lower_backdrop.frame=0;

	// upper backdrop
	upper_backdrop.x=renderer.rect.left;
	upper_backdrop.w=renderer.rect.right*2.0f;
	upper_backdrop.h=renderer.rect.bottom*2.0f;
	upper_backdrop.rot=0.0f;
	upper_backdrop.count=1;
	upper_backdrop.frame=0;

	// player
	player.w=PLAYER_WIDTH;
	player.h=PLAYER_HEIGHT;
	player.count=3;
	player.frame=0;
}

void state_s::reset(){
	// clear platforms
	for(std::vector<platform_s*>::iterator iter=platform_list.begin();iter!=platform_list.end();++iter)
		delete *iter;
	platform_list.clear();
	// clear saws
	for(std::vector<saw_s*>::iterator iter=saw_list.begin();iter!=saw_list.end();++iter)
		delete *iter;
	saw_list.clear();
	// clear electros
	for(std::vector<electro_s*>::iterator iter=electro_list.begin();iter!=electro_list.end();++iter)
		delete *iter;
	electro_list.clear();
	// clear particles
	for(std::vector<particle_s*>::iterator iter=particle_list.begin();iter!=particle_list.end();++iter)
		delete *iter;
	particle_list.clear();

	// player
	player.x=-PLAYER_WIDTH/2.0f;
	player.y=6.3f;
	player.xv=0.0f;
	player.yv=0.0f;
	player.rot=0.0f;
	player.apex=0.0f;
	player.dead=false;
	player.timer_frame=0.0f;

	// backdrops
	backdrop_1.tid=TID_BACKDROPGROUND;
	backdrop_1.x=renderer.rect.left;
	backdrop_1.y=renderer.rect.top;
	backdrop_1.w=renderer.rect.right*2.0f;
	backdrop_1.h=renderer.rect.bottom*2.0f;
	backdrop_1.rot=0.0f;
	backdrop_1.count=1;
	backdrop_1.frame=0;
	backdrop_2=backdrop_1;
	backdrop_2.tid=randomint(TID_BACKDROP_FIRST,TID_BACKDROP_LAST);
	backdrop_2.y=backdrop_1.y-backdrop_2.h;
	lower_backdrop.y=renderer.rect.top;
	upper_backdrop.y=renderer.rect.top;

	tilt=0.0f;
	timer_game=0.0f;
	height=0.0f;
}
