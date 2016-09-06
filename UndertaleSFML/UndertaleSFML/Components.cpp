#include "Components.h"
#include "UndertaleLoader.h"
#include <SFML/OpenGL.hpp>
#include <map>

using namespace sf;


PlayerOverWorldSystem::PlayerOverWorldSystem(ex::EntityX& app,sf::RenderTarget &target) : target(target), app(app) {
	//_player.load_resources(app);
}
void PlayerOverWorldSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt)  {
	// we update the player
	static constexpr float movement_amount = 30.0f;
	if (_player.isMoving()) {

		sf::Vector2f movement;
		bool turned = false;
		if (_player.isMoving(Player::PlayerFacing::LEFT)) {
			//if (self.xprevious == self.x + 3) self.x -= 2; else self.x -= 3;
			movement.x -= movement_amount;
			turned = true;
			if (_player.isMoving(Player::PlayerFacing::UP) && _player._facing == Player::PlayerFacing::UP) turned = false;
			if (_player.isMoving(Player::PlayerFacing::DOWN) && _player._facing == Player::PlayerFacing::DOWN) turned = false;
			if (turned) _player._facing = Player::PlayerFacing::LEFT;
		}
		if (_player.isMoving(Player::PlayerFacing::UP)) {
			movement.y -= movement_amount;
			turned = true;
			if (_player.isMoving(Player::PlayerFacing::RIGHT) && _player._facing == Player::PlayerFacing::RIGHT) turned = false;
			if (_player.isMoving(Player::PlayerFacing::LEFT) && _player._facing == Player::PlayerFacing::LEFT) turned = false;
			if (turned) _player._facing = Player::PlayerFacing::UP;
		}
		if (_player.isMoving(Player::PlayerFacing::RIGHT) && !_player.isMoving(Player::PlayerFacing::LEFT)) {
			movement.x += movement_amount;
			turned = true;
			if (_player.isMoving(Player::PlayerFacing::UP) && _player._facing == Player::PlayerFacing::UP) turned = false;
			if (_player.isMoving(Player::PlayerFacing::DOWN) && _player._facing == Player::PlayerFacing::DOWN) turned = false;
			if (turned) _player._facing = Player::PlayerFacing::RIGHT;
		}
		if (_player.isMoving(Player::PlayerFacing::DOWN) && !_player.isMoving(Player::PlayerFacing::UP)) {
			movement.y += movement_amount;	
			turned = true;
			if (_player.isMoving(Player::PlayerFacing::RIGHT) && _player._facing == Player::PlayerFacing::RIGHT) turned = false;
			if (_player.isMoving(Player::PlayerFacing::LEFT) && _player._facing == Player::PlayerFacing::LEFT) turned = false;
			if (turned) _player._facing = Player::PlayerFacing::DOWN;
		}
		_player._frameTime += dt;
		if (_player._frameTime > 0.2f) {
			_player._frameTime = 0.0f;
			_player._sprites[(int)_player._facing].next_frame();
		}
		_player._enity.component<Body>()->move(movement*dt);
	} else {
		_player._sprites[(char)_player._facing].image_index(0);
		_player._frameTime = 0;
	}
}

bool Player::load_resources(ex::EntityX& app) {
	

	_sprites[0].sprite_index(1043); 
	_sprites[1].sprite_index(1045); 
	_sprites[2].sprite_index(1044); 
	_sprites[3].sprite_index(1046);
	_facing = PlayerFacing::DOWN;
	// assign THEN create the enity
	_ismoving = false;
	
	_enity = app.entities.create();
	_enity.assign<Body>();
//	_enity.component<Body>()->setScale(1.5f);
	_enity.assign<RenderableCache>(*this);
	_enity.assign<Velocity>();

	_enity.component<Body>()->setPosition(50, 50);
	_directionDown[0] = _directionDown[1] = _directionDown[2] = _directionDown[3] = false;
	//app.events.subscribe<SystemEvent>(*this);
	return true;
}
Player::~Player() {
	if (_enity.valid()) {
		_enity.destroy();
	}
}
void Player::receive(const sf::Event &event) {
	switch (event.type) {
	case sf::Event::EventType::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Key::A:
			_directionDown[(int)PlayerFacing::LEFT] = true;
			break;
		case sf::Keyboard::Key::D:
			_directionDown[(int)PlayerFacing::RIGHT] = true;
			break;
		case sf::Keyboard::Key::W:
			_directionDown[(int)PlayerFacing::UP] = true;
			break;
		case sf::Keyboard::Key::S:
			_directionDown[(int)PlayerFacing::DOWN] = true;
			break;
		}
		break;
	case sf::Event::EventType::KeyReleased:
		switch (event.key.code) {
		case sf::Keyboard::Key::A:
			_directionDown[(int)PlayerFacing::LEFT] = false;
			break;
		case sf::Keyboard::Key::D:
			_directionDown[(int)PlayerFacing::RIGHT] = false;
			break;
		case sf::Keyboard::Key::W:
			_directionDown[(int)PlayerFacing::UP] = false;
			break;
		case sf::Keyboard::Key::S:
			_directionDown[(int)PlayerFacing::DOWN] = false;
			break;
		}
		break;
	}

}



bool Animation::update(Renderable& renderable, float dt) {
//	if (_watch.test_then_reset(dt)) 
//		return _reverse ? renderable.prev_frame() : renderable.next_frame();
	return true; // we havn't gotten to the next frame yet so just assume true
}
/*as a fall back to line()*/
void line_raw(float x1, float y1, float x2, float y2,
	float w,
	float Cr, float Cg, float Cb,
	float, float, float, bool)
{
	glLineWidth((float)w);
	float line_vertex[] =
	{
		x1,y1,
		x2,y2
	};
	float line_color[] =
	{
		Cr,Cg,Cb,
		Cr,Cg,Cb
	};
	// Bind no texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glVertexPointer(2, GL_FLOAT, 0, line_vertex);
	glColorPointer(3, GL_FLOAT, 0, line_color);
	glDrawArrays(GL_LINES, 0, 2);
}

void draw_box_old(sf::VertexArray& verts, const sf::FloatRect& rect, float thickness = 4.0f, const sf::Color color = sf::Color::Green) {
	float left = rect.left;
	float top = rect.top;
	float right = rect.left + rect.width;
	float bottom = rect.top + rect.height;
	// Add a quad for the current character
	verts.append(Vertex(Vector2f(left, top), color)); verts.append(Vertex(Vector2f(left, bottom), color));
	verts.append(Vertex(Vector2f(left, top), color)); verts.append(Vertex(Vector2f(right, top), color));
	verts.append(Vertex(Vector2f(right, bottom), color)); verts.append(Vertex(Vector2f(left, bottom), color));
	verts.append(Vertex(Vector2f(right, bottom), color)); verts.append(Vertex(Vector2f(right, top), color));
}
void draw_box(RawVertices& verts, const sf::FloatRect& rect, float thickness = 4.0f, const sf::Color color = sf::Color::Green) {
	float left = rect.left;
	float top = rect.top;
	float right = rect.left + rect.width;
	float bottom = rect.top + rect.height;
	global::insert_line(verts, sf::Vector2f(left, top), sf::Vector2f(right, top), thickness, color);
	global::insert_line(verts, sf::Vector2f(left, top), sf::Vector2f(left, bottom), thickness, color);
	global::insert_line(verts, sf::Vector2f(right, bottom), sf::Vector2f(left, bottom), thickness, color);
	global::insert_line(verts, sf::Vector2f(right, bottom), sf::Vector2f(right, top), thickness, color);
	// Add a quad for the current character
 
}
void draw_box(const sf::FloatRect& rect,  float thickness = 4.0f, const sf::Color color = sf::Color::Green) {
	sf::VertexArray test(sf::PrimitiveType::TrianglesStrip);
	glLineWidth(thickness);
	sf::VertexArray array(sf::PrimitiveType::Lines);
	float line_vertex[] =
	{
		rect.left,rect.top, rect.left,rect.height-rect.top,
		rect.left,rect.top, rect.width-rect.left, rect.top,
		rect.width-rect.left,rect.height-rect.top,rect.left,rect.height - rect.top,
		rect.width-rect.left,rect.height-rect.top,rect.width-rect.left, rect.top,
	};
	sf::Color line_color[] = { color, color,color, color, color, color,color, color };


}
RawVertices createTest() {
	RawVertices vect;
	global::insert_line(vect, sf::Vector2f(100.0f, 100.0f), sf::Vector2f(200.0f, 200.0f), 10.0f, sf::Color::White);
	global::insert_hair_line(vect, sf::Vector2f(200.0f, 150.0f), sf::Vector2f(50.0f, 50.0f), 10.0f, sf::Color::Green);
	return vect;
}
RawVertices test;
RenderSystem::RenderSystem(sf::RenderTarget &target) : target(target), debug_lines(sf::PrimitiveType::Lines) {
	if (!_font.loadFromFile("LiberationSans-Regular.ttf")) {
		std::cerr << "error: failed to load LiberationSans-Regular.ttf" << std::endl;
		exit(1);
	}
	text.setFont(_font);
	text.setPosition(sf::Vector2f(2, 2));
	text.setCharacterSize(18);
	text.setColor(sf::Color::White);
	test = createTest();
	_transform.scale(2.0f, 2.0f);
}
std::multimap<int, std::reference_wrapper<const sf::Drawable>> sorting;
void RenderSystem::LoadRoom(size_t index) {
	for (auto& e : _roomObjects) e.destroy();
	_roomObjects.clear();
	_room.reset();
	_room = std::move(UndertaleRoom::LoadRoom(index));
	if (_room) {
		ex::EntityManager &es = global::getEntities();
		for (auto& o : _room->objects()) {
			ex::Entity e = es.create();
			e.assign<UndertaleObject>(o.obj);
			auto body = e.assign<Body>(o.body);
			if (o.obj.sprite_index() >= 0) {
				auto handle = e.assign<UndertaleSprite>(o.obj.sprite_index());
				e.assign<RenderableCache>(*handle.get());
				_roomObjects.push_back(e);
			}
		}
	}
}
void RenderSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	RawVertices temp_verts;
	sortedVerts.clear();
	debug_lines.clear();
	size_t draw_count = 0;
	{
		if (_room) {
			if (_room->backgrounds().size() > 0) {
				for (auto& t : _room->backgrounds()) {
					if (t.forground) continue;
					int layer = t.depth;
					auto& verts = (sortedVerts[layer])[t.frame.texture()];
					verts.append(t.frame.ptr(), t.frame.ptr() + t.frame.size());
				}
				if (_room->tiles().size() > 0) {
					for (auto& t : _room->tiles()) {
						auto& verts = (sortedVerts[0])[t.first];
						const auto& f = t.second;
						verts += f.verts();
					}
				}
			}
		}
	}
		

	es.each<Body, RenderableCache>([this,&draw_count](ex::Entity entity, Body& body, RenderableCache &lrenderable) {
		constexpr bool draw_all_boxes = true;
		int layer = entity.has_component<Layer>() ? entity.component<Layer>() : 0;
		auto& verts = (sortedVerts[layer])[lrenderable.texture()];
		lrenderable.update_cache(body);
	
		verts += lrenderable.cache().transform_copy(body.getTransform());
	//	if (draw_all_boxes) {
	//		draw_box(temp_verts, transform.transformRect(renderable.bounds()));
	//	}
	});
	{
		if (_room && _room->backgrounds().size() > 0) {
			for (auto& t : _room->backgrounds()) {
				if (!t.forground) continue;
				int layer = t.depth;
				auto& verts = (sortedVerts[layer])[t.frame.texture()];
				verts.append(t.frame.ptr(), t.frame.ptr() + t.frame.size());
			}
		}
	}
}

void AnimationSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	
	es.each<RenderableRef, Animation>([this,dt](ex::Entity entity, RenderableRef &renderable_ref, Animation &animation) {
		//if (!animation.update(renderable_ref, dt)) entity.remove<Animation>();
		animation.update(renderable_ref, dt);
	});
}

std::unordered_map<size_t, size_t> debug_handles;
static size_t lineindex = 0;
size_t findDebugLine(size_t value) {
	auto it = debug_handles.find(value);
	if (it == debug_handles.end()) {
		it = debug_handles.emplace(value, lineindex).first;
		it->second = lineindex++;
	}
	return it->second;
}
void VelocitySystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	console::output_context context; // save the terminal information
	es.each<Body, Velocity>([this, dt](ex::Entity entity, Body& body, Velocity &velocity) {
		const sf::Vector2f& v = velocity.velocity;
		if (umath::compare(v.x, 0.0f) && umath::compare(v.y, 0.0f)) return;
		sf::Vector2f pos = body.getPosition();
		pos += velocity.velocity * dt;
		body.setPosition(pos); 
		auto id = (size_t)entity.id().id();
	//	std::cout << con::gotoy(findDebugLine(id)) << "\rID " << id << " Velocity " << (velocity.velocity * dt) << "Current Position " << pos << std::endl;
	});
	//console::redraw();
}

Application::Application(sf::RenderWindow &window) : _window(window), draw_count(0), frame_count(0) {
	_transform.scale(1.5f, 1.5f);
	systems.add<PlayerOverWorldSystem>(*this, _window);
	systems.add<VelocitySystem>(_window);
	systems.add<AnimationSystem>(_window);
//	systems.add<RenderSystem>(_window);
//	render_system = systems.system<RenderSystem>().get();
	systems.configure();

	if (!_font.loadFromFile("LiberationSans-Regular.ttf")) {
		std::cerr << "error: failed to load LiberationSans-Regular.ttf" << std::endl;
		exit(1);
	}
	_text.setFont(_font);
	_text.setPosition(sf::Vector2f(2, 2));
	_text.setCharacterSize(18);
	_text.setColor(sf::Color::White);

}
void Application::init(ex::EntityX& app) {
	systems.system<PlayerOverWorldSystem>()->init(app);
	
}