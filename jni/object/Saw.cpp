#include "../andbeyond.h"

Saw::Saw(const State &state){
	w=SAW_SIZE;
	h=SAW_SIZE;
	x=state.renderer.rect.left;
	y=state.renderer.rect.top-SAW_SIZE-5.0f;
	xv=SAW_VELOCITY;
	rot=0.0f;
	if(in_space(state.height-10.0f))
		texture=AID_OBSTACLE_SPACE;
	else
		texture=AID_OBSTACLE_NORMAL;

	// the saw will slide down a rail
	rail.w=state.renderer.rect.right*2.0f;
	rail.h=SAWRAIL_HEIGHT;
	rail.rot=0.0f;
	rail.x=state.renderer.rect.left;
	rail.y=y+(SAW_SIZE/2.0f)-(SAWRAIL_HEIGHT/2.0f);
	rail.texture=AID_RAIL;

	bool result;
	do{
		result=too_close(state.saw_list,state.electro_list,state.smasher_list);
		if(result)
			y-=5.1f;
	}while(result);
}

void Saw::process(State &state){
	for(std::vector<Saw*>::iterator iter=state.saw_list.begin();iter!=state.saw_list.end();){
		Saw *saw=*iter;

		if(state.player.alive_y<PLAYER_BASELINE)
			saw->y+=PLAYER_BASELINE-state.player.alive_y;

		// the rail follows the saw
		saw->rail.y=saw->y+(SAW_SIZE/2.0f)-(SAWRAIL_HEIGHT/2.0f);

		// check for obstacles colliding with player
		if(saw->collide(state.player,0.25f)&&!state.player.dead){
			state.player.dead=PLAYER_KILLED_BY_SAW;
		}

		// slide the saw down the rail
		saw->x+=saw->xv;
		if(saw->x+SAW_SIZE>state.renderer.rect.right){
			saw->x=state.renderer.rect.right-SAW_SIZE;
			saw->xv=-saw->xv;
		}
		else if(saw->x<state.renderer.rect.left){
			saw->x=state.renderer.rect.left;
			saw->xv=-saw->xv;
		}

		// rotate the obstacle
		if(saw->xv>0.0f)
			saw->rot+=SAW_SPIN_SPEED;
		else
			saw->rot-=SAW_SPIN_SPEED;

		// generate a particle
		if(saw->y+SAW_SIZE+1.5>state.renderer.rect.top)
			state.particle_list.push_back(new Particle(state,saw->x+(SAW_SIZE/2.0f),saw->y+(SAW_SIZE/2.0f),saw->xv>0.0f));

		// delete if obstacle goes below the screen
		if(saw->y>state.renderer.rect.bottom){
			delete saw;
			iter=state.saw_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void Saw::clear_all_ahead(std::vector<Saw*> &saw_list,float boundary){
	for(std::vector<Saw*>::iterator iter=saw_list.begin();iter!=saw_list.end();){
		if((*iter)->y+SAW_SIZE<boundary){
			delete *iter;
			iter=saw_list.erase(iter);
			continue;
		}
		++iter;
	}
}

void Saw::render(const Renderer &renderer,const std::vector<Saw*> &saw_list){
	// render rails
	for(std::vector<Saw*>::const_iterator iter=saw_list.begin();iter!=saw_list.end();++iter)
		renderer.draw((*iter)->rail,&renderer.atlas,false);
	// render saw
	for(std::vector<Saw*>::const_iterator iter=saw_list.begin();iter!=saw_list.end();++iter)
		renderer.draw(**iter,&renderer.atlas,(*iter)->xv>0.0f?true:false);
}
