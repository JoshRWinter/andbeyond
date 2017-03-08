#include "glesutil.h"

#define TID_SKY 0

struct base_s{
	float x,y,w,h,rot;
	int count,frame; // <frame> of <count> in a sprite sheet
};

#define PLAYER_WIDTH 1.0f
#define PLAYER_HEIGHT 0.8f
struct player_s:base_s{
	float xv,yv;
};

struct renderer_s{
	void init(android_app*);
	void term();
	void draw(base_s&);

	pack assets; // gameplay textures
	pack uiassets; // ui textures
	apack aassets; // sound effects

	// world rectangle
	struct{float left,right,bottom,top;}rect;

	// screen resolution
	device dev; // real resolution
	device screen; // altered resolution

	// uniforms
	struct{int vector,size,rot,texcoords,rgba,projection;}uniform;

	// fonts
	struct{ftfont *main;}font;

	unsigned vao,vbo,program;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
};

struct state_s{
	state_s();
	void reset();
	bool core();
	void render();
	int process();

	// renderer information
	renderer_s renderer;

	bool running;

	crosshair pointer[2]; // supports 2 fingers on the screen
	jni_info jni; // java native interface
	android_app *app;

	// entities
	base_s background;
	player_s player;
};
	
bool process(android_app*);
int32_t inputproc(android_app*,AInputEvent*);
void cmdproc(android_app*,int32_t);
void init_display(state_s&);
void term_display(state_s&);
bool core(state_s&);
void render(state_s&);

