
#ifndef MAP_REGION_H
#define MAP_REGION_H

#include "packed_byte_array_2d.hpp"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/bit_map.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/weak_ref.hpp>
#include <godot_cpp/templates/hash_set.hpp>

using namespace godot;

class MapRegion : public RefCounted
{
	GDCLASS(MapRegion, RefCounted);

protected:
	Ref<PackedByteArray2D> data;
	HashSet<Vector2i> points;
	Ref<WeakRef> _parent;
	Array children;
	int fill_value = 1;
	bool cache_ready = false;
	PackedVector2Array cached_polygon;
	Vector2 cached_center;

public:
	Ref<MapRegion> get_parent();
	void set_parent(Ref<MapRegion> value);

	Array get_children();
	void set_children(Array value);

	int get_fill_value();
	void set_fill_value(int value);

	bool in_bounds(Vector2i pos);
	void setv(Vector2i pos, int value);
	int getv(Vector2i pos);
	Vector2i size();
	void MapRegion::apply_callable(Callable callable);
	template <typename F>
	void apply(F func)
	{
		for (int y = 0; y < size().y; y += 1)
		{
			for (int x = 0; x < size().x; x += 1)
			{
				Vector2i pos = Vector2i(x, y);
				setv(pos, func(pos, getv(pos)));
			}
		}
	}
	Ref<BitMap> get_bitmap();
	Vector2 get_center();
	PackedVector2Array recache_polygon(double epsilon = 1.0);
	void recache_polygon_tree(double epsilon = 1.0);
	PackedVector2Array get_polygon(double epsilon = 1.0);
	Array get_polygons(double epsilon = 1.0);
	Ref<MapRegion> get_negated();
	Ref<MapRegion> get_inside();
	Ref<MapRegion> get_outside();
	bool is_empty();
	String _to_string();
	String _to_string_tree(int depth = 0);
	Ref<MapRegion> duplicate();

	static Vector2i OFFSETS[4];

	static Ref<MapRegion> from_size(Vector2i size);
	static Ref<MapRegion> from_data(Ref<PackedByteArray2D> data);
	static Ref<MapRegion> from_points(PackedVector2Array points, Vector2i size);
	static Ref<MapRegion> build_region_tree(Ref<PackedByteArray2D> data);
	static Array find_regions(Ref<PackedByteArray2D> data);
	static Ref<MapRegion> find_region(Vector2i start, Ref<PackedByteArray2D> data, Ref<PackedByteArray2D> visited, int fill_value = 1);
	static void visit_region(Vector2i start, Ref<PackedByteArray2D> data, Ref<PackedByteArray2D> visited, int fill_value = 1);

	static void _bind_methods();
};

#endif // MAP_REGION_H
