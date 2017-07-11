#include <time.h>
#include <unistd.h>

#include "andbeyond.h"

int32_t inputproc(android_app *app,AInputEvent *event){
	State *state=(State*)app->userData;
	int32_t type=AInputEvent_getType(event);
	if(type==AINPUT_EVENT_TYPE_MOTION){
		return retrieve_touchscreen_input(event,state->pointer,state->renderer.dev.w,state->renderer.dev.h,state->renderer.rect.right*2.0f,state->renderer.rect.bottom*2.0f);
	}
	else if(type==AINPUT_EVENT_TYPE_KEY){
		int32_t key=AKeyEvent_getKeyCode(event);
		int32_t action=AKeyEvent_getAction(event);
		if(key==AKEYCODE_BACK&&action==AKEY_EVENT_ACTION_UP){
			state->back=true;
			return true;
		}
		else if(key==AKEYCODE_MENU&&action==AKEY_EVENT_ACTION_UP){
			state->reset();
			return true;
		}
	}
	return false;
}

void cmdproc(android_app *app, int32_t cmd){
	State *state=(State*)app->userData;
	switch(cmd){
	case APP_CMD_START:
		usleep(300000); // dont ask
		break;
	case APP_CMD_RESUME:
		hidenavbars(&state->jni);
		keep_screen_on(&state->jni);
		break;
	case APP_CMD_INIT_WINDOW:
		state->running=true;
		state->renderer.init(app);
		break;
	case APP_CMD_TERM_WINDOW:
		state->running=false;
		state->renderer.term();
		break;
	case APP_CMD_DESTROY:
		state->reset();
		break;
	case APP_CMD_GAINED_FOCUS:
		enable_accel(&state->accel);
		break;
	case APP_CMD_LOST_FOCUS:
		disable_accel(&state->accel);
		break;
	}
}

// entry point
void android_main(android_app *app){
	logcat("--- BEGIN NEW LOG ---");
	srand48(time(NULL));
	State state;
	state.reset();
	state.app=app;
	app->onAppCmd=cmdproc;
	app->onInputEvent=inputproc;
	app->userData=&state;
	init_jni(app,&state.jni);
	init_accel(app,&state.accel);
	state.accel.x=0.0f;

	// three parts to a happy Main Loop:
	// state.process() -- synchronously dispatch system messages (onStart(), onResume(), touch input events etc)
	// state.core() -- allow entities to "think"
	// state.render() -- draw everything
	while(state.process()){

		try{
			state.core();
		}catch(const need_to_exit &e){
			break;
		}

		state.render();
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	term_accel(&state.accel);
	term_jni(&state.jni);
	logcat("--- END LOG ---");
}
