#include <exception>

class need_to_exit:public std::exception{
public:
	virtual const char *what()const _GABIXX_NOEXCEPT{
		return "system exit requested";
	}
};

class Menu{
public:
	virtual void render(const Renderer&)const=0;
};

#define MENU_CARD_X (-4.5f+0.75f)
#define MENU_CARD_Y (-8.0f+1.5f)
#define MENU_CARD_W (9.0f-1.5f)
#define MENU_CARD_H (16.0f-3.0f)

#define PLAYER_BOUNCE_Y 0.4f
#define MENU_SLIDE -0.7f
#define MENU_SLIDE_RETARD 0.01498f;
class MenuMain:public Menu{
public:
	void exec(State&);
	virtual void render(const Renderer&)const;
private:
	void show_highscores(State&);

	Base background,entry_skyline,entry;
	Player player;
	Button play,settings,quit,about;
	const State *local;
	float yoffset;
	float slide;
};

#define FULL_WHITE_TARGET_ALPHA 0.8f
#define FULL_WHITE_TRANSITION_SPEED 0.0175f
#define ATTACHMENT_Y_TARGET 6.15f
#define ATTACHMENT_Y_SPEED 0.03f
#define TRANSITION_OUT_TARGET -22.5f
class MenuGameover:public Menu{
public:
	void exec(State&);
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
	const char *header;
	int scoreboard_index;
	const int *scoreboard;
	float scoreboard_x;
	int score;
	bool play;
};

class MenuConfig:public Menu{
public:
	void exec(State &state,const Menu &menu);
	virtual void render(const Renderer &renderer)const;
private:

	const Menu *parent;
	const State *local;
	Base full_white,card;
	Button vibrate,sounds,back;
	float full_white_alpha,full_white_alpha_target;
	float yoffset,yoffset_target;
	bool changed; // config dirty
};

class MenuPause:public Menu{
public:
	void exec(State&);
	virtual void render(const Renderer&)const;
private:
	Base full_white;
	const State *local;
	Button menu,settings,back;
	bool no_fingers;
	float alpha;
};

class MenuMessage:public Menu{
public:
	void exec(State&,const Menu&,const char*,const char* =NULL);
	virtual void render(const Renderer&)const;
private:
	Base background,full_white;
	Button ok;
	const char *text;
	const char *header;
	const Menu *local;
	float full_white_alpha,full_white_alpha_target;
	float yoffset,yoffset_target;
};
