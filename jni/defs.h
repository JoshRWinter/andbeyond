#include "glesutil.h"

#include <vector>

#define SHOW_FPS

#define TID_PLATFORM 0
#define TID_PLAYER 1
#define TID_BACKDROPGROUND 2
#define TID_BACKDROP_FIRST 3
#define TID_BACKDROP_LAST 9
#define TID_LOWERBACKDROP 10
#define TID_SPRING 11

#define HEIGHT_INCREMENT 0.1f
#define PLAYER_UPWARD_VELOCITY 0.26f
#define PLAYER_SUPER_UPWARD_VELOCITY 0.4f
#define PLAYER_BASELINE -3.0f
#define TILT_DIVISOR 20.0f
#define GRAVITY 0.007f
#define TERMINAL_VELOCITY 0.4f

struct state_s;

struct base_s{
	bool collide(const base_s&,float);

	float x,y,w,h,rot;
	int count,frame; // <frame> of <count> in a sprite sheet
};

#define PLAYER_WIDTH 0.9f
#define PLAYER_HEIGHT 1.025f
struct player_s:base_s{
	float xv,yv;
	float apex; // highest point in the jump
};

#define SPRING_WIDTH 0.3f
#define SPRING_HEIGHT 0.2f
struct spring_s:base_s{
	spring_s();

	float xoffset;
};

#define PLATFORM_WIDTH 1.3f
#define PLATFORM_HEIGHT 0.3625f
#define PLATFORM_NORMAL 0
#define PLATFORM_X_VELOCITY 0.05f
struct platform_s:base_s{
	platform_s(const state_s&,float,int);

	bool has_spring;
	spring_s spring;
	int type;
	bool xflip;
	float xv;
};

struct backdrop_s:base_s{
	int tid; // texture id
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
	float tilt; // accelerometer
	float timer_game;
	float height;

	crosshair pointer[2]; // supports 2 fingers on the screen
	jni_info jni; // java native interface
	accel_info accel; // accelerometer info
	android_app *app;

	// entities
	backdrop_s lower_backdrop,backdrop_1,backdrop_2;
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

