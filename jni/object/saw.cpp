#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include <vector>

#include "../defs.h"

saw_s::saw_s(const state_s &state){
	w=SAW_SIZE;
	h=SAW_SIZE;
	x=state.renderer.rect.left;
	y=state.renderer.rect.top-SAW_SIZE-5.0f;
	xv=SAW_VELOCITY;
	rot=0.0f;
	count=1.0f;
	frame=0.0f;

	// the saw will slide down a rail
	rail.w=state.renderer.rect.right*2.0f;
	rail.h=SAWRAIL_HEIGHT;
	rail.rot=0.0f;
	rail.x=state.renderer.rect.left;
	rail.y=y+(SAW_SIZE/2.0f)-(SAWRAIL_HEIGHT/2.0f);
	rail.count=1.0f;
	rail.frame=0.0f;
}

void saw_s::process(state_s &state){
	for(std::vector<saw_s*>::iterator iter=state.saw_list.begin();iter!=state.saw_list.end();){
		saw_s *saw=*iter;

		if(state.player.y<PLAYER_BASELINE){
			saw->y+=PLAYER_BASELINE-state.player.y;
			saw->rail.y+=PLAYER_BASELINE-state.player.y;
		}

		// check for obstacles colliding with player
		if(saw->collide(state.player,0.25f)){
			state.player.dead=true;
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
			state.particle_list.push_back(new particle_s(state,saw->x+(SAW_SIZE/2.0f),saw->y+(SAW_SIZE/2.0f),saw->xv>0.0f));

		// delete if obstacle goes below the screen
		if(saw->y>state.renderer.rect.bottom){
			delete saw;
			iter=state.saw_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void saw_s::clear_all_ahead(std::vector<saw_s*> &saw_list,float boundary){
	for(std::vector<saw_s*>::iterator iter=saw_list.begin();iter!=saw_list.end();){
		if((*iter)->y+SAW_SIZE<boundary){
			delete *iter;
			iter=saw_list.erase(iter);
			continue;
		}
		++iter;
	}
}

void saw_s::render(const renderer_s &renderer,std::vector<saw_s*> &saw_list){
	// render rails
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_SAWRAIL].object);
	for(std::vector<saw_s*>::const_iterator iter=saw_list.begin();iter!=saw_list.end();++iter)
		renderer.draw((*iter)->rail,false);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_SAW].object);
	for(std::vector<saw_s*>::const_iterator iter=saw_list.begin();iter!=saw_list.end();++iter)
		renderer.draw(**iter,(*iter)->xv>0.0f?true:false);
}
