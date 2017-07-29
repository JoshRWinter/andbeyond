struct Saw;
struct Electro;
struct Smasher;

#define SCORECOUNTER_WIDTH 1.8f
#define SCORECOUNTER_HEIGHT 0.8125f

struct Base{
	bool collide(const Base&,float)const;
	bool collide_y(const Base&,float)const;
	bool touching(const crosshair*)const;
	int correct(const Base&);
	bool too_close(const std::vector<Saw*>&,const std::vector<Electro*>&,const std::vector<Smasher*>&);
	void background(const Renderer&);

	float x,y,w,h,rot;
	int texture;
};

#define BUTTON_WIDTH 3.6f
#define BUTTON_HEIGHT 1.3875f
#define BUTTON_PUSH_DOWN 0.1f
struct Button:Base{
	void init(float,float,const char*);
	bool process(State&);
	void render(const Renderer&,float=0.0f)const;
	void render_text(const Renderer&,const ftfont*,float=0.0f)const;

	bool active;
	const char *label;
};

#define PLAYER_WIDTH 1.075f
#define PLAYER_HEIGHT 1.225f
#define PLAYER_KILLED_BY_FALL 1
#define PLAYER_KILLED_BY_SAW 2
#define PLAYER_KILLED_BY_ELECTRO 3
#define PLAYER_KILLED_BY_SMASHER 4
struct Player:Base{
	void process(State&);
	void render(const Renderer&)const;

	float xv,yv;
	float apex; // highest point in the jump
	float alive_y; // shadows Base::y while player is alive
	int dead;
	bool dead_first; // first process flag
	float timer_frame; // animation queues
};

#define SPRING_WIDTH 0.3f
#define SPRING_HEIGHT 0.2f
struct Spring:Base{
	Spring();

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
struct Platform:Base{
	Platform(const State&,float,int);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Platform*>&);

	bool has_spring;
	Spring spring;
	int type;
	bool xflip;
	float xv,yv;
};

#define SAW_SIZE 1.35f
#define SAWRAIL_HEIGHT 0.1f
#define SAW_VELOCITY 0.095f
#define SAW_SPIN_SPEED 0.15f
struct Saw:Base{
	Saw(const State&);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Saw*>&);
	static void clear_all_ahead(std::vector<Saw*>&,float);

	Base rail;
	float xv;
};

#define ELECTRO_WIDTH 3.75f
#define ELECTRO_HEIGHT 1.0125f
struct Electro:Base{
	Electro(const State&);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Electro*>&);
	static void clear_all_ahead(std::vector<Electro*>&,float);

	float timer_frame;
	bool space;
};

#define SMASHER_WIDTH 4.5f
#define SMASHER_HEIGHT 1.5f
#define SMASHER_RETRACT 3.675f
#define SMASHER_SMASH_TIMER 110.0f
#define SMASHER_RETRACT_TIMER 50.0f
#define SMASHER_SMASH_VELOCITY 0.2f
#define SMASHER_RETRACT_VELOCITY 0.05f
struct Smasher{
	Smasher(const State&);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Smasher*>&);
	static void clear_all_ahead(std::vector<Smasher*>&,float);

	Base left,right;
	bool retracting; // direction
	bool smashed; // currently smashed
	float timer;
};

#define SCENERY_PARALLAX 15.0f
#define SCENERY_BLIMP 1
#define SCENERY_MOON 2
#define SCENERY_STARBABY 3
#define SCENERY_BLIMP_WIDTH 2.75f
#define SCENERY_BLIMP_HEIGHT 1.025f
#define SCENERY_MOON_SIZE 3.5f
#define SCENERY_STARBABY_WIDTH 9.0f
#define SCENERY_STARBABY_HEIGHT 12.8f
struct Scenery:Base{
	Scenery(const State&,int);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Scenery>&);

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
struct Particle:Base{
	Particle(const State&,float,float,bool);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Particle*>&);

	float xv,yv;
	float ttl;
};

#define LIGHT_WIDTH 0.3125f
#define LIGHT_HEIGHT 1.6875f
#define LIGHT_START_HEIGHT 700
#define LIGHT_STEADY_HEIGHT 900
#define LIGHT_STOP_HEIGHT 1315
struct Light:Base{
	Light(const State&);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Light>&);

	float yv;
	float rgb[3];

private:
	void choose_color();
	static int probability(int);
};

struct Backdrop:Base{
	int tid; // texture id
};
