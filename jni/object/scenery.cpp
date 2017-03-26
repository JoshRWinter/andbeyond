#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include <vector>

#include "defs.h"

scenery_s::scenery_s(const state_s &state,int scenery_type){
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
	count=1;
	frame=0;
	type=scenery_type;
}

void scenery_s::process(state_s &state){
	for(std::vector<scenery_s*>::iterator iter=state.scenery_list.begin();iter!=state.scenery_list.end();){
		scenery_s &scenery=**iter;

		if(state.player.y<PLAYER_BASELINE)
			scenery.y+=(PLAYER_BASELINE-state.player.y)/SCENERY_PARALLAX;

		if(scenery.y>state.renderer.rect.bottom){
			delete *iter;
			iter=state.scenery_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void scenery_s::render(const renderer_s &renderer,const std::vector<scenery_s*> &scenery_list){
	for(std::vector<scenery_s*>::const_iterator iter=scenery_list.begin();iter!=scenery_list.end();++iter){
		glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[(*iter)->tid].object);
		renderer.draw(**iter,(*iter)->xflip);
	}
}
