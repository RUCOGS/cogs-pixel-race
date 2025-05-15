class_name SpriteConstructor
extends Node


@export var sprite: Sprite2D
@export var pixel_contestant: PixelContestant


func construct(texture: Texture2D):
	sprite.texture = texture
	pixel_contestant.icon = texture
	pixel_contestant.color = _get_average_color(texture.get_image())
	pixel_contestant.contestant_name = texture.resource_path.get_file().get_basename()


func _get_average_color(image: Image) -> Color:
	var color_freq = {}
	for y in image.get_height():
		for x in image.get_width():
			var color = image.get_pixel(x, y)
			if color.a == 0:
				continue
			if color not in color_freq:
				color_freq[color] = 0
			color_freq[color] += 1
	var arr = color_freq.keys()
	arr.sort_custom(func(x, y): return color_freq[x] > color_freq[y])
	if arr.size() > 0:
		return Color(arr[0], 1.0)
	return Color.BLACK
