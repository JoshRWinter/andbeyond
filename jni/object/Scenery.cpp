#include "../andbeyond.h"

Scenery::Scenery(const State &state,int scenery_type){
	switch(scenery_type){
	case SCENERY_BLIMP:
		w=SCENERY_BLIMP_WIDTH;
		h=SCENERY_BLIMP_HEIGHT;
		tid=TID_SCENERY_BLIMP;
		break;
	case SCENERY_MOON:
		w=SCENERY_MOON_SIZE;
		h=SCENERY_MOON_SIZE;
		tid=TID_SCENERY_MOON;
	}

	x=randomint((state.renderer.rect.left+1.0f)*10.0f,(state.renderer.rect.right-w-1.0f)*10.0f)/10.0f;
	y=state.renderer.rect.top-h;
	xflip=(onein(2));
	rot=0.0f;
	texture=-1;
	type=scenery_type;
}

void Scenery::process(State &state){
	for(std::vector<Scenery*>::iterator iter=state.scenery_list.begin();iter!=state.scenery_list.end();){
		Scenery &scenery=**iter;

		if(state.player.alive_y<PLAYER_BASELINE)
			scenery.y+=(PLAYER_BASELINE-state.player.alive_y)/SCENERY_PARALLAX;

		if(scenery.y>state.renderer.rect.bottom){
			delete *iter;
			iter=state.scenery_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void Scenery::render(const Renderer &renderer,const std::vector<Scenery*> &scenery_list){
	for(std::vector<Scenery*>::const_iterator iter=scenery_list.begin();iter!=scenery_list.end();++iter){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[(*iter)->tid].object);
		renderer.draw(**iter,NULL,(*iter)->xflip);
	}
}
