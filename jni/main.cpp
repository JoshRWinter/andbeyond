#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <time.h>
#include <unistd.h>

#include "defs.h"

extern const char *vertexshader,*fragmentshader;

// set up opengl using egl
// set up fonts, textures, sound effects
void renderer_s::init(android_app *app){
	initextensions(); // opengl extensions
	getdims(&dev,app->window,DIMS_PORT);
	screen.w=dev.w>720?720:504;
	screen.h=dev.h>1280?1280:896;
	display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display,NULL,NULL);
	EGLConfig config;
	int configcount;
	int config_attr[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
	eglChooseConfig(display,config_attr,&config,1,&configcount);
	ANativeWindow_setBuffersGeometry(app->window,screen.w,screen.h,0);
	surface=eglCreateWindowSurface(display,config,app->window,NULL);
	int context_attr[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};
	context=eglCreateContext(display,config,NULL,context_attr);
	eglMakeCurrent(display,surface,surface,context);

	// load gameplay textures
	if(!loadpack(&assets,app->activity->assetManager,"assets",NULL))
		logcat("texture init error");
	// load ui textures
	/*if(!loadpack(&uiassets,app->activity->assetManager,"uiassets",NULL))
		logcat("ui texture init error");
		*/

	// shader uniforms
	program=initshaders(vertexshader,fragmentshader);
	glUseProgram(program);
	uniform.vector=glGetUniformLocation(program,"vector");
	uniform.size=glGetUniformLocation(program,"size");
	uniform.rot=glGetUniformLocation(program,"rot");
	uniform.texcoords=glGetUniformLocation(program,"texcoords");
	uniform.rgba=glGetUniformLocation(program,"rgba");
	uniform.projection=glGetUniformLocation(program,"projection");

	// projection matrix
	float matrix[16];
	initortho(matrix,rect.left,rect.right,rect.bottom,rect.top,-1.0f,1.0f);
	glUniformMatrix4fv(uniform.projection,1,false,matrix);

	// VAOs and VBOs
	glGenVertexArrays(1,&vao);
	glGenBuffers(1,&vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	float verts[]={-0.5f,-0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f};
	glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,verts,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,false,0,NULL);

	// other settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0627f,0.7412f,0.9294f,1.0f);

	// set up fonts
	set_ftfont_params(screen.w,screen.h,rect.right*2.0f,rect.bottom*2.0f,uniform.vector,uniform.size,uniform.texcoords);
	font.main=create_ftfont(app->activity->assetManager,0.4f,"arial.ttf");
}

void renderer_s::term(){
	destroy_ftfont(font.main);
	glDeleteBuffers(1,&vbo);
	glDeleteVertexArrays(1,&vao);
	glDeleteProgram(program);
	destroypack(&assets);
	//destroypack(&uiassets);

	eglMakeCurrent(display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
	eglDestroyContext(display,context);
	eglDestroySurface(display,surface);
	eglTerminate(display);
}

int32_t inputproc(android_app *app,AInputEvent *event){
	state_s *state=(state_s*)app->userData;
	int32_t type=AInputEvent_getType(event);
	if(type==AINPUT_EVENT_TYPE_MOTION){
		return retrieve_touchscreen_input(event,state->pointer,state->renderer.dev.w,state->renderer.dev.h,state->renderer.rect.right*2.0f,state->renderer.rect.bottom*2.0f);
	}
	else if(type==AINPUT_EVENT_TYPE_KEY){
		int32_t key=AKeyEvent_getKeyCode(event);
		int32_t action=AKeyEvent_getAction(event);
		if(key==AKEYCODE_BACK&&action==AKEY_EVENT_ACTION_UP){
			ANativeActivity_finish(app->activity);
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
	state_s *state=(state_s*)app->userData;
	switch(cmd){
	case APP_CMD_START:
		usleep(300000); // dont ask
		break;
	case APP_CMD_RESUME:
		hidenavbars(&state->jni);
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

int state_s::process(){
	int ident,events;
	android_poll_source *source;
	while((ident=ALooper_pollAll(running?0:-1,NULL,&events,(void**)&source))>=0){
		if(source)
			source->process(app,source);
		if(ident==LOOPER_ID_USER){
			handle_accel(&accel);
		}
		if(app->destroyRequested)
			return false;
	}
	return true;
}

// entry point
void android_main(android_app *app){
	logcat("--- BEGIN NEW LOG ---");
	app_dummy();
	srand48(time(NULL));
	state_s state;
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
	while(state.process()&&state.core()){
		state.render();
		eglSwapBuffers(state.renderer.display,state.renderer.surface);
	}

	term_accel(&state.accel);
	term_jni(&state.jni);
	logcat("--- END LOG ---");
}
