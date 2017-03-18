#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include <vector>

#include "defs.h"

electro_s::electro_s(const state_s &state){
	w=ELECTRO_WIDTH;
	h=ELECTRO_HEIGHT;
	x=randomint(state.renderer.rect.left*10.0f,(state.renderer.rect.right-w)*10.0f)/10.0f;
	y=state.renderer.rect.top-8.0f;
	rot=0.0f;
	count=5.0f;
	frame=0.0f;
	timer_frame=0.0f;

	// if too close to other electro, move it up
	for(std::vector<electro_s*>::const_iterator iter=state.electro_list.begin();iter!=state.electro_list.end();++iter){
		if(collide(**iter,-4.0f))
			y-=8.0f;
	}
}

void electro_s::process(state_s &state){
	// maybe spawn a new electro
	if(onein(300)&&state.electro_list.size()<2)
		state.electro_list.push_back(new electro_s(state));

	for(std::vector<electro_s*>::iterator iter=state.electro_list.begin();iter!=state.electro_list.end();){
		electro_s *electro=*iter;

		if(state.player.y<PLAYER_BASELINE)
			electro->y+=PLAYER_BASELINE-state.player.y;

		// update animation
		electro->timer_frame-=1.0f;
		if(electro->timer_frame<=0.0f){
			electro->timer_frame=2.0f;
			electro->frame+=1.0f;
			if(electro->frame>4.5f)
				electro->frame=0.0f;
		}

		// check for electro colliding with player
		if(electro->collide(state.player,0.6f)){
			state.player.dead=true;
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

void electro_s::clear_all_ahead(std::vector<electro_s*> &electro_list,float level){
	for(std::vector<electro_s*>::iterator iter=electro_list.begin();iter!=electro_list.end();){
		if((*iter)->y+ELECTRO_HEIGHT<level){
			delete *iter;
			iter=electro_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void electro_s::render(const renderer_s &renderer,const std::vector<electro_s*> &electro_list){
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_ELECTRO].object);
	for(std::vector<electro_s*>::const_iterator iter=electro_list.begin();iter!=electro_list.end();++iter)
		renderer.draw(**iter,false);
}
