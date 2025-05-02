
#include "geometry_utils.hpp"

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/geometry2d.hpp>

float GeometryUtils::_sort_custom(Vector2 a, Vector2 b, float angle)
{
	return a.rotated(angle + Math::deg_to_rad(90.0)).x - b.rotated(angle + Math::deg_to_rad(90.0)).x;
}

Array GeometryUtils::cut_polygon_along_parallel_lines(PackedVector2Array polygon, Array positions, double angle, double scale)
{
	// Sort positions by order
	Ref<GeometryUtils> utils;
	utils.instantiate();
	positions.sort_custom(Callable(utils.ptr(), "_sort_custom").bind(angle));
	// positions.sort_custom([&](Vector2 a, Vector2 b)
	// {
	// 	return a.rotated(angle + Math::deg_to_rad(90.0)).x - b.rotated(angle + Math::deg_to_rad(90.0)).x;
	// });
	Array polygons;
	Array left_polygons;
	left_polygons.push_back(polygon);
	for (int i = 0; i < positions.size(); i++)
	{
		Vector2 position = positions[i];
		Array new_left_polygons = Array();
		Array new_right_polgyons = Array();
		for (int j = 0; j < left_polygons.size(); j++)
		{
			PackedVector2Array left_polygon = static_cast<PackedVector2Array>(left_polygons.get(j));
			Array res = cut_polygon_along_line(left_polygon, position, angle, scale);
			new_right_polgyons.append_array(res[0]);
			// Add the right polygons to the running total
			new_left_polygons.append_array(res[1]);
		}
		// try cutting the left polygons
		polygons.append(new_right_polgyons);
		left_polygons = new_left_polygons;
	}
	// Add the remaining to polygons
	if (left_polygons.size() > 0)
		polygons.append(left_polygons);
	return polygons;
}

// Cuts a polygon along a line, defined by a position and an angle
Array GeometryUtils::cut_polygon_along_line(PackedVector2Array polygon, Vector2 position, double angle, double scale)
{
	Transform2D cut_transform = Transform2D(angle, position);

	// NOTE: Transform2D applies rotation first, then translation
	// 		Therefore, the packedVector2Array we pass in must be relative to the origin
	PackedVector2Array cut_right = transform_polygon(cut_transform, PackedVector2Array{
																																			Vector2(-scale, 0),
																																			Vector2(scale, 0),
																																			Vector2(scale, -scale),
																																			Vector2(-scale, -scale),
																																	});
	PackedVector2Array cut_left = transform_polygon(cut_transform, PackedVector2Array{
																																		 Vector2(-scale, 0),
																																		 Vector2(scale, 0),
																																		 Vector2(scale, scale),
																																		 Vector2(-scale, scale),
																																 });
	Array res = Array();
	res.append(Geometry2D::get_singleton()->clip_polygons(polygon, cut_right));
	res.append(Geometry2D::get_singleton()->clip_polygons(polygon, cut_left));
	return res;
}

// Cuts a polygon along a line, defined by a position and a direction
Array GeometryUtils::cut_polygon_along_line_dir(PackedVector2Array polygon, Vector2 position, Vector2 direction, double scale)
{
	return cut_polygon_along_line(polygon, position, direction.angle(), scale);
}

PackedVector2Array GeometryUtils::transform_polygon(Transform2D transform, PackedVector2Array polygon)
{
	Array new_points = Array();
	for (auto point : polygon)
	{
		new_points.append(transform.xform(point));
	}
	return PackedVector2Array(new_points);
}

void GeometryUtils::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_sort_custom", "a", "b", "angle"), &GeometryUtils::_sort_custom);

	ClassDB::bind_static_method("GeometryUtils", D_METHOD("cut_polygon_along_parallel_lines", "polygon", "positions", "angle", "scale"), &GeometryUtils::cut_polygon_along_parallel_lines, DEFVAL(100000));
	ClassDB::bind_static_method("GeometryUtils", D_METHOD("cut_polygon_along_line", "polygon", "position", "angle", "scale"), &GeometryUtils::cut_polygon_along_line, DEFVAL(100000));
	ClassDB::bind_static_method("GeometryUtils", D_METHOD("cut_polygon_along_line_dir", "polygon", "position", "direction", "scale"), &GeometryUtils::cut_polygon_along_line_dir, DEFVAL(100000));
	ClassDB::bind_static_method("GeometryUtils", D_METHOD("transform_polygon", "transform", "polygon"), &GeometryUtils::transform_polygon);
}
