#include "andbeyond.h"

extern const char *vertexshader,*fragmentshader;

// set up opengl using egl
// set up fonts, textures, sound effects
void Renderer::init(android_app *app){
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
	if(!loadpack(&uiassets,app->activity->assetManager,"uiassets",NULL))
		logcat("ui texture init error");

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
	font.button=create_ftfont(app->activity->assetManager,0.6f,"arial.ttf");
	font.header=create_ftfont(app->activity->assetManager,0.9f,"arial.ttf");
}

void Renderer::term(){
	destroy_ftfont(font.main);
	destroy_ftfont(font.button);
	destroy_ftfont(font.header);
	glDeleteBuffers(1,&vbo);
	glDeleteVertexArrays(1,&vao);
	glDeleteProgram(program);
	destroypack(&assets);
	destroypack(&uiassets);

	eglMakeCurrent(display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
	eglDestroyContext(display,context);
	eglDestroySurface(display,surface);
	eglTerminate(display);
}

void Renderer::draw(const Base &base,bool xflip)const{
	const float size=1.0f/base.count;
	const float left=size*base.frame;
	const float right=left+size;

	if(xflip)
		glUniform4f(uniform.texcoords,right,left,0.0f,1.0f);
	else
		glUniform4f(uniform.texcoords,left,right,0.0f,1.0f);
	glUniform2f(uniform.vector,base.x,base.y);
	glUniform2f(uniform.size,base.w,base.h);
	glUniform1f(uniform.rot,base.rot);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void Renderer::draw(const Base &base,float yoffset)const{
	const float size=1.0f/base.count;
	const float left=size*base.frame;
	const float right=left+size;

	glUniform4f(uniform.texcoords,left,right,0.0f,1.0f);
	glUniform2f(uniform.vector,base.x,base.y+yoffset);
	glUniform2f(uniform.size,base.w,base.h);
	glUniform1f(uniform.rot,base.rot);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}
