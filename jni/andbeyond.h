#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <vector>
#include "glesutil.h"
struct Renderer;
struct State;
#include "object/object.h"


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


struct Renderer{
	void init(android_app*);
	void term();
	void draw(const Base&,bool)const;

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

struct State{
	State();
	void reset();
	bool core();
	void render()const;
	int process();

	// renderer information
	Renderer renderer;

	bool running;
	float tilt; // accelerometer
	float timer_game;
	float height;

	crosshair pointer[2]; // supports 2 fingers on the screen
	jni_info jni; // java native interface
	accel_info accel; // accelerometer info
	android_app *app;

	// entities
	Backdrop lower_backdrop,upper_backdrop,backdrop_1,backdrop_2;
	Player player;
	std::vector<Platform*> platform_list;
	std::vector<Saw*> saw_list;
	std::vector<Particle*> particle_list;
	std::vector<Electro*> electro_list;
	std::vector<Smasher*> smasher_list;
	std::vector<Scenery*> scenery_list;
};

int32_t inputproc(android_app*,AInputEvent*);
void cmdproc(android_app*,int32_t);
