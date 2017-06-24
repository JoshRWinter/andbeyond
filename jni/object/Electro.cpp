#include "andbeyond.h"

Electro::Electro(const State &state){
	w=ELECTRO_WIDTH;
	h=ELECTRO_HEIGHT;
	x=randomint(state.renderer.rect.left*10.0f,(state.renderer.rect.right-w)*10.0f)/10.0f;
	y=state.renderer.rect.top-8.0f;
	rot=0.0f;
	count=5;
	frame=0;
	timer_frame=0.0f;

	bool result;
	do{
		result=too_close(state.saw_list,state.electro_list,state.smasher_list);
		if(result)
			y-=5.1f;
	}while(result);
}

void Electro::process(State &state){
	// maybe spawn a new electro
	if(onein(300)&&state.electro_list.size()<2)
		state.electro_list.push_back(new Electro(state));

	for(std::vector<Electro*>::iterator iter=state.electro_list.begin();iter!=state.electro_list.end();){
		Electro *electro=*iter;

		if(state.player.alive_y<PLAYER_BASELINE)
			electro->y+=PLAYER_BASELINE-state.player.alive_y;

		// update animation
		electro->timer_frame-=1.0f;
		if(electro->timer_frame<=0.0f){
			electro->timer_frame=2.0f;
			++electro->frame;
			if(electro->frame>4)
				electro->frame=0;
		}

		// check for electro colliding with player
		if(electro->collide(state.player,0.6f)&&!state.player.dead){
			state.player.dead=PLAYER_KILLED_BY_ELECTRO;
		}

		// delete if it goes below screen
		if(electro->y>state.renderer.rect.bottom){
			delete electro;
			iter=state.electro_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void Electro::clear_all_ahead(std::vector<Electro*> &electro_list,float level){
	for(std::vector<Electro*>::iterator iter=electro_list.begin();iter!=electro_list.end();){
		if((*iter)->y+ELECTRO_HEIGHT<level){
			delete *iter;
			iter=electro_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void Electro::render(const Renderer &renderer,const std::vector<Electro*> &electro_list){
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_ELECTRO].object);
	for(const Electro *electro:electro_list)
		renderer.draw(*electro,false);
}
