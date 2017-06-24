#include "../andbeyond.h"

bool MenuConfig::exec(State &state,const Menu &menu){
	// backgrounds
	full_white.background(state.renderer);
	card.x=MENU_CARD_X;
	card.y=MENU_CARD_Y;
	card.w=MENU_CARD_W;
	card.h=MENU_CARD_H;
	card.rot=0.0f;
	card.count=1;
	card.frame=0;

	// buttons
	vibrate.init(-BUTTON_WIDTH/2.0f,1.0f,"Vibration");
	sounds.init(-BUTTON_WIDTH/2.0f,vibrate.y+BUTTON_HEIGHT+0.2f,"Sounds");
	back.init(-BUTTON_WIDTH/2.0f,sounds.y+BUTTON_HEIGHT+0.2f,"Back");

	full_white_alpha=0.0f;
	full_white_alpha_target=FULL_WHITE_TARGET_ALPHA;
	yoffset=state.renderer.rect.top*2.0f;
	yoffset_target=0.0f;
	parent=&menu;
	local=&state;

	while(state.process()){
		// full white fade
		targetf(&full_white_alpha,FULL_WHITE_TRANSITION_SPEED,full_white_alpha_target);

		// yoffset slide
		if(targetf(&yoffset,(fabsf(yoffset_target-yoffset)/20.0f)+0.1f,yoffset_target)==state.renderer.rect.bottom*2.0f)
			return true;

		// buttons
		if(vibrate.process(state)){
			state.config.vibration=!state.config.vibration;
			changed=true;
		}
		if(sounds.process(state)){
			state.config.sounds=!state.config.sounds;
			changed=true;
		}
		if(back.process(state)||state.back){
			state.back=false;
			yoffset_target=state.renderer.rect.bottom*2.0f;
			full_white_alpha_target=0.0f;
			if(changed)
				state.write_config();
		}

		render(state.renderer);
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	return false;
}

void MenuConfig::render(const Renderer &renderer)const{
	// render parent
	parent->render(renderer);

	// full white
	glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,full_white_alpha);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_FULL_WHITE].object);
	renderer.draw(full_white);

	// menu card
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_GAME_OVER].object);
	renderer.draw(card,yoffset);

	// buttons
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_BUTTON].object);
	vibrate.render(renderer,yoffset);
	sounds.render(renderer,yoffset);
	back.render(renderer,yoffset);

	// button labels
	glBindTexture(GL_TEXTURE_2D,renderer.font.button->atlas);
	vibrate.render_text(renderer,renderer.font.button,yoffset);
	sounds.render_text(renderer,renderer.font.button,yoffset);
	back.render_text(renderer,renderer.font.button,yoffset);

	// config text
	char config[150];
	sprintf(config,"Vibration: %s\nSounds: %s",local->config.vibration?"Yes":"No",local->config.sounds?"Yes":"No");
	glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
	drawtextcentered(renderer.font.main,0.0f,-2.0f+yoffset,config);

	// header text
	glBindTexture(GL_TEXTURE_2D,renderer.font.header->atlas);
	drawtextcentered(renderer.font.header,0.0f,-5.0f+yoffset,"Settings");
}
