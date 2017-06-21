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
