
#ifndef PACKED_BYTE_ARRAY_2D_H
#define PACKED_BYTE_ARRAY_2D_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/bit_map.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

class PackedByteArray2D : public RefCounted
{
	GDCLASS(PackedByteArray2D, RefCounted);

public:
protected:
	Vector2i size;
	PackedByteArray array;

public:

	void resize(Vector2i new_size);
	bool in_bounds(Vector2i pos);
	bool try_setv(Vector2i pos, int value);
	uint8_t try_getv(Vector2i pos);
	void setv(Vector2i pos, int value);
	uint8_t getv(Vector2i pos);
	
	// callable = func(pos: Vector2i, x: int) -> int
	void apply(Callable callable);
	Ref<PackedByteArray2D> duplicate();
	String to_string();
	Ref<BitMap> to_bitmap(int cutoff = 128);

	static Ref<PackedByteArray2D> from_size(Vector2i size);
	static Ref<PackedByteArray2D> from_image(Ref<Image> image, String channel = "a");
	static void _bind_methods();
};

#endif // PACKED_BYTE_ARRAY_2D_H
