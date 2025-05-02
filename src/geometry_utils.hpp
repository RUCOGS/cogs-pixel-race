
#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

// Cuts a polygon along a set of parallel lines
class GeometryUtils : public RefCounted
{
	GDCLASS(GeometryUtils, RefCounted);

public:
	float _sort_custom(Vector2 a, Vector2 b, float angle);
	// Returns the right and left halves
	static Array cut_polygon_along_parallel_lines(PackedVector2Array polygon, Array positions, double angle, double scale = 100000);
	// Returns the right and left halves
	static Array cut_polygon_along_line(PackedVector2Array polygon, Vector2 position, double angle, double scale = 100000);
	static Array cut_polygon_along_line_dir(PackedVector2Array polygon, Vector2 position, Vector2 direction, double scale = 100000);
	static PackedVector2Array transform_polygon(Transform2D transform, PackedVector2Array polygon);

	static void _bind_methods();
};

#endif // GEOMETRY_UTILS_H
