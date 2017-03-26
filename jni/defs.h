#include "glesutil.h"

#include <vector>

#define SHOW_FPS
//#define INVINCIBLE
//#define START_HIGH

#define TID_PLATFORM 0
#define TID_PLAYER 1
#define TID_BACKDROPGROUND 2
#define TID_BACKDROP_FIRST 3
#define TID_BACKDROP_LAST 9
#define TID_BACKDROP_TRANSITION 10
#define TID_LOWERBACKDROP 11
#define TID_SPRING 12
#define TID_SAW 13
#define TID_SAWRAIL 14
#define TID_PARTICLE 15
#define TID_UPPERBACKDROP 16
#define TID_ELECTRO 17
#define TID_SMASHER 18
#define TID_SCENERY_BLIMP 19
#define TID_SCENERY_MOON 20

#define TRANSITION_SPACE_HEIGHT 520.0f
#define PLAYER_UPWARD_VELOCITY 0.26f
#define PLAYER_SUPER_UPWARD_VELOCITY 0.4f
#define PLAYER_BASELINE -1.0f
#define UPPER_BACKDROP_FULL_TRANSITION 430.0f
#define UPPER_BACKDROP_START_TRANSITION 405.0f
#define TILT_DIVISOR 20.0f
#define GRAVITY 0.007f
#define TERMINAL_VELOCITY 0.4f

#define in_space(h) (h>TRANSITION_SPACE_HEIGHT)
#define around(h,m) (h>m-1.0f&&h<m+1.0f)

#define COLLIDE_TOP 1
#define COLLIDE_LEFT 2
#define COLLIDE_RIGHT 3
#define COLLIDE_BOTTOM 4

struct state_s;
struct renderer_s;

struct base_s{
	bool collide(const base_s&,float)const;
	bool collide_y(const base_s&,float)const;
	int correct(const base_s&);

	float x,y,w,h,rot;
	int count,frame; // <frame> of <count> in a sprite sheet
};

#define PLAYER_WIDTH 1.075f
#define PLAYER_HEIGHT 1.225f
struct player_s:base_s{
	void process(state_s&);
	void render(const renderer_s&)const;

	float xv,yv;
	float apex; // highest point in the jump
	bool dead;
	float timer_frame; // animation queues
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
	static void process(state_s&);
	static void render(const renderer_s&,const std::vector<platform_s*>&);

	bool has_spring;
	spring_s spring;
	int type;
	bool xflip;
	float xv,yv;
};

#define SAW_SIZE 1.35f
#define SAWRAIL_HEIGHT 0.1f
#define SAW_VELOCITY 0.095f
#define SAW_SPIN_SPEED 0.15f
struct saw_s:base_s{
	saw_s(const state_s&);
	static void process(state_s&);
	static void render(const renderer_s&,const std::vector<saw_s*>&);
	static void clear_all_ahead(std::vector<saw_s*>&,float);

	base_s rail;
	float xv;
};

#define ELECTRO_WIDTH 3.75f
#define ELECTRO_HEIGHT 1.0125f
struct electro_s:base_s{
	electro_s(const state_s&);
	static void process(state_s&);
	static void render(const renderer_s&,const std::vector<electro_s*>&);
	static void clear_all_ahead(std::vector<electro_s*>&,float);

	float timer_frame;
};

#define SMASHER_WIDTH 4.5f
#define SMASHER_HEIGHT 1.5f
#define SMASHER_RETRACT 3.675f
#define SMASHER_SMASH_TIMER 110.0f
#define SMASHER_RETRACT_TIMER 50.0f
#define SMASHER_SMASH_VELOCITY 0.2f
#define SMASHER_RETRACT_VELOCITY 0.05f 
struct smasher_s{
	smasher_s(const state_s&);
	static void process(state_s&);
	static void render(const renderer_s&,const std::vector<smasher_s*>&);
	static void clear_all_ahead(std::vector<smasher_s*>&,float);

	base_s left,right;
	bool retracting; // direction
	bool smashed; // currently smashed
	float timer;
};

#define SCENERY_PARALLAX 15.0f
#define SCENERY_BLIMP 1
#define SCENERY_MOON 2
#define SCENERY_BLIMP_WIDTH 2.75f
#define SCENERY_BLIMP_HEIGHT 1.025f
#define SCENERY_MOON_SIZE 3.5f
struct scenery_s:base_s{
	scenery_s(const state_s&,int);
	static void process(state_s&);
	static void render(const renderer_s&,const std::vector<scenery_s*>&);

	int type;
	int tid; // texture id
	bool xflip;
};

#define PARTICLE_TERMINAL_VELOCITY 0.15f
#define PARTICLE_TTL 7,18
#define PARTICLE_SPEED 0.275f
#define PARTICLE_WIDTH 0.5f
#define PARTICLE_HEIGHT 0.035f
#define PARTICLE_DRAG 0.001f
struct particle_s:base_s{
	particle_s(const state_s&,float,float,bool);
	static void process(state_s&);
	static void render(const renderer_s&,const std::vector<particle_s*>&);

	float xv,yv;
	float ttl;
};

struct backdrop_s:base_s{
	int tid; // texture id
};

struct renderer_s{
	void init(android_app*);
	void term();
	void draw(const base_s&,bool)const;

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
	void render()const;
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
	backdrop_s lower_backdrop,upper_backdrop,backdrop_1,backdrop_2;
	player_s player;
	std::vector<platform_s*> platform_list;
	std::vector<saw_s*> saw_list;
	std::vector<particle_s*> particle_list;
	std::vector<electro_s*> electro_list;
	std::vector<smasher_s*> smasher_list;
	std::vector<scenery_s*> scenery_list;
};

int32_t inputproc(android_app*,AInputEvent*);
void cmdproc(android_app*,int32_t);

bool too_close(const base_s&,const std::vector<saw_s*>&,const std::vector<electro_s*>&,const std::vector<smasher_s*>&);
