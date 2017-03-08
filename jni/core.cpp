#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>

#include <vector>
#include <string>

#include "defs.h"

bool state_s::core(){
	return true;
}

void state_s::render(){
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D,renderer.assets.texture[TID_SKY].object);
	glUniform4f(renderer.uniform.rgba,1.0f,1.0f,1.0f,1.0f);
	renderer.draw(background);

	{
		glBindTexture(GL_TEXTURE_2D,renderer.font.main->atlas);
		glUniform4f(renderer.uniform.rgba,0.0f,0.0f,0.0f,1.0f);
		static char msg[30];
		static int last_time=0;
		static int fps=0;
		int current_time=time(NULL);
		if(current_time!=last_time){
			last_time=current_time;
			sprintf(msg,"[and beyond] fps: %d",fps);
			fps=0;
		}
		else
			++fps;

		drawtext(renderer.font.main,renderer.rect.left+0.1f,renderer.rect.top+0.1f,msg);
	}
}

state_s::state_s(){
	running=false;
	renderer.rect.right=4.5f;
	renderer.rect.left=-4.5f;
	renderer.rect.bottom=8.0f;
	renderer.rect.top=-8.0f;

	background.x=renderer.rect.left;
	background.y=renderer.rect.top;
	background.w=renderer.rect.right*2.0f;
	background.h=renderer.rect.bottom*2.0f;
	background.rot=0.0f;
	background.count=1.0f;
	background.frame=0.0f;

	player.x=-PLAYER_WIDTH/2.0f;
	player.y=-PLAYER_HEIGHT/2.0f;
	player.count=1.0f;
	player.frame=0.0f;
}

void state_s::reset(){
	player.xv=0.0f;
	player.yv=0.0f;
	player.rot=0.0f;
}

