@tool
class_name Map
extends StaticBody2D

@export var map_texture: Texture2D :
	get:
		return map_texture
	set(value):
		map_texture = value
		if _readied:
			if map_texture:
				map_sprite.texture = map_texture
				bg_sprite.scale = map_texture.get_size()
			else:
				map_sprite.texture = null
				bg_sprite.scale = Vector2.ZERO
@export var bg_color: Color :
	get:
		return bg_color
	set(value):
		bg_color = value
		if _readied:
			bg_sprite.modulate = value
@export var wall_color: Color

@export_group("Dependencies")
@export var map_sprite: Sprite2D
@export var bg_sprite: Sprite2D

var _readied = false


func _ready() -> void:
	_readied = true
	if Engine.is_editor_hint():
		return
	RenderingServer.set_default_clear_color(wall_color)
	create_colliders(0.4, 2)


func create_colliders(resolution: float = 0.05, polygon_epsilon: float = 1.0) -> void:
	for child in get_children():
		if child is CollisionPolygon2D:
			child.queue_free()
	
	var image = map_texture.get_image().duplicate() as Image
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
				collision_polygon.position -= (Vector2(region.size()) / resolution) / 2
		stack.append_array(region.children)
