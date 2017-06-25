#include "../andbeyond.h"

bool MenuPause::exec(State &state){
	// full_white
	full_white.background(state.renderer);

	// buttons
	menu.init(-BUTTON_WIDTH/2.0f,0.0f,"Menu");
	settings.init(-BUTTON_WIDTH/2.0f,menu.y+BUTTON_HEIGHT+0.2f,"Settings");
	back.init(-BUTTON_WIDTH/2.0f,5.0f,"Resume");

	local=&state;
	no_fingers=!state.pointer[0].active&&!state.pointer[1].active;
	alpha=0.0f;

	while(state.process()){
		if(!no_fingers)
			no_fingers=!state.pointer[0].active&&!state.pointer[1].active;

		// alpha fade in
		targetf(&alpha,0.06f,0.8f);

		// buttons
		if(no_fingers){
			if(menu.process(state)){
				state.show_menu=true;
				state.reset();
				return state.core();
			}
			if(settings.process(state)){
				if(!state.menu.config.exec(state,*this))
					return false;
			}
			if(back.process(state)||state.back){
				state.back=false;
				return true;
			}
		}

		render(state.renderer);
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	return false;
}

void MenuPause::render(const Renderer &renderer)const{
	// render parent
	local->render();

	// background
	glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,alpha);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_FULL_WHITE].object);
	renderer.draw(full_white);

	// buttons
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_BUTTON].object);
	menu.render(renderer);
	settings.render(renderer);
	back.render(renderer);

	// button labels
	glBindTexture(GL_TEXTURE_2D,renderer.font.button->atlas);
	menu.render_text(renderer,renderer.font.button);
	settings.render_text(renderer,renderer.font.button);
	back.render_text(renderer,renderer.font.button);

	// header
	glBindTexture(GL_TEXTURE_2D,renderer.font.header->atlas);
	drawtextcentered(renderer.font.header,0.0f,-5.0f,"PAUSED");
}
