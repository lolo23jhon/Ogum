#ifndef ENGINE_HPP
#define ENGINE_HPP


class Engine
{
public:
	int m_fovRadius;
	int m_screen_w;
	int m_screen_h;
	int m_level;
	std::vector<std::unique_ptr<Actor>> m_actors;
	std::unique_ptr<Map> m_map;
	std::vector<std::unique_ptr<TCODConsole>> m_consoles;
	std::unique_ptr<Gui> m_gui;
	Actor* m_player;			// Non-owning ptr
	Actor* m_stairs;			// Non-ownning ptr
	TCOD_key_t m_last_key;
	TCOD_mouse_t m_mouse;

	static Engine* s_engine;
	static TCODRandom* s_rng;	// Non-owning ptr

	enum class GAME_STATUS
	{
		STARTUP,  // First frame
		IDLE,     // No new turn, redraw same screen
		NEW_TURN, // Update some position
		VICTORY,  // The player won
		DEFEAT    // The player was killed
	} m_game_status;

	Engine(const int t_screen_w, const int t_srceen_h);
	~Engine();
	void render();
	void update();
	void init();
	void term();
	void load(const bool t_pause = false);
	void save();
	void sendToBack(Actor* t_actor);
	void nextLevel();
	Actor* getClosestMonster(const int t_x, const int t_y, const float t_range) const;
	Actor* getActor(const int t_x, const int t_y) const;
	bool pickATile(int* t_x, int* t_y, const float t_max_range = 0.0f);

	template< typename ...Args>
	static void message(const TCODColor& t_color, const char* t_msg, const Args...t_args) {
		s_engine->m_gui->message(t_color, t_msg, t_args);
	}

	template< typename ...Args>
	static void messageFirstUpper(const TCODColor& t_color, const char* t_msg, const Args...t_args) {
		std::string msg{t_msg};
		msg[0] = toupper(msg[0]);
		m_gui->message(t_color,msg.c_str(),t_args);
	}

	static void spawActorPreset(const int t_x, const int t_y, const ActorPreset t_actor_preset);
};

#endif

