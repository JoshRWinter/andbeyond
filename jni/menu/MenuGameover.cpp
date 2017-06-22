#include "../andbeyond.h"

bool MenuGameover::exec(State &state){
	full_white.background(state.renderer);
	const float x_inward=0.75f,y_inward=1.5f;
	background.x=state.renderer.rect.left+x_inward;
	background.y=state.renderer.rect.top+y_inward;
	background.w=(state.renderer.rect.right*2.0f)-(x_inward*2.0f);
	background.h=(state.renderer.rect.bottom*2.0f)-(y_inward*2.0f);
	background.rot=0.0f;
	background.count=1;
	background.frame=0;

	// buttons
	again.init(-BUTTON_WIDTH/2.0f,1.0f,"Play again");
	menu.init(-BUTTON_WIDTH/2.0f,again.y+BUTTON_HEIGHT+0.2f,"Menu");

	yoffset_target=0.0f;
	yoffset=state.renderer.rect.top*2.0f;
	full_white_alpha=0.0f;

	while(state.process()){
		// handle transition
		targetf(&yoffset,(fabsf(yoffset-yoffset_target)/20.0f)+0.1f,yoffset_target);

		// make the background more opaque
		targetf(&full_white_alpha,FULL_WHITE_TRANSITION_SPEED,FULL_WHITE_TARGET_ALPHA);

		// process buttons
		if(again.process(state)){
			state.reset();
			return true;
		}
		if(menu.process(state)){
			state.show_menu=true;
			state.reset();
			return state.core();
		}

		if(!state.core())
			return false;
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

	// background
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_GAME_OVER].object);
	renderer.draw(background,yoffset);

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
}
