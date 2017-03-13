#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include "defs.h"

bool state_s::core(){
	timer_game+=1.0f;
	if(timer_game>200.0f)
		timer_game=60.0f;

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

		// proc springs
		if(platform->has_spring){
			platform->spring.x=platform->x+platform->spring.xoffset;
			platform->spring.y=platform->y-SPRING_HEIGHT;

			// check for player colliding with spring
			if(platform->spring.collide(player,0.0f)&&player.apex+PLAYER_HEIGHT<platform->spring.y&&player.yv>0.0f){
				player.y=platform->spring.y-PLAYER_HEIGHT;
				player.yv=-PLAYER_SUPER_UPWARD_VELOCITY;
			}
		}

		// check for platforms colliding with player
		if(player.yv>0.0f&&platform->collide(player,0.0f)&&player.apex+PLAYER_HEIGHT<platform->y+0.15f&&player.y+(PLAYER_HEIGHT/1.5f)<platform->y){
			if(timer_game>60.0f){
				player.y=platform->y-PLAYER_HEIGHT;
				player.yv=-PLAYER_UPWARD_VELOCITY;
			}
			else{
				player.y=platform->y-PLAYER_HEIGHT;
				player.yv=0.1f;
			}
		}

		++iter;
	}

	// proc obstacles
	if(obstacle_list.size()<2&&onein(120)&&(obstacle_list.size()==0||obstacle_list[0]->y-renderer.rect.top>4.0f))
		obstacle_list.push_back(new obstacle_s(*this));
	for(std::vector<obstacle_s*>::iterator iter=obstacle_list.begin();iter!=obstacle_list.end();){
		obstacle_s *obstacle=*iter;

		if(player.y<PLAYER_BASELINE){
			obstacle->y+=PLAYER_BASELINE-player.y;
			obstacle->rail.y+=PLAYER_BASELINE-player.y;
		}

		// check for obstacles colliding with player
		if(obstacle->collide(player,0.25f)){
			player.dead=true;
		}

		// slide the saw down the rail
		obstacle->x+=obstacle->xv;
		if(obstacle->x+OBSTACLE_SIZE>renderer.rect.right){
			obstacle->x=renderer.rect.right-OBSTACLE_SIZE;
			obstacle->xv=-obstacle->xv;
		}
		else if(obstacle->x<renderer.rect.left){
			obstacle->x=renderer.rect.left;
			obstacle->xv=-obstacle->xv;
		}

		// rotate the obstacle
		if(obstacle->xv>0.0f)
			obstacle->rot+=OBSTACLE_SPIN_SPEED;
		else
			obstacle->rot-=OBSTACLE_SPIN_SPEED;

		// generate a particle
		particle_list.push_back(new particle_s(*this,obstacle->x+(OBSTACLE_SIZE/2.0f),obstacle->y+(OBSTACLE_SIZE/2.0f),obstacle->xv>0.0f));

		// delete if obstacle goes below the screen
		if(obstacle->y>renderer.rect.bottom){
			delete obstacle;
			iter=obstacle_list.erase(iter);
			continue;
		}

		++iter;
	}

	// proc particles
	for(std::vector<particle_s*>::iterator iter=particle_list.begin();iter!=particle_list.end();){
		particle_s *particle=*iter;

		particle->ttl-=1.0f;
		if(particle->ttl<0.0f){
			delete particle;
			iter=particle_list.erase(iter);
			continue;
		}

		if(player.y<PLAYER_BASELINE)
			particle->y+=PLAYER_BASELINE-player.y;

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
		for(std::vector<platform_s*>::const_iterator iter=platform_list.begin();iter!=platform_list.end();++iter){
			int side;
			if((side=particle->correct(**iter))){
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
		if(particle->x+PARTICLE_HEIGHT>renderer.rect.right){
			particle->x=renderer.rect.right-PARTICLE_HEIGHT;
			particle->xv=-particle->xv;
		}
		else if(particle->x<renderer.rect.left){
			particle->x=renderer.rect.left;
			particle->xv=-particle->xv;
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
		if(lower_backdrop.y<renderer.rect.bottom)
			lower_backdrop.y+=(PLAYER_BASELINE-player.y)/50.0f;
	}

	// proc player
	{
		// keep the player below the baseline
		bool going_up=player.yv<0.0f;
		if(player.y<PLAYER_BASELINE){
			height+=PLAYER_BASELINE-player.y;
			player.y=PLAYER_BASELINE;
		}
		player.yv+=GRAVITY;
		if(going_up&&player.yv>0.0f) // player has reached the apex of the jump
			player.apex=player.y;

		if(player.yv>TERMINAL_VELOCITY)
			player.yv=TERMINAL_VELOCITY;
		player.y+=player.yv;

		if(timer_game>60.0f){
			targetf(&tilt,0.7f,accel.x);
			player.x-=tilt/TILT_DIVISOR;
		}
		// wrap the screen edges
		if(player.x+(PLAYER_WIDTH/2.0f)>renderer.rect.right)
			player.x=renderer.rect.left-(PLAYER_WIDTH/2.0f);
		else if(player.x<renderer.rect.left-(PLAYER_WIDTH/2.0f))
			player.x=renderer.rect.right-(PLAYER_WIDTH/2.0f);
		if(player.y>renderer.rect.bottom)
			player.dead=true;
		if(player.dead)
			reset();
	}

	return true;
}

void state_s::render(){
	// draw the sky
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw the lower backdrop
	if(lower_backdrop.y<renderer.rect.bottom){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_LOWERBACKDROP].object);
		renderer.draw(lower_backdrop,false);
	}

	// draw the backdrops (if any)
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_1.tid].object);
	renderer.draw(backdrop_1,false);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[backdrop_2.tid].object);
	renderer.draw(backdrop_2,false);

	// render particles
	if(particle_list.size()!=0){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PARTICLE].object);
		for(std::vector<particle_s*>::const_iterator iter=particle_list.begin();iter!=particle_list.end();++iter)
			renderer.draw(**iter,false);
	}

	// render obstacles
	if(obstacle_list.size()!=0){
		// render rails
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_OBSTACLERAIL].object);
		for(std::vector<obstacle_s*>::const_iterator iter=obstacle_list.begin();iter!=obstacle_list.end();++iter)
			renderer.draw((*iter)->rail,false);
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_OBSTACLE].object);
		for(std::vector<obstacle_s*>::const_iterator iter=obstacle_list.begin();iter!=obstacle_list.end();++iter)
			renderer.draw(**iter,(*iter)->xv>0.0f?true:false);
	}

	// render platforms
	if(platform_list.size()!=0){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLATFORM].object);
		for(std::vector<platform_s*>::const_iterator iter=platform_list.begin();iter!=platform_list.end();++iter)
			renderer.draw(**iter,(*iter)->xflip);
		// render springs
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_SPRING].object);
		for(std::vector<platform_s*>::const_iterator iter=platform_list.begin();iter!=platform_list.end();++iter){
			if((*iter)->has_spring)
				renderer.draw((*iter)->spring,false);
		}
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
	renderer.rect.right=4.5f;
	renderer.rect.left=-4.5f;
	renderer.rect.bottom=8.0f;
	renderer.rect.top=-8.0f;

	lower_backdrop.x=renderer.rect.left;
	lower_backdrop.w=renderer.rect.right*2.0f;
	lower_backdrop.h=(renderer.rect.bottom*2.0f)+2.0f;
	lower_backdrop.rot=0.0f;
	lower_backdrop.count=1.0f;
	lower_backdrop.frame=0.0f;

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
	// clear obstacles
	for(std::vector<obstacle_s*>::iterator iter=obstacle_list.begin();iter!=obstacle_list.end();++iter)
		delete *iter;
	obstacle_list.clear();
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
	lower_backdrop.y=renderer.rect.top-2.0f;

	tilt=0.0f;
	timer_game=0.0f;
	height=0.0f;
}
