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
#define TID_OBSTACLE 12
#define TID_OBSTACLERAIL 13
#define TID_PARTICLE 14

#define HEIGHT_INCREMENT 0.1f
#define PLAYER_UPWARD_VELOCITY 0.26f
#define PLAYER_SUPER_UPWARD_VELOCITY 0.4f
#define PLAYER_BASELINE -3.0f
#define TILT_DIVISOR 20.0f
#define GRAVITY 0.007f
#define TERMINAL_VELOCITY 0.4f

#define COLLIDE_TOP 1
#define COLLIDE_LEFT 2
#define COLLIDE_RIGHT 3
#define COLLIDE_BOTTOM 4

struct state_s;

struct base_s{
	bool collide(const base_s&,float);
	int correct(const base_s&);

	float x,y,w,h,rot;
	int count,frame; // <frame> of <count> in a sprite sheet
};

#define PLAYER_WIDTH 1.0f
#define PLAYER_HEIGHT 1.1375f
struct player_s:base_s{
	float xv,yv;
	float apex; // highest point in the jump
	bool dead;
};

#define SPRING_WIDTH 0.3f
#define SPRING_HEIGHT 0.2f
struct spring_s:base_s{
	spring_s();

	float xoffset;
};

#define PLATFORM_DONTCARE -1
#define PLATFORM_NORMAL 0
#define PLATFORM_SLIDING 1
#define PLATFORM_DISAPPEARING 2
#define PLATFORM_DISAPPEARING_FADE 0.07f
#define PLATFORM_WIDTH 1.3f
#define PLATFORM_HEIGHT 0.45f
#define PLATFORM_X_VELOCITY 0.05f
struct platform_s:base_s{
	platform_s(const state_s&,float,int);

	bool has_spring;
	spring_s spring;
	int type;
	bool xflip;
	float alpha;
	float xv;
};

#define OBSTACLE_SIZE 1.35f
#define OBSTACLERAIL_HEIGHT 0.1f
#define OBSTACLE_VELOCITY 0.095f
#define OBSTACLE_SPIN_SPEED 0.15f
struct obstacle_s:base_s{
	obstacle_s(const state_s&);

	base_s rail;
	float xv;
};

#define PARTICLE_TERMINAL_VELOCITY 0.15f
#define PARTICLE_TTL 7,18
#define PARTICLE_SPEED 0.275f
#define PARTICLE_WIDTH 0.5f
#define PARTICLE_HEIGHT 0.035f
#define PARTICLE_DRAG 0.001f
struct particle_s:base_s{
	particle_s(const state_s&,float,float,bool);

	float xv,yv;
	float ttl;
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
	std::vector<obstacle_s*> obstacle_list;
	std::vector<particle_s*> particle_list;
};

bool process(android_app*);
int32_t inputproc(android_app*,AInputEvent*);
void cmdproc(android_app*,int32_t);
void init_display(state_s&);
void term_display(state_s&);
bool core(state_s&);
void render(state_s&);
