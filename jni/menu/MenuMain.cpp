#include "../andbeyond.h"

bool MenuMain::exec(State &state){
	// buttons
	const float BUTTON_OFFSET=BUTTON_HEIGHT+0.2f;
	play.init(-BUTTON_WIDTH/2.0f,1.0f,"Play");
	settings.init(-BUTTON_WIDTH/2.0f,play.y+BUTTON_OFFSET,"Settings");
	about.init(-BUTTON_WIDTH/2.0f,settings.y+BUTTON_OFFSET,"Aboot");
	quit.init(-BUTTON_WIDTH/2.0f,about.y+BUTTON_OFFSET,"Quit");

	// player
	player.w=PLAYER_WIDTH;
	player.h=PLAYER_HEIGHT;
	player.x=-PLAYER_WIDTH/2.0f;
	player.y=PLAYER_BOUNCE_Y-PLAYER_HEIGHT;
	player.rot=0.0f;
	player.count=3;
	player.frame=0;
	player.xv=0.0f;
	player.yv=1.0f;

	// backdrops
	background.background(state.renderer);
	entry.background(state.renderer);
	entry.y=background.y+background.h;
	entry_skyline.background(state.renderer);

	yoffset=0.0f;
	slide=0.0f;
	local=&state;

	while(state.process()){
		if(yoffset==0.0f){
			// buttons
			if(play.process(state)){
				slide=MENU_SLIDE;
				Platform::process(state); // ensure new platforms are generated
			}
			if(settings.process(state))
				;
			if(about.process(state))
				;
			if(quit.process(state))
				ANativeActivity_finish(state.app->activity);
		}

		// process bouncing player
		player.y+=player.yv;
		player.yv+=GRAVITY;
		if(player.y+PLAYER_HEIGHT>PLAYER_BOUNCE_Y){
			player.y=PLAYER_BOUNCE_Y-PLAYER_HEIGHT;
			player.yv=-PLAYER_UPWARD_VELOCITY/1.5f;
		}

		// handle slide transition
		if(slide!=0.0f){
			slide+=MENU_SLIDE_RETARD;
			if(slide>0.0f)
				slide=0.0f;

			yoffset+=slide;
			if(yoffset<state.renderer.rect.top*2.0f)
				return true;
		}

		render(state.renderer);
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	return false;
}

void MenuMain::render(const Renderer &renderer)const{
	glClear(GL_COLOR_BUFFER_BIT);

	// backgrounds
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_LOWERBACKDROP].object);
	renderer.draw(entry_skyline);
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_MENU].object);
	renderer.draw(background,yoffset);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_BACKDROPGROUND].object);
	renderer.draw(entry,yoffset);

	// draw the platforms during the transition
	local->fake_render(yoffset+(renderer.rect.bottom*2.0f));

	// buttons
	glBindTexture(GL_TEXTURE_2D,renderer.uiassets.texture[TID_BUTTON].object);
	play.render(renderer,yoffset);
	settings.render(renderer,yoffset);
	about.render(renderer,yoffset);
	quit.render(renderer,yoffset);

	// button labels
	glBindTexture(GL_TEXTURE_2D,renderer.font.button->atlas);
	play.render_text(renderer,renderer.font.button,yoffset);
	settings.render_text(renderer,renderer.font.button,yoffset);
	about.render_text(renderer,renderer.font.button,yoffset);
	quit.render_text(renderer,renderer.font.button,yoffset);

	// player
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_PLAYER].object);
	renderer.draw(player,yoffset);
}
