#include "andbeyond.h"

bool State::core(){
	if(show_menu){
		if(!menu.main.exec(*this))
			return false;
		show_menu=false;
	}

	if(show_gameover){
		show_gameover=false;
		if(!menu.gameover.exec(*this))
			return false;
	}

	timer_game+=1.0f;
	if(timer_game>200.0f)
		timer_game=60.0f;

	// proc smashers
	// must be first
	Smasher::process(*this);

	// proc platforms
	Platform::process(*this);

	// proc particles
	Particle::process(*this);

	// proc saws
	Saw::process(*this);

	// proc electros
	Electro::process(*this);

	// proc scenery
	Scenery::process(*this);

	// proc backdrops
	if(player.alive_y<PLAYER_BASELINE){
		backdrop_1.y+=PLAYER_BASELINE-player.alive_y;
		backdrop_2.y+=PLAYER_BASELINE-player.alive_y;
		if(backdrop_1.y>renderer.rect.bottom){
			if(backdrop_2.tid!=TID_BACKDROP_TRANSITION&&backdrop_1.tid!=TID_BACKDROP_TRANSITION)
				backdrop_1.y=backdrop_2.y-backdrop_1.h+0.05f;
			if(in_space(height))
				backdrop_1.tid=TID_BACKDROP_TRANSITION;
			else
				backdrop_1.tid=randomint(TID_BACKDROP_FIRST,TID_BACKDROP_LAST);
		}
		else if(backdrop_2.y>renderer.rect.bottom){
			if(backdrop_2.tid!=TID_BACKDROP_TRANSITION&&backdrop_1.tid!=TID_BACKDROP_TRANSITION)
				backdrop_2.y=backdrop_1.y-backdrop_2.h+0.05f;
			if(in_space(height))
				backdrop_2.tid=TID_BACKDROP_TRANSITION;
			else
				backdrop_2.tid=randomint(TID_BACKDROP_FIRST,TID_BACKDROP_LAST);
		}
		if(lower_backdrop.y<renderer.rect.bottom)
			lower_backdrop.y+=(PLAYER_BASELINE-player.alive_y)/22.5f;
	}

	// needs to be last
	// proc player
	player.process(*this);

	if(scenery_list.size()==0){
		if(around(height,150.0f))
			scenery_list.push_back(new Scenery(*this,SCENERY_BLIMP));
		else if(around(height,500.0f))
			scenery_list.push_back(new Scenery(*this,SCENERY_MOON));
	}

	return true;
}

void State::render()const{
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

	// render scenery
	if(scenery_list.size()!=0){
		Scenery::render(renderer,scenery_list);
	}

	// draw the backdrops
	if(height<TRANSITION_SPACE_HEIGHT+90.0f){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_1.tid].object);
		renderer.draw(backdrop_1,false);
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_2.tid].object);
		renderer.draw(backdrop_2,false);
	}

	// render particles
	if(particle_list.size()!=0){
		Particle::render(renderer,particle_list);
	}

	// render electros
	if(electro_list.size()!=0){
		Electro::render(renderer,electro_list);
	}

	// render saws
	if(saw_list.size()!=0){
		Saw::render(renderer,saw_list);
	}

	// render platforms
	if(platform_list.size()!=0){
		Platform::render(renderer,platform_list);
	}

	// render smashers
	if(smasher_list.size()!=0){
		Smasher::render(renderer,smasher_list);
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

// draw the odds and ends of the game world until the parent function returns and the real renderer can take over
void State::fake_render(float yoffset)const{
	// do the platforms
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLATFORM].object);
	for(const Platform *p:platform_list)
		renderer.draw(*p,yoffset);
	// player
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLAYER].object);
	renderer.draw(player,yoffset);
}

State::State(){
	running=false;
	show_menu=true;
	show_gameover=false;

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

void State::reset(){
	// clear platforms
	for(std::vector<Platform*>::iterator iter=platform_list.begin();iter!=platform_list.end();++iter)
		delete *iter;
	platform_list.clear();
	// clear saws
	for(std::vector<Saw*>::iterator iter=saw_list.begin();iter!=saw_list.end();++iter)
		delete *iter;
	saw_list.clear();
	// clear electros
	for(std::vector<Electro*>::iterator iter=electro_list.begin();iter!=electro_list.end();++iter)
		delete *iter;
	electro_list.clear();
	// clear smashers
	for(std::vector<Smasher*>::iterator iter=smasher_list.begin();iter!=smasher_list.end();++iter)
		delete *iter;
	smasher_list.clear();
	// clear scenery
	for(std::vector<Scenery*>::iterator iter=scenery_list.begin();iter!=scenery_list.end();++iter)
		delete *iter;
	scenery_list.clear();
	// clear particles
	for(std::vector<Particle*>::iterator iter=particle_list.begin();iter!=particle_list.end();++iter)
		delete *iter;
	particle_list.clear();

	// player
	player.x=-PLAYER_WIDTH/2.0f;
	player.y=6.3f;
	player.xv=0.0f;
	player.yv=0.0f;
	player.rot=0.0f;
	player.apex=0.0f;
	player.alive_y=player.y;
	player.dead=false;
	player.dead_first=true;
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
	timer_game_over=TIMER_GAME_OVER;
#ifdef START_HIGH
	height=320.0f;
#else
	height=0.0f;
#endif
}

int State::process(){
	int ident,events;
	android_poll_source *source;
	while((ident=ALooper_pollAll(running?0:-1,NULL,&events,(void**)&source))>=0){
		if(source)
			source->process(app,source);
		if(ident==LOOPER_ID_USER){
			handle_accel(&accel);
		}
		if(app->destroyRequested)
			return false;
	}
	return true;
}