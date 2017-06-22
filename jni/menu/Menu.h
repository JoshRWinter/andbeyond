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
	Base background,entry_skyline,entry;
	Player player;
	Button play,settings,quit,about;
	const State *local;
	float yoffset;
	float slide;
};

#define FULL_WHITE_TARGET_ALPHA 0.8f
#define FULL_WHITE_TRANSITION_SPEED 0.01f
#define ATTACHMENT_Y_TARGET 6.15f
#define ATTACHMENT_Y_SPEED 0.03f
#define TRANSITION_OUT_TARGET -22.5f
class MenuGameover:public Menu{
public:
	bool exec(State&);
	virtual void render(const Renderer&)const;
private:
	State *local;
	Base background,full_white;
	Base attachment;
	Button again,menu;
	Base entry;
	float full_white_alpha;
	float yoffset;
	float yoffset_target;
	float attachment_target;
};
