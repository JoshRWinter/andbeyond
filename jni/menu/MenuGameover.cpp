#include "../andbeyond.h"

bool MenuGameover::exec(State &state){
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

	yoffset_target=0.0f;
	yoffset=state.renderer.rect.top*2.0f;
	full_white_alpha=0.0f;
	local=&state;

	while(state.process()){
		// handle transition
		if(targetf(&yoffset,(fabsf(yoffset-yoffset_target)/20.0f)+0.1f,yoffset_target)==TRANSITION_OUT_TARGET)
			return true;

		// handle attachment animation
		targetf(&attachment.y,ATTACHMENT_Y_SPEED,ATTACHMENT_Y_TARGET);

		// make the background more opaque
		targetf(&full_white_alpha,FULL_WHITE_TRANSITION_SPEED,yoffset_target==TRANSITION_OUT_TARGET?1.0f:FULL_WHITE_TARGET_ALPHA);

		// process buttons
		if(again.process(state)&&attachment.y==ATTACHMENT_Y_TARGET&&yoffset_target!=TRANSITION_OUT_TARGET){
			state.reset();
				Platform::process(state);
			yoffset_target=TRANSITION_OUT_TARGET;
		}
		if(menu.process(state)){
			state.show_menu=true;
			state.reset();
			return state.core();
		}

		if(yoffset_target!=TRANSITION_OUT_TARGET){
			if(!state.core())
				return false;
		}
		state.render();
		render(state.renderer);
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	return false;
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
	glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
	drawtextcentered(renderer.font.main,0.0f,-3.0f+yoffset,"u ded");

	if(attachment.y==ATTACHMENT_Y_TARGET){
		glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
		local->fake_render(yoffset+22.4f);
	}
}
