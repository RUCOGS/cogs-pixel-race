class_name Map
extends StaticBody2D

@export var sprite: Sprite2D


func _ready() -> void:
	create_colliders(0.1, 1.0)


func create_colliders(resolution: float = 0.05, polygon_epsilon: float = 1.0) -> void:
	for child in get_children():
		if child is CollisionPolygon2D:
			child.queue_free()
	
	var image = sprite.texture.get_image().duplicate() as Image
	image.resize(round(image.get_size().x * resolution), round(image.get_size().y * resolution), 0)
	var raw_alpha = PackedByteArray2D.from_image(image)
	raw_alpha.apply(func(pos, x): return 1 if x > 0 else 0)
	#print("raw_alpha: ", raw_alpha)
			
	var root = MapRegion.build_region_tree(raw_alpha)
	root.recache_polygon_tree(polygon_epsilon)
	#print("root: ", root)
	
	var stack = [root]
	while stack:
		var region = stack.pop_back() as MapRegion
		if region.fill_value == 1:
			var polygons = []
			var right_polygons = [region.get_polygon()]
			for child in region.children:
				var new_right_polygons = []
				for polygon in right_polygons:
					var res = GeometryUtils.cut_polygon_along_line(polygon, child.get_center(), 0)
					# Subtract child region from left and right polygons
					
					for lpoly in res[0]:
						polygons.append_array(Geometry2D.clip_polygons(lpoly, child.get_polygon()))
					for rpoly in res[1]:
						new_right_polygons.append_array(Geometry2D.clip_polygons(rpoly, child.get_polygon()))
				right_polygons = new_right_polygons
			if right_polygons.size() > 0:
				# Add leftover polygons
				polygons.append_array(right_polygons)
			#print("polygons (%s): %s" % [polygons.size(), polygons])
			for polygon in polygons:
				# Undo the shrinking (scaling up)
				var scale_transform = Transform2D(0, Vector2.ONE / resolution, 0, Vector2.ZERO)
				var collision_polygon = CollisionPolygon2D.new()
				collision_polygon.polygon = GeometryUtils.transform_polygon(scale_transform, polygon)
				add_child(collision_polygon)
				collision_polygon.position -= (Vector2(region.size) / resolution) / 2
		stack.append_array(region.children)




#
## Returns list of points on the edge of the region
#func get_region_edges(region: Dictionary[Vector2i, bool]) -> Dictionary[Vector2i, bool]:
	#var edges: Dictionary[Vector2i, bool] = {}
	#for pos in region:
		#for offset in OFFSETS:
			#var new_pos = pos + offset
			#if new_pos not in region:
				#edges[new_pos] = true
	#return edges
#
#
## Checks if region_a overlaps region_b
## If a overlaps b, then the edges of a must all be surrounded by region_b
#func is_region_overlaping(region_a: Dictionary[Vector2i, bool], region_b: Dictionary[Vector2i, bool]):
	#var a_edges = get_region_edges(region_a)
	#for pos in a_edges:
		#if pos not in region_b:
			#return false
	#return true
