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
		break;
	case SCENERY_STARBABY:
		w=SCENERY_STARBABY_WIDTH;
		h=SCENERY_STARBABY_HEIGHT;
		tid=TID_SCENERY_STARBABY;
		x=state.renderer.rect.left;
		break;
	}

	if(tid!=TID_SCENERY_STARBABY)
		x=randomint((state.renderer.rect.left+1.0f)*10.0f,(state.renderer.rect.right-w-1.0f)*10.0f)/10.0f;
	y=state.renderer.rect.top-h;
	xflip=(onein(2));
	rot=0.0f;
	texture=-1;
	type=scenery_type;
}

void Scenery::process(State &state){
	for(std::vector<Scenery>::iterator iter=state.scenery_list.begin();iter!=state.scenery_list.end();){
		Scenery &scenery=*iter;

		if(state.player.alive_y<PLAYER_BASELINE)
			scenery.y+=(PLAYER_BASELINE-state.player.alive_y)/SCENERY_PARALLAX;

		if(scenery.y>state.renderer.rect.bottom){
			iter=state.scenery_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void Scenery::render(const Renderer &renderer,const std::vector<Scenery> &scenery_list){
	for(const Scenery &scenery:scenery_list){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[scenery.tid].object);
		renderer.draw(scenery,NULL,scenery.xflip);
	}
}
