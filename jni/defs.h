#include "glesutil.h"

#include <vector>

#define SHOW_FPS

#define TID_SKY 0
#define TID_PLATFORM 1
#define TID_PLAYER 2

#define PLAYER_UPWARD_VELOCITY 0.3f
#define GRAVITY 0.01f
#define TERMINAL_VELOCITY 0.3f

struct state_s;

struct base_s{
	bool collide(const base_s&,float);

	float x,y,w,h,rot;
	int count,frame; // <frame> of <count> in a sprite sheet
};

#define PLAYER_WIDTH 1.0f
#define PLAYER_HEIGHT 0.8f
struct player_s:base_s{
	float xv,yv;
};

#define PLATFORM_WIDTH 1.3f
#define PLATFORM_HEIGHT 0.366666f
#define PLATFORM_NORMAL 0
struct platform_s:base_s{
	platform_s(const state_s&,float,int);

	int type;
	bool xflip;
};

struct renderer_s{
	void init(android_app*);
	void term();
	void draw(const base_s&,bool);

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

	// opengl handles
	unsigned vao,vbo,program;

	// EGL handles
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
	std::vector<platform_s*> platform_list;
};
	
bool process(android_app*);
int32_t inputproc(android_app*,AInputEvent*);
void cmdproc(android_app*,int32_t);
void init_display(state_s&);
void term_display(state_s&);
bool core(state_s&);
void render(state_s&);

