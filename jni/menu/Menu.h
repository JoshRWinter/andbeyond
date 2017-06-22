class Menu{
public:
	virtual void render(const Renderer&)const=0;
};

#define PLAYER_BOUNCE_Y 0.4f
#define MENU_SLIDE -0.7f
#define MENU_SLIDE_RETARD 0.01498f;
class MenuMain:public Menu{
public:
	bool exec(State&);
	virtual void render(const Renderer&)const;
private:
	void fake_render(const Renderer&)const;

	Base background,entry_skyline,entry;
	Player player;
	Button play,settings,quit,about;
	const State *local;
	float yoffset;
	float slide;
};

#define FULL_WHITE_TARGET_ALPHA 0.8f
#define FULL_WHITE_TRANSITION_SPEED 0.01f
class MenuGameover:public Menu{
public:
	bool exec(State&);
	virtual void render(const Renderer&)const;
private:

	Base background,full_white;
	Button again,menu;
	float full_white_alpha;
	float yoffset;
	float yoffset_target;
};
