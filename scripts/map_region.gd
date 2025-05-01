class_name MapRegion
extends RefCounted


var data: PackedByteArray2D
var points: Dictionary[Vector2i, bool]
var parent: MapRegion

var size: Vector2i:
	get:
		return data.size


func _init(points: Dictionary[Vector2i, bool], size: Vector2i) -> void:
	self.data = PackedByteArray2D.new(size)
	self.points = points
	self.parent = null
	for pos in points:
		data.setv(pos, 1)


func in_bounds(pos: Vector2i):
	return data.in_bounds(pos)


func setv(pos: Vector2i, value: int):
	data.setv(pos, value)
	if value != 0:
		points[pos] = true
	else:
		points.erase(pos)


func getv(pos: Vector2i) -> int:
	return data.getv(pos)


# Applies a function over all points
# callable = func(pos: Vector2i, x: int) -> int 
func apply(callable: Callable):
	for y in range(size.y):
		for x in range(size.x):
			var pos = Vector2i(x, y)
			setv(pos, callable.call(pos, getv(pos)))


# Returns copy of current region with points negated
func get_negated() -> MapRegion:
	var new = MapRegion.from_size(size)
	new.apply(func(pos, x): return 0 if pos in points else 1)
	return new


# Returns a region that is the opposite of the outside
# 	AKA returns the region with its insides filled
func get_inside() -> MapRegion:
	var outside = get_outside()
	return outside.get_negated()


# Return a region that encircles the current region from the edges of the data
func get_outside() -> MapRegion:
	var visited = PackedByteArray2D.new(size)
	print("get outside of ", self)
	for y in range(size.y):
		# Try create regions from left and right edges
		var pos = Vector2i(0, y)
		if pos not in self.points:
			visit_region(pos, data, visited, 1)
		pos = Vector2i(size.x - 1, y)
		if pos not in self.points:
			visit_region(pos, data, visited, 1)
	for x in range(size.x):
		# Try create regions from top and bottom edges
		var pos = Vector2i(x, 0)
		if pos not in self.points:
			visit_region(pos, data, visited, 1)
		pos = Vector2i(x, size.y - 1)
		if pos not in self.points:
			visit_region(pos, data, visited, 1)
	# Now visited should contain all reachable positions from the edge
	return MapRegion.from_data(visited)


func is_empty() -> bool:
	return points.size() == 0


func _to_string() -> String:
	return "MapRegion: (%s) {%s}" % [self.points.size(), self.data.to_string()]


func duplicate() -> MapRegion:
	var new = MapRegion.new(self.points.duplicate(), size)
	return new


static func from_size(size: Vector2i) -> MapRegion:
	return MapRegion.new({}, size)


static func from_data(data: PackedByteArray2D) -> MapRegion:
	var points: Dictionary[Vector2i, bool] = {}
	for y in range(data.size.y):
		for x in range(data.size.x):
			var pos = Vector2i(x, y)
			if data.getv(pos) != 0:
				points[pos] = true
	return MapRegion.new(points, data.size)


static func find_regions(data: PackedByteArray2D) -> Array[MapRegion]:
	var visited = PackedByteArray2D.new(data.size)
	var regions: Array[MapRegion] = []
	var region_id = 0
	for y in range(data.size.y):
		for x in range(data.size.x):
			var pos = Vector2i(x, y)
			var region = find_region(pos, data, visited)
			if not region.is_empty():
				regions.append(region)
	return regions


const OFFSETS = [Vector2i.DOWN, Vector2i.UP, Vector2i.LEFT, Vector2i.RIGHT]

static func find_region(start: Vector2i, data: PackedByteArray2D, visited: PackedByteArray2D, empty_value: int = 0) -> MapRegion:
	var stack = [start]
	var region_data: Dictionary[Vector2i, bool] = {}
	while stack:
		var pos = stack.pop_back()
		if not visited.in_bounds(pos) or visited.getv(pos) == 1 or data.getv(pos) == empty_value:
			# Already visited
			continue
		# Not visited, mark as visited
		visited.setv(pos, 1)
		region_data[pos] = true
		for offset in OFFSETS:
			stack.push_back(pos + offset)
	return MapRegion.new(region_data, data.size)


static func visit_region(start: Vector2i, data: PackedByteArray2D, visited: PackedByteArray2D, empty_value: int = 0):
	var stack = [start]
	var region_data: Dictionary[Vector2i, bool] = {}
	while stack:
		var pos = stack.pop_back()
		if not visited.in_bounds(pos) or visited.getv(pos) == 1 or data.getv(pos) == empty_value:
			# Already visited
			continue
		# Not visited, mark as visited
		visited.setv(pos, 1)
		region_data[pos] = true
		for offset in OFFSETS:
			stack.push_back(pos + offset)
