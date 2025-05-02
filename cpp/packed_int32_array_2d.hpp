
#ifndef PACKED_INT32_ARRAY_2D_H
#define PACKED_INT32_ARRAY_2D_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/bit_map.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/packed_byte_array2d.hpp>
#include <godot_cpp/classes/packed_int32_array2d.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

class PackedInt32Array2D : public RefCounted {
	GDCLASS(PackedInt32Array2D, RefCounted);
public:

protected:
	Vector2i size;
	PackedInt32Array array;

public:
	void _init(Vector2i size) override;

	void resize(Vector2i new_size);

	bool in_bounds(Vector2i pos);
// Returns null if value does not exist
	bool try_setv(Vector2i pos, int value);

	void try_getv(Vector2i pos);

	void setv(Vector2i pos, int value);

// callable = func(pos: Vector2i, x: int) -> int 
	void getv(Vector2i pos);

	void apply(Callable callable);

	Ref<PackedByteArray2D> duplicate();

	String _to_string() override;

	Ref<BitMap> to_bitmap(int cutoff = 128);

	static Ref<PackedInt32Array2D> from_image(Ref<Image> image, String channel = "a");

	static void _bind_methods();
};

#endif // PACKED_INT32_ARRAY_2D_H
