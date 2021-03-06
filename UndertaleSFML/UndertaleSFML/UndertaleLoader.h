#pragma once

#include "Global.h"
#include "Drawables.h"
#include "UndertaleLib.h"

class UFont  : public std::enable_shared_from_this<UFont> {
protected:
	size_t _fontSize;
	const sf::Texture* _texture;
	typedef std::unordered_map<sf::Uint32, sf::Glyph> GlyphTable;
	GlyphTable _glyphTable;
	int _index;
	UFont();
public:
	static std::shared_ptr<UFont> LoadUndertaleFont(size_t font_index);
	int getIndex() const { return _index; }
	const sf::Texture& getTexture() const { return *_texture; }
	size_t getFontSize() const { return _fontSize; }
	const sf::Glyph& getGlyph(sf::Uint32 codePoint) const;
	float getKerning(sf::Uint32 first, sf::Uint32 second) const;
	float getLineSpacing() const;
};



class UndertaleSprite  : public SpriteFrameRef {
public:
	class UndertaleSpriteData : std::enable_shared_from_this<UndertaleSpriteData> {
	protected:
		sf::Vector2f _size;
		size_t _index;
		std::vector<SpriteFrame> _frames;
		UndertaleSpriteData() : _index(0) {}
	public:
		typedef std::shared_ptr<UndertaleSpriteData> type;
		type ptr() { return shared_from_this(); }
		static type LoadSprite(size_t sprite_index);
		inline const SpriteFrame& frame(size_t i) const { return _frames[i%_frames.size()]; }
		inline size_t frame_count() const { return _frames.size(); }
		inline size_t index() const { return _index; }
		inline const sf::Vector2f size() const { return _size; }
	};
	UndertaleSpriteData::type _sprite;
	static std::unordered_map<size_t, UndertaleSpriteData::type> _cache; 
	// we use this so if we change this sprite, we don't lose the old one in case we need to switch fast
	size_t _image_index;
	
public:
	UndertaleSprite() : _image_index(0), _sprite(nullptr) {}
	explicit UndertaleSprite(size_t index) : _image_index(0) {
		_sprite = UndertaleSpriteData::LoadSprite(index);
		assign(_sprite->frame(_image_index = 0));
	} 
	UndertaleSprite(const UndertaleSprite& copy) : _image_index(0), _sprite(copy._sprite), SpriteFrameRef(_sprite->frame(0)) {}
	UndertaleSprite& operator=(UndertaleSprite sprite) {
		using std::swap;
		swap(_sprite, sprite._sprite);
		assign(_sprite->frame(_image_index=0));
		return *this;
	}
	// interface
	// sprite stuff
	bool valid() const { return (bool)_sprite; }
	size_t sprite_index() const { return _sprite ? _sprite->index() : 0; }
	void sprite_index(size_t index);
	// this clears the static cache, but dosn't free up the already exisiting ptrs
	inline static void clear_cache() { _cache.clear(); }
	size_t image_index() const { return _image_index; }
	size_t image_count() const { return _sprite->frame_count(); }
	void image_index(size_t index) {  
		index %= image_count();
		if (index != _image_index) {
			_image_index = index;
			assign(_sprite->frame(_image_index));
		}
	}
};

// font texture algorithm 
// http://gamedev.stackexchange.com/questions/2829/texture-packing-algorithm

class UndertaleRoom : std::enable_shared_from_this<UndertaleRoom> {

public:
	struct RoomObject  {
		UndertaleLib::Object obj;
		Body body;
		int depth;
	};
	struct RoomBackground {
		SpriteFrame frame;
		sf::Vector2f pos;
		sf::Vector2f speed;
		bool strech;
		bool tiled;
		bool forground;
		int depth;
		bool visible;
	};
	typedef std::shared_ptr<UndertaleRoom> type;
	type ptr() { return shared_from_this(); }
	static type LoadRoom(size_t sprite_index);
	
	const std::unordered_map<const sf::Texture*, TileMap> tiles() const { return _tiles; }
	const std::vector<RoomBackground>& backgrounds() const { return _backgrounds; }
	const std::vector<RoomObject>& objects() const { return _objects; }
	size_t index() const { return _index; }
protected:
	std::unordered_map<const sf::Texture*, TileMap> _tiles;
	std::vector<RoomBackground> _backgrounds;
	std::vector<RoomObject> _objects;
	size_t _index;
	UndertaleRoom() : _index(0) {}
};

namespace Global {
	UndertaleLib::Object LookupObject(size_t index);
	SpriteFrame LoadFrame(const UndertaleLib::SpriteFrame& uf);
	SpriteFrame LookupBackground(size_t index);
	SpriteFrame LoadFrame(const UndertaleLib::SpriteFrame& uf);
	bool LoadUndertaleDataWin(const std::string& filename);
	const sf::Texture& GetUndertaleTexture(size_t index);
	sf::Shader& LoadShader(const std::string& filename); // loads a shader, or from a cache
	void DestroyEveything();
};