
#include "map_region.hpp"

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Ref<MapRegion> MapRegion::get_parent()
{
	if (_parent.is_null())
		return nullptr;
	return _parent.ptr()->get_ref();
}

void MapRegion::set_parent(Ref<MapRegion> value)
{
	_parent = UtilityFunctions::weakref(value.ptr());
}

Array MapRegion::get_children()
{
	return children;
}

void MapRegion::set_children(Array value)
{
	children = value;
}

bool MapRegion::in_bounds(Vector2i pos)
{
	return data->in_bounds(pos);
}

int MapRegion::get_fill_value()
{
	return fill_value;
}

void MapRegion::set_fill_value(int value)
{
	fill_value = value;
}

void MapRegion::setv(Vector2i pos, int value)
{
	data->setv(pos, value);
	if (value != 0)
		points.insert(pos);
	else
		points.erase(pos);
}

int MapRegion::getv(Vector2i pos)
{
	return data->getv(pos);
}

Vector2i MapRegion::size()
{
	return data->get_size();
}

// Applies a function over all points
// callable = func(pos: Vector2i, x: int) -> int
void MapRegion::apply_callable(Callable callable)
{
	for (int y = 0; y < size().y; y += 1)
	{
		for (int x = 0; x < size().x; x += 1)
		{
			Vector2i pos = Vector2i(x, y);
			setv(pos, callable.call(pos, getv(pos)));
		}
	}
}

Ref<BitMap> MapRegion::get_bitmap()
{
	Ref<BitMap> bitmap;
	bitmap.instantiate();
	bitmap->resize(size());
	for (auto pos : points)
		bitmap->set_bitv(pos, true);
	return bitmap;
}

Vector2 MapRegion::get_center()
{
	if (cache_ready)
		return cached_center;
	recache_polygon();
	return cached_center;
}

PackedVector2Array MapRegion::recache_polygon(double epsilon)
{
	cache_ready = true;
	Array polygons = get_polygons(epsilon);
	if (polygons.size() == 0)
		cached_polygon = PackedVector2Array();
	else
		cached_polygon = polygons[0];
	cached_center = Vector2();
	for (Variant pos : cached_polygon)
		cached_center += pos;
	cached_center /= cached_polygon.size();
	return cached_polygon;
}

// Recaches the entire MapRegion tree's polygons
void MapRegion::recache_polygon_tree(double epsilon)
{
	Array stack = Array();
	stack.push_back(this);
	while (!stack.is_empty())
	{
		// Pop from top of stack
		Ref<MapRegion> region = static_cast<Ref<MapRegion>>(stack.pop_back());
		region->recache_polygon(epsilon);
		stack.append_array(region->children);
	}
}

PackedVector2Array MapRegion::get_polygon(double epsilon)
{
	if (cache_ready)
		return cached_polygon;
	return recache_polygon(epsilon);
}

Array MapRegion::get_polygons(double epsilon)
{
	return get_bitmap()->opaque_to_polygons(Rect2i(Vector2i(), size()), epsilon);
}

// Returns copy of current region with points negated
Ref<MapRegion> MapRegion::get_negated()
{
	Ref<MapRegion> copy = MapRegion::from_size(size());
	copy->apply([&](Vector2 pos, int x)
							{ return points.has(pos) ? 0 : 1; });
	return copy;
}

// Returns a region that is the opposite of the outside
// AKA returns the region with its insides filled
Ref<MapRegion> MapRegion::get_inside()
{
	Ref<MapRegion> outside = get_outside();
	return outside->get_negated();
}

// Return a region that encircles the current region from the edges of the data
Ref<MapRegion> MapRegion::get_outside()
{
	Ref<PackedByteArray2D> visited = PackedByteArray2D::from_size(size());
	for (int y = 0; y < size().y; y += 1)
	{

		// Try create regions from left and right edges
		Vector2i pos = Vector2i(0, y);
		if (!points.has(pos))
			visit_region(pos, data, visited, 0);
		pos = Vector2i(size().x - 1, y);
		if (!points.has(pos))
			visit_region(pos, data, visited, 0);
	}
	for (int x = 0; x < size().x; x += 1)
	{

		// Try create regions from top and bottom edges
		Vector2i pos = Vector2i(x, 0);
		if (!points.has(pos))
			visit_region(pos, data, visited, 0);
		pos = Vector2i(x, size().y - 1);
		if (!points.has(pos))
			visit_region(pos, data, visited, 0);
	}

	// Now visited should contain all reachable positions from the edge
	return MapRegion::from_data(visited);
}

bool MapRegion::is_empty()
{
	return points.size() == 0;
}

String MapRegion::_to_string()
{
	return _to_string_tree();
}

String MapRegion::_to_string_tree(int depth)
{
	String str = vformat("MapRegion: fill: %s (%s) {%s}\nChildren:", fill_value, points.size(), data->to_string());
	PackedStringArray lines = str.split("\n");
	for (int i = 0; i < lines.size(); i += 1)
	{
		lines[i] = String("    ").repeat(depth) + lines[i];
	}
	str = String("\n").join(lines) + "\n";
	for (int i = 0; i < children.size(); i++)
		str += static_cast<Ref<MapRegion>>(children.get(i))->_to_string_tree(depth + 1);
	return str;
}

Ref<MapRegion> MapRegion::duplicate()
{
	PackedVector2Array new_points;
	for (auto point : points)
		new_points.append(point);
	return MapRegion::from_points(new_points, size());
}

Ref<MapRegion> MapRegion::from_size(Vector2i size)
{
	return MapRegion::from_points(PackedVector2Array(), size);
}

Ref<MapRegion> MapRegion::from_points(PackedVector2Array points, Vector2i size)
{
	Ref<MapRegion> res;
	res.instantiate();
	res->data = PackedByteArray2D::from_size(size);
	res->points.clear();
	for (auto point : points)
	{
		res->points.insert(point);
		res->data->setv(point, 1);
	}
	return res;
}

Ref<MapRegion> MapRegion::from_data(Ref<PackedByteArray2D> data)
{
	PackedVector2Array points = PackedVector2Array();
	for (int y = 0; y < data->get_size().y; y++)
	{
		for (int x = 0; x < data->get_size().x; x++)
		{
			Vector2i pos = Vector2i(x, y);
			if (data->getv(pos) != 0)
				points.append(pos);
		}
	}
	return MapRegion::from_points(points, data->get_size());
}

Ref<MapRegion> MapRegion::build_region_tree(Ref<PackedByteArray2D> data)
{

	// The root has the entire data array set to 1s (Selected)
	Ref<MapRegion> root = MapRegion::from_size(data->get_size())->get_negated();
	root->fill_value = 0;
	Array stack;
	stack.push_back(root);
	while (!stack.is_empty())
	{
		Ref<MapRegion> region = stack.pop_back();
		Ref<PackedByteArray2D> visited = PackedByteArray2D::from_size(data->get_size());
		int op_fill_value = region->fill_value == 0 ? 1 : 0;

		// Iterate over region, and find next children regions (must have opposite fill value)
		for (auto point : region->points)
		{
			if (visited->getv(point) != 1 && data->getv(point) == op_fill_value)
			{
				// Find new child region
				Ref<MapRegion> child_region = find_region(point, data, visited, op_fill_value)->get_inside();
				child_region->fill_value = op_fill_value;
				for (auto child_point : child_region->points)
				{
					visited->setv(child_point, 1);
				}
				child_region->_parent = UtilityFunctions::weakref(region);
				region->children.append(child_region);
				stack.push_back(child_region);
			}
		}
	}
	return root;
}

Array MapRegion::find_regions(Ref<PackedByteArray2D> data)
{
	Ref<MapRegion> visited = PackedByteArray2D::from_size(data->get_size());
	Array regions;
	int region_id = 0;
	for (int y = 0; y < data->get_size().y; y += 1)
	{
		for (int x = 0; x < data->get_size().x; x += 1)
		{
			Vector2i pos = Vector2i(x, y);
			Ref<MapRegion> region = find_region(pos, data, visited);
			if (!region->is_empty())
				regions.append(region);
		}
	}
	return regions;
}

Vector2i MapRegion::OFFSETS[4] = {Vector2i(0, -1), Vector2i(0, 1), Vector2i(-1, 0), Vector2i(1, 0)};

Ref<MapRegion> MapRegion::find_region(Vector2i start, Ref<PackedByteArray2D> data, Ref<PackedByteArray2D> visited, int fill_value)
{
	Array stack;
	stack.push_back(start);
	HashSet<Vector2i> region_data;
	while (!stack.is_empty())
	{
		Vector2i pos = static_cast<Vector2i>(stack.pop_back());

		// Already visited
		if (!visited->in_bounds(pos) || visited->getv(pos) == 1 || data->getv(pos) != fill_value)
			continue;

		// Not visited, mark as visited
		visited->setv(pos, 1);
		region_data.insert(pos);
		for (Variant offset : MapRegion::OFFSETS)
			stack.push_back(pos + offset);
	}
	PackedVector2Array points;
	for (auto elem : region_data)
		points.append(elem);
	return MapRegion::from_points(points, data->get_size());
}

void MapRegion::visit_region(Vector2i start, Ref<PackedByteArray2D> data, Ref<PackedByteArray2D> visited, int fill_value)
{
	Array stack;
	stack.push_back(start);
	HashSet<Vector2i> region_data;
	while (!stack.is_empty())
	{
		Vector2i pos = static_cast<Vector2i>(stack.pop_back());
		// Already visited
		if (!visited->in_bounds(pos) || visited->getv(pos) == 1 || data->getv(pos) != fill_value)
			continue;

		// Not visited, mark as visited
		visited->setv(pos, 1);
		region_data.insert(pos);
		for (Variant offset : MapRegion::OFFSETS)
			stack.push_back(pos + offset);
	}
}

void MapRegion::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("in_bounds", "pos"), &MapRegion::in_bounds);
	ClassDB::bind_method(D_METHOD("setv", "pos", "value"), &MapRegion::setv);
	ClassDB::bind_method(D_METHOD("getv", "pos"), &MapRegion::getv);
	ClassDB::bind_method(D_METHOD("size"), &MapRegion::size);
	ClassDB::bind_method(D_METHOD("apply", "callable"), &MapRegion::apply_callable);
	ClassDB::bind_method(D_METHOD("get_bitmap"), &MapRegion::get_bitmap);
	ClassDB::bind_method(D_METHOD("get_center"), &MapRegion::get_center);
	ClassDB::bind_method(D_METHOD("recache_polygon", "epsilon"), &MapRegion::recache_polygon, DEFVAL(2.0));
	ClassDB::bind_method(D_METHOD("recache_polygon_tree", "epsilon"), &MapRegion::recache_polygon_tree, DEFVAL(2.0));
	ClassDB::bind_method(D_METHOD("get_polygon", "epsilon"), &MapRegion::get_polygon, DEFVAL(2.0));
	ClassDB::bind_method(D_METHOD("get_polygons", "epsilon"), &MapRegion::get_polygons, DEFVAL(2.0));
	ClassDB::bind_method(D_METHOD("get_negated"), &MapRegion::get_negated);
	ClassDB::bind_method(D_METHOD("get_inside"), &MapRegion::get_inside);
	ClassDB::bind_method(D_METHOD("get_outside"), &MapRegion::get_outside);
	ClassDB::bind_method(D_METHOD("is_empty"), &MapRegion::is_empty);
	ClassDB::bind_method(D_METHOD("duplicate"), &MapRegion::duplicate);

	ClassDB::bind_method(D_METHOD("get_parent"), &MapRegion::get_parent);
	ClassDB::bind_method(D_METHOD("set_parent", "value"), &MapRegion::set_parent);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parent"), "set_parent", "get_parent");

	ClassDB::bind_method(D_METHOD("set_children", "value"), &MapRegion::set_children);
	ClassDB::bind_method(D_METHOD("get_children"), &MapRegion::get_children);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "children"), "set_children", "get_children");

	ClassDB::bind_method(D_METHOD("set_fill_value", "value"), &MapRegion::set_fill_value);
	ClassDB::bind_method(D_METHOD("get_fill_value"), &MapRegion::get_fill_value);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fill_value"), "set_fill_value", "get_fill_value");

	ClassDB::bind_static_method("MapRegion", D_METHOD("from_size", "size"), &MapRegion::from_size);
	ClassDB::bind_static_method("MapRegion", D_METHOD("from_data", "data"), &MapRegion::from_data);
	ClassDB::bind_static_method("MapRegion", D_METHOD("from_points", "points", "size"), &MapRegion::from_points);
	ClassDB::bind_static_method("MapRegion", D_METHOD("build_region_tree", "data"), &MapRegion::build_region_tree);
	ClassDB::bind_static_method("MapRegion", D_METHOD("find_regions", "data"), &MapRegion::find_regions);
	ClassDB::bind_static_method("MapRegion", D_METHOD("find_region", "start", "data", "visited", "fill_value"), &MapRegion::find_region, DEFVAL(1));
	ClassDB::bind_static_method("MapRegion", D_METHOD("visit_region", "start", "data", "visited", "fill_value"), &MapRegion::visit_region, DEFVAL(1));
}
