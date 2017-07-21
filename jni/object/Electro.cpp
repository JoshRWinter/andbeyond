#include "andbeyond.h"

Electro::Electro(const State &state){
	w=ELECTRO_WIDTH;
	h=ELECTRO_HEIGHT;
	x=randomint(state.renderer.rect.left*10.0f,(state.renderer.rect.right-w)*10.0f)/10.0f;
	y=state.renderer.rect.top-8.0f;
	rot=0.0f;
	timer_frame=0.0f;

	if(in_space(state.height-10.0f)){
		space=true;
		texture=AID_ELECTRO_SPACE_1;
	}
	else{
		space=false;
		texture=AID_ELECTRO_1;
	}

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
		const int base_frame=electro->space?AID_ELECTRO_SPACE_1:AID_ELECTRO_1;
		const int top_frame=electro->space?AID_ELECTRO_SPACE_5:AID_ELECTRO_5;
		electro->timer_frame-=1.0f;
		if(electro->timer_frame<=0.0f){
			electro->timer_frame=2.0f;
			++electro->texture;
			if(electro->texture>top_frame)
				electro->texture=base_frame;
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
	for(const Electro *electro:electro_list){
		renderer.draw(*electro,&renderer.atlas,false);
	}
}
