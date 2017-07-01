#include <stdio.h>
#include "../andbeyond.h"

void selection(int*);

void MenuGameover::exec(State &state){
	full_white.background(state.renderer);
	const float x_inward=0.75f,y_inward=1.5f;
	background.x=MENU_CARD_X;
	background.y=MENU_CARD_Y;
	background.w=MENU_CARD_W;
	background.h=MENU_CARD_H;
	background.rot=0.0f;
	background.count=1;
	background.frame=0;

	// attachment
	attachment.x=state.renderer.rect.left;
	attachment.y=state.renderer.rect.bottom+0.5f;
	attachment.w=state.renderer.rect.right*2.0f;
	attachment.h=8.225f;
	attachment.rot=0.0f;
	attachment.count=1;
	attachment.frame=0;

	entry.background(state.renderer);
	entry.y=ATTACHMENT_Y_TARGET+attachment.h;

	// buttons
	again.init(-BUTTON_WIDTH/2.0f,2.0f,"Play again");
	menu.init(-BUTTON_WIDTH/2.0f,again.y+BUTTON_HEIGHT+0.2f,"Menu");

	// header text
	switch(state.player.dead){
	case PLAYER_KILLED_BY_FALL:
		header="SPLATTERED";
		break;
	case PLAYER_KILLED_BY_SAW:
		header="SLICED";
		break;
	case PLAYER_KILLED_BY_ELECTRO:
		header="ELECTROCUTED";
		break;
	case PLAYER_KILLED_BY_SMASHER:
		header="SMASHED";
		break;
	}

	yoffset_target=0.0f;
	yoffset=state.renderer.rect.top*2.0f;
	full_white_alpha=0.0f;
	local=&state;
	play=false;
	score=state.height;

	// highscore processing
	scoreboard=state.scoreboard;
	scoreboard_index=-1;
	if(score>state.scoreboard[0]){
		state.scoreboard[0]=score;
		selection(state.scoreboard);
		// find the index
		for(int i=0;i<SCOREBOARD_COUNT;++i){
			if(state.scoreboard[i]==score){
				scoreboard_index=i;
				break;
			}
		}

		state.write_score();
	}

	// find longest scoreboard line
	float len=0.0f;
	for(int i=0;i<SCOREBOARD_COUNT;++i){
		char entry[20];
		if(scoreboard[i]==0)
			sprintf(entry,"%d: -",SCOREBOARD_COUNT-i);
		else
			sprintf(entry,"%d: %dm",SCOREBOARD_COUNT-i,scoreboard[i]);
		const float this_entry=textlen(state.renderer.font.main,entry);
		if(this_entry>len)
			len=this_entry;
	}
	scoreboard_x=-len/2.0f;

	while(state.process()){
		// handle transition
		if(targetf(&yoffset,(fabsf(yoffset-yoffset_target)/20.0f)+0.1f,yoffset_target)==TRANSITION_OUT_TARGET)
			return;

		// handle attachment animation
		targetf(&attachment.y,ATTACHMENT_Y_SPEED,ATTACHMENT_Y_TARGET);

		// make the background more opaque
		targetf(&full_white_alpha,FULL_WHITE_TRANSITION_SPEED,yoffset_target==TRANSITION_OUT_TARGET?1.0f:FULL_WHITE_TARGET_ALPHA);

		if(play&&attachment.y==ATTACHMENT_Y_TARGET&&yoffset_target!=TRANSITION_OUT_TARGET){
			state.reset();
			Platform::process(state);
			yoffset_target=TRANSITION_OUT_TARGET;
		}

		// process buttons
		if(again.process(state)||state.back){
			state.back=false;
			play=true;
		}
		if(menu.process(state)){
			state.show_menu=true;
			state.reset();
			state.core();
			return;
		}

		if(yoffset_target!=TRANSITION_OUT_TARGET){
			state.core();
		}

		state.render();
		render(state.renderer);
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	throw need_to_exit();
}

void MenuGameover::render(const Renderer &renderer)const{
	// full white background
	glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,full_white_alpha);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_FULL_WHITE].object);
	renderer.draw(full_white);

	// entry + skyline
	glUniform4f(renderer.uniform.rgba,0.0627f,0.7412f,0.9294f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_FULL_WHITE].object);
	if(yoffset_target==TRANSITION_OUT_TARGET)
		renderer.draw(entry,yoffset);
	else
		renderer.draw(entry);
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_LOWERBACKDROP].object);
	if(yoffset_target==TRANSITION_OUT_TARGET)
		renderer.draw(entry,yoffset);
	else
		renderer.draw(entry);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_BACKDROPGROUND].object);
	if(yoffset_target==TRANSITION_OUT_TARGET)
		renderer.draw(entry,yoffset);
	else
		renderer.draw(entry);

	// background
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_GAME_OVER].object);
	renderer.draw(background,yoffset);

	// attachment
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_ATTACHMENT].object);
	if(attachment.y==ATTACHMENT_Y_TARGET)
		renderer.draw(attachment,yoffset);
	else
		renderer.draw(attachment);

	// buttons
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_BUTTON].object);
	again.render(renderer,yoffset);
	menu.render(renderer,yoffset);

	// button labels
	glBindTexture(GL_TEXTURE_2D,renderer.font.button->atlas);
	again.render_text(renderer,renderer.font.button,yoffset);
	menu.render_text(renderer,renderer.font.button,yoffset);

	// header text
	glBindTexture(GL_TEXTURE_2D,renderer.font.header->atlas);
	drawtextcentered(renderer.font.header,0.0f,-5.0f+yoffset,header);

	// scoreboard
	const float newline=0.45f;
	float entry_offset=-1.5f;
	glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	for(int i=SCOREBOARD_COUNT-1;i>=0;--i){
		char entry[20];
		if(scoreboard[i]==0)
			sprintf(entry,"%d: -",SCOREBOARD_COUNT-i);
		else
			sprintf(entry,"%d: %dm",SCOREBOARD_COUNT-i,scoreboard[i]);
		if(i==scoreboard_index){
			glUniform4f(renderer.uniform.rgba,1.0f,1.0f,0.0f,1.0f);
			drawtext(renderer.font.main,scoreboard_x,entry_offset+yoffset,entry);
			glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
		}
		else
			drawtext(renderer.font.main,scoreboard_x,entry_offset+yoffset,entry);

		entry_offset+=newline;
	}

	// your score
	char your_score[50];
	sprintf(your_score,"Your score: %dm",score);
	drawtextcentered(renderer.font.main,0.0f,-3.0f+yoffset,your_score);

	if(attachment.y==ATTACHMENT_Y_TARGET){
		glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
		local->fake_render(yoffset+22.4f);
	}
}

static void swap(int *i,int *j){
	int temp=*i;
	*i=*j;
	*j=temp;
}
void selection(int *a){
	const int n=5;
	int	i,j;
	for(j=0;j<n-1;j++){
		int min=j;
		for(i=j+1;i<n;i++){
			if(a[i]<a[min]){
				min=i;
			}
		}
		if(min!=j){
			swap(a+j,a+min);
		}
	}
}
