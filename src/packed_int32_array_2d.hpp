
#ifndef PACKED_INT32_ARRAY_2D_H
#define PACKED_INT32_ARRAY_2D_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/bit_map.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

class PackedInt32Array2D : public RefCounted
{
	GDCLASS(PackedInt32Array2D, RefCounted);

public:
protected:
	Vector2i size;
	PackedInt32Array array;

public:

	void resize(Vector2i new_size);
	bool in_bounds(Vector2i pos);
	bool try_setv(Vector2i pos, int value);
	int32_t try_getv(Vector2i pos);
	void setv(Vector2i pos, int value);
	int32_t getv(Vector2i pos);
	
	// callable = func(pos: Vector2i, x: int) -> int
	void apply(Callable callable);
	Ref<PackedInt32Array2D> duplicate();
	String to_string();
	Ref<BitMap> to_bitmap(int cutoff = 128);

	static Ref<PackedInt32Array2D> from_size(Vector2i size);
	static Ref<PackedInt32Array2D> from_image(Ref<Image> image, String channel = "a");
	static void _bind_methods();
};

#endif // PACKED_INT32_ARRAY_2D_H
