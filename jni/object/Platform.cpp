#include "../andbeyond.h"

Platform::Platform(const State &state,float highest,int platform_type){
	// choose spacing
	int closest;
	int farthest;
	int stage;
	if(state.height<60){
		closest=100;
		farthest=170;
		stage=1;
	}
	else if(state.height<120){
		closest=150;
		farthest=235;
		stage=2;
	}
	else if(state.height<190){
		closest=215;
		farthest=240;
		stage=3;
	}
	else if(state.height<250){
		closest=270;
		farthest=320;
		stage=4;
	}
	else{
		closest=340;
		farthest=370;
		stage=5;
	}

	// first platform of the level
	bool first;
	if(highest==state.renderer.rect.bottom)
		first=true;
	else
		first=false;

	// the last platform spawned was a PLATFORM_DISAPPEARING
	bool last_disappearing=false;
	if(!first&&state.platform_list[state.platform_list.size()-1]->type==PLATFORM_DISAPPEARING)
		last_disappearing=true;

	// set platform type
	if(platform_type==PLATFORM_DONTCARE){
		// choose PLATFORM_DISAPPEARING
		if(((last_disappearing&&!onein(4))||onein(25))&&!first)
			type=PLATFORM_DISAPPEARING;
		// most likely choose PLATFORM_NORMAL or PLATFORM_SLIDING
		else{
			int sliding_probability;
			if(stage==3)
				sliding_probability=5;
			else if(stage>3)
				sliding_probability=2;
			else
				sliding_probability=7;
			if(onein(sliding_probability)&&!first)
				type=PLATFORM_SLIDING;
			else
				type=PLATFORM_NORMAL;
		}
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
	texture=AID_PLATFORM_NORMAL+type;
	xflip=randomint(0,1)==0;
	yv=0.0f;
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

	// handle space platform textures
	if(in_space(state.height-10))
		texture+=3;

	// move platform to the side if it is near an electro
	if(type!=PLATFORM_SLIDING){
		float Y_TOLERANCE=4.5f;
		for(const Electro *e:state.electro_list){
			const Electro &electro=*e;

			if(y+PLATFORM_HEIGHT+Y_TOLERANCE>electro.y&&y<electro.y+ELECTRO_HEIGHT+Y_TOLERANCE){
				if(x+PLATFORM_WIDTH>electro.x&&x<electro.x+ELECTRO_WIDTH){
					// try to move left first
					x=electro.x-PLATFORM_WIDTH;
					if(x<state.renderer.rect.left){
						// that didn't work, move it to the right
						x=electro.x+ELECTRO_WIDTH;
					}
				}
			}
		}

		// move platform towards the center if near a smasher
		for(const Smasher *s:state.smasher_list){
			const Smasher &smasher=*s;
			if(y>smasher.left.y&&y<smasher.left.y+Y_TOLERANCE){
				// bring it in towards the center
				const float bound=5.0f;
				x=randomint((state.renderer.rect.left+bound)*10.0f,(state.renderer.rect.right-bound-PLATFORM_WIDTH)*10.0f)/10.0f;
			}
		}
	}
}

Spring::Spring(){
	w=SPRING_WIDTH;
	h=SPRING_HEIGHT;
	rot=0.0f;
	texture=AID_SPRING;
	xoffset=randomint(1,((PLATFORM_WIDTH-SPRING_WIDTH-0.1f))*10.0f)/10.0f;
}

void Platform::process(State &state){
	// spawn new platforms
	float highest_y;
	do{
		if(state.platform_list.size()==0)
			highest_y=state.renderer.rect.bottom;
		else
			highest_y=state.platform_list[state.platform_list.size()-1]->y;
		if(highest_y>state.renderer.rect.top)
			state.platform_list.push_back(new Platform(state,highest_y,PLATFORM_DONTCARE));
	}while(highest_y>state.renderer.rect.top);

	for(std::vector<Platform*>::iterator iter=state.platform_list.begin();iter!=state.platform_list.end();){
		Platform *platform=*iter;

		// move the platforms down if player is above the baseline
		if(state.player.alive_y<PLAYER_BASELINE)
			platform->y+=PLAYER_BASELINE-state.player.alive_y;

		// decide whether to delete a platform
		if(platform->y>state.renderer.rect.bottom){
			delete platform;
			iter=state.platform_list.erase(iter);
			continue;
		}

		// some platforms slide across the screen
		platform->x+=platform->xv;
		if(platform->x<state.renderer.rect.left){
			platform->x=state.renderer.rect.left;
			platform->xv=-platform->xv;
		}
		else if(platform->x+PLATFORM_WIDTH>state.renderer.rect.right){
			platform->x=state.renderer.rect.right-PLATFORM_WIDTH;
			platform->xv=-platform->xv;
		}
		if(platform->yv>0.0f){
			platform->yv+=GRAVITY;
			platform->y+=platform->yv;
		}

		// proc springs
		if(platform->has_spring){
			platform->spring.x=platform->x+platform->spring.xoffset;
			platform->spring.y=platform->y-SPRING_HEIGHT;

			// check for player colliding with spring
			if(platform->spring.collide(state.player,0.0f)&&state.player.apex+PLAYER_HEIGHT<platform->spring.y&&state.player.yv>0.0f){
				// delete any obstacles that have spawned but are still offscreen
				Saw::clear_all_ahead(state.saw_list,state.renderer.rect.top);
				Electro::clear_all_ahead(state.electro_list,state.renderer.rect.top);
				Smasher::clear_all_ahead(state.smasher_list,state.renderer.rect.top);

				state.player.y=platform->spring.y-PLAYER_HEIGHT;
				state.player.yv=-PLAYER_SUPER_UPWARD_VELOCITY;
			}
		}

		// check for platforms colliding with player
		if(state.player.yv>0.0f&&platform->collide(state.player,0.0f)&&state.player.apex+PLAYER_HEIGHT<platform->y+0.15f&&state.player.y+(PLAYER_HEIGHT/1.5f)<platform->y&&!state.player.dead){
			if(state.timer_game>60.0f){
				state.player.y=platform->y-PLAYER_HEIGHT;
				state.player.yv=-PLAYER_UPWARD_VELOCITY;
			}
			else{
				state.player.y=platform->y-PLAYER_HEIGHT;
				state.player.yv=0.1f;
			}

			// PLATFORM_DISAPPEARING platforms disappear after the player jumps on them
			if(platform->type==PLATFORM_DISAPPEARING)
				platform->yv+=GRAVITY;
		}

		// PLATFORM_DISAPPEARING platforms will delete all obstacles above it
		if(platform->type==PLATFORM_DISAPPEARING&&platform->yv==0.0f){
			Saw::clear_all_ahead(state.saw_list,platform->y);
			Electro::clear_all_ahead(state.electro_list,platform->y);
			Smasher::clear_all_ahead(state.smasher_list,platform->y);
		}

		++iter;
	}
}

void Platform::render(const Renderer &renderer,const std::vector<Platform*> &platform_list){
	for(const Platform *platform:platform_list)
		renderer.draw(*platform,&renderer.atlas,platform->xflip);
	// render springs
	for(const Platform *platform:platform_list){
		if(platform->has_spring)
			renderer.draw(platform->spring,&renderer.atlas,false);
	}
}
