#pragma once
#include "Global.h"
#include "Drawables.h"
#include "UndertaleLoader.h"

struct SpriteAnimation {
	explicit SpriteAnimation(float speed) : _speed(umath::abs(speed)), _current(0.0f), _start_frame(0), _stop_frame(std::numeric_limits<size_t>::max()), _current_frame(0), _forward(speed >= 0.0f), _running(false) {}
	explicit SpriteAnimation(float speed, size_t frame_count) : _speed(umath::abs(speed)), _current(0.0f), _start_frame(0), _stop_frame(frame_count-1), _current_frame(0), _forward(speed >= 0.0f), _running(false) {}
	explicit SpriteAnimation(float speed, size_t start_frame, size_t stop_frame) : _speed(umath::abs(speed)), _current(0.0f), _start_frame(start_frame), _stop_frame(stop_frame), _current_frame(0), _forward(speed >= 0.0f), _running(false) {}
	void reset() { _current = 0.0f;  _current_frame = _start_frame;  }
	void stop() { _running = false; }
	void start() { _running = true; }
	float speed() const { return _speed; }
	void speed(float s) {  _speed=s; }
	bool is_running() const { return _running; }
	size_t start_frame() const { return _start_frame; }
	size_t stop_frame() const { return _stop_frame; }
	size_t current_frame() const { return _current_frame; }
	void start_frame(size_t frame) { _start_frame = frame; _current_frame = _start_frame; }
	void stop_frame(size_t frame)  { _stop_frame=frame; _current_frame = _start_frame; }
	void current_frame(size_t frame) {
		if (frame > _stop_frame) 
			_current_frame = _start_frame;
		else if (frame < _start_frame) 
			_current_frame = _stop_frame;
		else _current_frame = frame;
	}
	void next_frame() { 
		if (_forward) {
			if (_current_frame == _stop_frame || _current_frame == std::numeric_limits<size_t>::max())
				_current_frame = _start_frame;
			else
				_current_frame++;
		} else {
			if (_current_frame == 0 || _current_frame == _start_frame)
				_current_frame = _stop_frame;
			else
				_current_frame--;
		}
	}
	bool forward() const { return _forward; }
	void forward(bool direction) { _forward = direction; }
	bool update(float dt) {
		if (_running) {
			_current += dt;
			if (_current > _speed) {
				_current = 0.0f;
				std::cerr << "From: " << _current_frame;
				next_frame();
				std::cerr << " To: " << _current_frame << std::endl;
				return true;
			}
		}
		return false; // no update
	}
private:
	size_t _start_frame, _stop_frame, _current_frame;
	float _current;
	bool _running;
	bool _forward;
	float _speed;
};

struct Bounds {
	explicit Bounds(sf::FloatRect bounds) :bounds(bounds) {}
	sf::FloatRect bounds;
};


struct Velocity {
	explicit Velocity(sf::Vector2f velocity) :velocity(velocity) {}
	explicit Velocity(float x, float y) :velocity(x, y) {}
	explicit Velocity() :velocity(0.0f,0.0f) {}
	sf::Vector2f velocity;
};
/*

struct UndertaleObject {

};
*/
struct Layer {
	explicit Layer(int layer) :layer(layer) {}
	int layer;
};




struct SystemEvent : public ex::Event<SystemEvent> {
	sf::Event event;
	explicit SystemEvent() : event() {}
	explicit SystemEvent(const sf::Event& event) : event(event) {}
};
/*

class Animation {
	StopWatch<float> _watch;
	bool _reverse;
public:
	explicit Animation(float fps, bool reverse) : _watch(std::fabs(fps)), _reverse(reverse) {}
	explicit Animation(float fps) : _watch(std::fabs(fps)), _reverse(fps > 0.0f ? false : true) {}
	bool update(Renderable& renderable, float dt);
};
*/
template<class T, typename EQUAL = std::equal_to<T>>
class ValueMonitor {
	T _value;
	mutable   bool _changed;
public:
	explicit ValueMonitor() : _value(T{}), _changed(true) {}
	explicit ValueMonitor(T value) : _value(value), _changed(true) {}
	ValueMonitor& operator=(const T& value) { if (!EQUAL(_value, value)) { _changed = true; _value = value; } return *this; }
	operator T&() { return _value; }
	operator const T&() const { return _value; }
	bool changed() const { return _changed; }
	void reset() const { _changed = false; }
};

enum class  Direction : char {
	Down = 0, Right = 1, Up = 2, Left = 3
};

struct PlayerControl {
	static std::unordered_set<sf::Keyboard::Key> keys_down;
	static bool DirectionDown[4];
	inline static bool isKeyDown(sf::Keyboard::Key key) { return keys_down.find(key) != keys_down.end(); }
	inline static bool isMoving() {
		return DirectionDown[0] || DirectionDown[1] || DirectionDown[2] || DirectionDown[3];
	}
	inline static bool isMoving(Direction direction)  { return DirectionDown[(char)direction]; }
	static void update_keys(const sf::Event &event);
	float moving_speed;
	Direction facing;
	explicit PlayerControl(float speed) : moving_speed(speed), facing(Direction::Down) { }
	sf::Vector2f getMovement();
};
struct SpriteFacing {
	std::array<UndertaleSprite, 4> facing_sprites;
	Direction direction = Direction::Down;
	explicit SpriteFacing() {}
	explicit SpriteFacing(int down, int right, int up, int left) :
	//explicit SpriteFacing(size_t down, size_t right, size_t up, size_t left) :
		facing_sprites{ UndertaleSprite(down), UndertaleSprite(right), UndertaleSprite(up), UndertaleSprite(left) } {}
	UndertaleSprite getCurrentFace(Direction direction) { return facing_sprites[(char)direction]; }
};



struct UndertaleObject {
	UndertaleLib::Object obj;
	std::set<UndertaleLib::Object> parents;
	UndertaleObject() : obj() {}
	UndertaleObject(UndertaleLib::Object obj) : obj(obj) {
		UndertaleLib::Object parent = Global::LookupObject(obj.parent_index());
		while (parent.valid()) {
			parents.emplace(parent);
			parent = Global::LookupObject(parent.parent_index());
		}
	}
};

namespace kc {
	using bounds =kult::component<'bond', sf::FloatRect> ;
	using body =kult::component<'body', Body> ;
	using velocity =kult::component<'velc', sf::Vector2f> ;
	using depth =kult::component<'dept', ValueMonitor<int>> ;
	using sprite =kult::component<'sprt', UndertaleSprite> ;
	using object =kult::component<'uobj', UndertaleObject> ;
	using player_control =kult::component<'pctl', PlayerControl> ;
	using sprite_facing =kult::component<'sfac', SpriteFacing> ;
	using sprite_animation =kult::component<'sanm', SpriteAnimation> ;
	
	
};


class Application  : public ex::EntityX {
	sf::Clock _clock;
	sf::Clock _debugUpdate;
	sf::RenderWindow& _window;
	sf::Time _lastTime;
	size_t frame_count;
	size_t draw_count;
	size_t update_count;
	sf::Font _font;
	sf::Text _text;
	sf::Transform _transform;
	typedef std::unordered_map<const sf::Texture*, RawVertices> t_dumb_batch;
	typedef std::vector<sf::FloatRect> t_debug_boxes;
	std::map<int, t_dumb_batch> sortedVerts;
	struct StaticFrames {
		SpriteFrame frame;
		bool visiable;
		int depth;
		sf::Vector2f speed;
		explicit StaticFrames(SpriteFrame frame, bool visiable, int depth, sf::Vector2f speed) :
			frame(frame), visiable(visiable), depth(depth), speed(speed) {}
	};
	std::vector<StaticFrames> _backgrounds;
	std::vector<StaticFrames> _foregrounds;
	std::unordered_multimap<size_t, ex::Entity> _roomObjects;
	std::vector<ex::Entity> _static_entitys;
	std::vector<ex::Entity> _dynamic_enitys;
	std::vector<ex::Entity> _roomEntitys;
	UndertaleRoom::type _room;
	RawVertices temp_verts; // used in the rendering system
public:
	explicit Application(sf::RenderWindow &window);
	std::set<ex::Entity> findOjbects(size_t index);
	ex::Entity findSingleObject(size_t index);
	void LoadRoom(size_t index);
	size_t getRoomIndex() const { return _room ? _room->index() : 0; }
	void init(ex::EntityX& app); 
	void draw();
	void update_verts(ex::TimeDelta dt);
	static constexpr size_t room_fps = (1000/ 60);
	void update(ex::TimeDelta dt);
};

