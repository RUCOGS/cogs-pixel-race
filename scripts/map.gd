class_name Map
extends StaticBody2D

@export var sprite: Sprite2D


func _ready() -> void:
	create_colliders()


func create_colliders() -> void:
	var image = sprite.texture.get_image()
	var raw_alpha = PackedByteArray2D.from_image(image)
	var found_region = MapRegion.find_region(Vector2i(6, 6), raw_alpha, PackedByteArray2D.new(raw_alpha.size))
	print("found_region: ", found_region)
	var outside_region = found_region.get_outside()
	print("outside_region: ", outside_region)
	var inside_region = found_region.get_inside()
	print("inside_region: ", inside_region)
	#var regions = find_regions(raw_alpha)
	#for child in get_children():
		#if child is CollisionPolygon2D:
			#child.queue_free()
	#for region in regions:
		#var bitmap = BitMap.new()
		#bitmap.resize(raw_alpha.size)
		#for pos in region:
			#bitmap.set_bitv(pos, true)
		#var polygons = bitmap.opaque_to_polygons(Rect2i(Vector2i.ZERO, bitmap.get_size()))
		#for polygon in polygons:
			#var collision_polygon = CollisionPolygon2D.new()
			#collision_polygon.polygon = polygon
			#add_child(collision_polygon)
	#var new_texture = ImageTexture.create_from_image()
	#sprite.texture = new_texture




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
