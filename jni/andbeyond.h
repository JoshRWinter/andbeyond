#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <vector>
#include "glesutil.h"
struct Renderer;
struct State;
#include "object/object.h"
#include "menu/Menu.h"

#define DATAPATH "/data/data/joshwinter.andbeyond/files"
//#define SHOW_FPS
//#define INVINCIBLE
//#define START_HIGH 540.0f

// gameplay assets
#define TID_PLAYER 0
#define TID_BACKDROPGROUND 1
#define TID_BACKDROP_FIRST 2
#define TID_BACKDROP_LAST 8
#define TID_BACKDROP_TRANSITION 9
#define TID_LOWERBACKDROP 10
#define TID_UPPERBACKDROP 11
#define TID_SCENERY_BLIMP 12
#define TID_SCENERY_MOON 13
#define TID_SCENERY_STARBABY 14

// gameplay atlas
#define AID_PLATFORM_NORMAL 0
#define AID_PLATFORM_DISAPPEARING 1
#define AID_PLATFORM_SLIDING 2
#define AID_PLATFORM_SPACE_NORMAL 3
#define AID_PLATFORM_SPACE_DISAPPEARING 4
#define AID_PLATFORM_SPACE_SLIDING 5
#define AID_ELECTRO_1 6
#define AID_ELECTRO_2 7
#define AID_ELECTRO_3 8
#define AID_ELECTRO_4 9
#define AID_ELECTRO_5 10
#define AID_ELECTRO_SPACE_1 11
#define AID_ELECTRO_SPACE_2 12
#define AID_ELECTRO_SPACE_3 13
#define AID_ELECTRO_SPACE_4 14
#define AID_ELECTRO_SPACE_5 15
#define AID_OBSTACLE_NORMAL 16
#define AID_OBSTACLE_SPACE 17
#define AID_RAIL 18
#define AID_PARTICLE 19
#define AID_PLAYER_NORMAL 20
#define AID_PLAYER_MIDBLINK 21
#define AID_PLAYER_BLINK 22
#define AID_SMASHER_NORMAL 23
#define AID_SMASHER_SPACE 24
#define AID_SPRING 25
#define AID_SCOREBOARD 26

// light atlas
#define AID_LIGHT_1 0
#define AID_LIGHT_2 1
#define AID_LIGHT_3 2
#define AID_LIGHT_4 3

// ui assets
#define TID_MENU 0
#define TID_BUTTON 1
#define TID_FULL_WHITE 2
#define TID_GAME_OVER 3
#define TID_ATTACHMENT 4

#define TRANSITION_SPACE_HEIGHT 520.0f
#define PLAYER_UPWARD_VELOCITY 0.26f
#define PLAYER_SUPER_UPWARD_VELOCITY 0.4f
#define PLAYER_BASELINE -1.0f
#define UPPER_BACKDROP_FULL_TRANSITION 430.0f
#define UPPER_BACKDROP_START_TRANSITION 405.0f
#define TILT_DIVISOR 20.0f
#define GRAVITY 0.007f
#define TERMINAL_VELOCITY 0.4f
#define TIMER_GAME_OVER 55
#define SCOREBOARD_COUNT 5
#define TEXT_COLOR 0.7816f,0.7816f,0.7237f

#define in_space(h) (h>TRANSITION_SPACE_HEIGHT)
#define around(h,m) (h>m-1.0f&&h<m+1.0f)

#define COLLIDE_TOP 1
#define COLLIDE_LEFT 2
#define COLLIDE_RIGHT 3
#define COLLIDE_BOTTOM 4

struct Renderer{
	void init(android_app*);
	void term();
	void draw(const Base&,const Atlas*,bool=false)const;
	void draw(const Base&,const Atlas*,float)const;

	pack assets; // gameplay textures
	pack uiassets; // ui textures
	apack aassets; // sound effects
	Atlas atlas,atlas_light;

	// world rectangle
	struct{float left,right,bottom,top;}rect;

	// screen resolution
	device dev; // real resolution
	device screen; // altered resolution

	// uniforms
	struct{int vector,size,rot,texcoords,rgba,projection;}uniform;

	// fonts
	struct{ftfont *main,*button,*header;}font;

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
	void core();
	void render()const;
	void fake_render(float)const;
	int process();
	void read_config();
	void write_config();
	void read_score();
	void write_score();

	// renderer information
	Renderer renderer;

	bool running,back,show_menu,show_gameover;
	float tilt; // accelerometer
	float timer_game;
	float height;
	int timer_game_over;
	int scoreboard[SCOREBOARD_COUNT];

	// menus
	struct{
		MenuMain main;
		MenuGameover gameover;
		MenuConfig config;
		MenuPause pause;
		MenuMessage message;
	}menu;

	// settings
	struct{
		bool sounds;
		bool vibration;
	}config;

	crosshair pointer[2]; // supports 2 fingers on the screen
	jni_info jni; // java native interface
	accel_info accel; // accelerometer info
	android_app *app;

	// entities
	Backdrop lower_backdrop,upper_backdrop,backdrop_1,backdrop_2;
	Base scorecounter;
	Player player;
	std::vector<Platform*> platform_list;
	std::vector<Saw*> saw_list;
	std::vector<Particle*> particle_list;
	std::vector<Electro*> electro_list;
	std::vector<Smasher*> smasher_list;
	std::vector<Scenery> scenery_list;
	std::vector<Light> light_list;
};

int32_t inputproc(android_app*,AInputEvent*);
void cmdproc(android_app*,int32_t);
