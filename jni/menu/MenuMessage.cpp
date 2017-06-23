#include "../andbeyond.h"

bool MenuMessage::exec(State &state,const Menu &parent,const char *msg,const char *hdr){
	// backgrounds
	background.x=MENU_CARD_X;
	background.y=MENU_CARD_Y;
	background.w=MENU_CARD_W;
	background.h=MENU_CARD_H;
	background.rot=0.0f;
	background.count=1;
	background.frame=0;
	full_white.background(state.renderer);

	// button
	ok.init(-BUTTON_WIDTH/2.0f,3.5f,"OK");

	full_white_alpha=0.0f;
	full_white_alpha_target=FULL_WHITE_TARGET_ALPHA;
	yoffset=state.renderer.rect.top*2.0f;
	yoffset_target=0.0f;
	text=msg;
	header=hdr;
	local=&parent;

	while(state.process()){
		// full white fade
		targetf(&full_white_alpha,FULL_WHITE_TRANSITION_SPEED,full_white_alpha_target);

		// yoffset transition
		if(targetf(&yoffset,(fabsf(yoffset-yoffset_target)/20.0f)+0.1f,yoffset_target)==state.renderer.rect.bottom*2.0f)
			return true;

		if(ok.process(state)||state.back){
			state.back=false;
			yoffset_target=state.renderer.rect.bottom*2.0f;
			full_white_alpha_target=0.0f;
		}

		render(state.renderer);
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	return false;
}

void MenuMessage::render(const Renderer &renderer)const{
	local->render(renderer);

	// full white
	glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,full_white_alpha);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_FULL_WHITE].object);
	renderer.draw(full_white);

	// menu card
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_GAME_OVER].object);
	renderer.draw(background,yoffset);

	// buttons
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_BUTTON].object);
	ok.render(renderer,yoffset);

	// button label
	glBindTexture(GL_TEXTURE_2D,renderer.font.button->atlas);
	ok.render_text(renderer,renderer.font.button,yoffset);

	// header text
	glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
	if(header!=NULL)
		drawtextcentered(renderer.font.main,0.0f,-5.0f+yoffset,header);

	// main text
	drawtextcentered(renderer.font.main,0.0f,-2.0f+yoffset,text);
}
