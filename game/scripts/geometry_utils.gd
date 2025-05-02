class_name GeometryUtils
extends RefCounted

# Cuts a polygon along a set of parallel lines
static func cut_polygon_along_parallel_lines(polygon: PackedVector2Array, positions: Array[Vector2], angle: float, scale: float = 100_000) -> Array[Array]:
	# Sort positions by order
	positions.sort_custom(func(a, b): return a.rotated(angle + deg_to_rad(90)).x - b.rotated(angle + deg_to_rad(90)).x)
	var polygons: Array[Array] = []
	var left_polygons: Array[PackedVector2Array] = [polygon]
	for position in positions:
		var new_left_polygons: Array[PackedVector2Array] = []
		var new_right_polgyons: Array[PackedVector2Array] = []
		for left_polygon in left_polygons:
			var res = cut_polygon_along_line(left_polygon, position, angle, scale)
			new_right_polgyons.append_array(res[0]) # Add the right polygons to the running total
			new_left_polygons.append_array(res[1]) # try cutting the left polygons
		polygons.append(new_right_polgyons)
		left_polygons = new_left_polygons
	if left_polygons.size() > 0:
		# Add the remaining to polygons
		polygons.append(left_polygons)
	return polygons


# Cuts a polygon along a line, defined by a position and an angle
# Returns the right and left halves
static func cut_polygon_along_line(polygon: PackedVector2Array, position: Vector2, angle: float, scale: float = 100_000) -> Array[Array]:
	var cut_transform = Transform2D(angle, position)
	# NOTE: Transform2D applies rotation first, then translation
	# 		Therefore, the packedVector2Array we pass in must be relative to the origin
	var cut_right = transform_polygon(cut_transform, PackedVector2Array([
		Vector2(-scale, 0),
		Vector2(scale, 0),
		Vector2(scale, -scale),
		Vector2(-scale, -scale)
	]))
	var cut_left = transform_polygon(cut_transform, PackedVector2Array([
		Vector2(-scale, 0),
		Vector2(scale, 0),
		Vector2(scale, scale),
		Vector2(-scale, scale)
	]))
	var res: Array[Array] = []
	res.append(Geometry2D.clip_polygons(polygon, cut_right))
	res.append(Geometry2D.clip_polygons(polygon, cut_left))
	return res


# Cuts a polygon along a line, defined by a position and a direction
# Returns the right and left halves
static func cut_polygon_along_line_dir(polygon: PackedVector2Array, position: Vector2, direction: Vector2, scale: float = 100_000) -> Array[Array]:
	return cut_polygon_along_line(polygon, position, direction.angle(), scale)


static func transform_polygon(transform: Transform2D, polygon: PackedVector2Array) -> PackedVector2Array:
	var new_points = []
	for point in polygon:
		new_points.append(transform * point)
	return PackedVector2Array(new_points)
