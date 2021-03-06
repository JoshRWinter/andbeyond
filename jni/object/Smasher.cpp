#include "andbeyond.h"

Smasher::Smasher(const State &state){
	left.w=SMASHER_WIDTH;
	left.h=SMASHER_HEIGHT;
	left.x=state.renderer.rect.left-SMASHER_RETRACT;
	left.y=state.renderer.rect.top-8.0f;
	left.rot=0.0f;
	if(in_space(state.height-10.0f))
		left.texture=AID_SMASHER_SPACE;
	else
		left.texture=AID_SMASHER_NORMAL;
	retracting=true;
	smashed=false;
	timer=SMASHER_SMASH_TIMER;

	right=left;
	right.x=state.renderer.rect.right-SMASHER_WIDTH+SMASHER_RETRACT;

	bool result;
	do{
		result=left.too_close(state.saw_list,state.electro_list,state.smasher_list);
		if(result){
			left.y-=5.1f;
			right.y=left.y;
		}
	}while(result);
}

void Smasher::process(State &state){
	// maybe make a new smasher
	if(state.player.y<PLAYER_BASELINE&&onein(500)&&state.smasher_list.size()==0)
		state.smasher_list.push_back(new Smasher(state));

	for(std::vector<Smasher*>::iterator iter=state.smasher_list.begin();iter!=state.smasher_list.end();){
		Smasher &smasher=**iter;

		// check for smasher colliding with player
		int left_collide=state.player.correct(smasher.left);
		int right_collide=state.player.correct(smasher.right);
		if(left_collide&&right_collide&&!smasher.smashed&&!smasher.retracting&&!state.player.dead)
			state.player.dead=PLAYER_KILLED_BY_SMASHER;
		else if(left_collide){
			if(left_collide==COLLIDE_BOTTOM)
				state.player.yv=0.0f;
			else if(left_collide==COLLIDE_TOP)
				state.player.yv=-PLAYER_UPWARD_VELOCITY;
			else if(left_collide==COLLIDE_RIGHT||left_collide==COLLIDE_LEFT){
				state.player.xv=0.0f;
				state.player.yv/=1.02f;
			}
		}
		else if(right_collide){
			if(right_collide==COLLIDE_BOTTOM)
				state.player.yv=0.0f;
			else if(right_collide==COLLIDE_TOP)
				state.player.yv=-PLAYER_UPWARD_VELOCITY;
			else if(right_collide==COLLIDE_LEFT||right_collide==COLLIDE_RIGHT){
				state.player.xv=0.0f;
				state.player.yv/=1.02f;
			}
		}

		if(state.player.alive_y<PLAYER_BASELINE){
			smasher.left.y+=PLAYER_BASELINE-state.player.alive_y;
			smasher.right.y+=PLAYER_BASELINE-state.player.alive_y;
		}

		// smash
		if(smasher.retracting){
			smasher.left.x-=SMASHER_RETRACT_VELOCITY;
			if(smasher.left.x<state.renderer.rect.left-SMASHER_RETRACT)
				smasher.left.x=state.renderer.rect.left-SMASHER_RETRACT;
			smasher.right.x+=SMASHER_RETRACT_VELOCITY;
			if(smasher.right.x>state.renderer.rect.right-SMASHER_WIDTH+SMASHER_RETRACT)
				smasher.right.x=state.renderer.rect.right-SMASHER_WIDTH+SMASHER_RETRACT;
			smasher.timer-=1.0f;
			if(smasher.timer<0.0f){
				smasher.timer=SMASHER_RETRACT_TIMER;
				smasher.retracting=false;
			}
		}
		else{
			smasher.left.x+=SMASHER_SMASH_VELOCITY;
			if(smasher.left.x+SMASHER_WIDTH>0.0f){
				smasher.smashed=true;
				smasher.left.x=-SMASHER_WIDTH;
			}
			else
				smasher.smashed=false;
			smasher.right.x-=SMASHER_SMASH_VELOCITY;
			if(smasher.right.x<0.0f)
				smasher.right.x=0.0f;
			smasher.timer-=1.0f;
			if(smasher.timer<0.0f){
				smasher.timer=SMASHER_SMASH_TIMER;
				smasher.retracting=true;
				smasher.smashed=false;
			}
		}

		// if it goes below screen, delete it
		if(smasher.left.y>state.renderer.rect.bottom){
			delete *iter;
			iter=state.smasher_list.erase(iter);
			continue;
		}

		++iter;
	}
}

void Smasher::render(const Renderer &renderer,const std::vector<Smasher*> &smasher_list){
	for(std::vector<Smasher*>::const_iterator iter=smasher_list.begin();iter!=smasher_list.end();++iter){
		renderer.draw((*iter)->left,&renderer.atlas,false);
		renderer.draw((*iter)->right,&renderer.atlas,true);
	}
}

void Smasher::clear_all_ahead(std::vector<Smasher*> &smasher_list,float level){
	for(std::vector<Smasher*>::iterator iter=smasher_list.begin();iter!=smasher_list.end();){
		if((*iter)->left.y+SMASHER_HEIGHT<level){
			delete *iter;
			iter=smasher_list.erase(iter);
			continue;
		}
		++iter;
	}
}
