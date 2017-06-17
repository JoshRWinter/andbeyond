struct Saw;
struct Electro;
struct Smasher;
struct Base{
	bool collide(const Base&,float)const;
	bool collide_y(const Base&,float)const;
	int correct(const Base&);
	bool too_close(const std::vector<Saw*>&,const std::vector<Electro*>&,const std::vector<Smasher*>&);

	float x,y,w,h,rot;
	int count,frame; // <frame> of <count> in a sprite sheet
};

#define PLAYER_WIDTH 1.075f
#define PLAYER_HEIGHT 1.225f
struct Player:Base{
	void process(State&);
	void render(const Renderer&)const;

	float xv,yv;
	float apex; // highest point in the jump
	bool dead;
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
#define SCENERY_BLIMP_WIDTH 2.75f
#define SCENERY_BLIMP_HEIGHT 1.025f
#define SCENERY_MOON_SIZE 3.5f
struct Scenery:Base{
	Scenery(const State&,int);
	static void process(State&);
	static void render(const Renderer&,const std::vector<Scenery*>&);

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

struct Backdrop:Base{
	int tid; // texture id
};
