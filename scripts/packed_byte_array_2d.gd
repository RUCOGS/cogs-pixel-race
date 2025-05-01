class_name PackedByteArray2D
extends RefCounted


var size: Vector2i
var array: PackedByteArray


func _init(size: Vector2i) -> void:
	self.size = size
	array = PackedByteArray()
	resize(size)


func resize(new_size: Vector2i):
	assert(new_size.x > 0 and new_size.y > 0)
	array.resize(new_size.x * new_size.y)


func in_bounds(pos: Vector2i) -> bool:
	return pos.x >= 0 and pos.y >= 0 and pos.x < size.x and pos.y < size.y


func try_setv(pos: Vector2i, value: int) -> bool:
	if not in_bounds(pos):
		return false
	setv(pos, value)
	return true


# Returns value if value exists
# Returns null if value does not exist
func try_getv(pos: Vector2i):
	if not in_bounds(pos):
		return null
	return getv(pos)


func setv(pos: Vector2i, value: int):
	array[pos.y * size.x + pos.x] = value


func getv(pos: Vector2i):
	return array[pos.y * size.x + pos.x]


# callable = func(pos: Vector2i, x: int) -> int 
func apply(callable: Callable):
	for y in range(size.y):
		for x in range(size.x):
			var pos = Vector2i(x, y)
			setv(pos, callable.call(pos, getv(pos)))


func _to_string() -> String:
	var str = ["PackedByteArray2D: [\n"]
	for y in range(size.y):
		for x in range(size.x):
			var res = getv(Vector2i(x, y))
			if res != 0:
				str.append("%3d" % res)
			else:
				str.append("   ")
			if x < size.x - 1:
				str.append(" ")
		if y < size.y - 1:
			str.append("\n")
	str.append("]")
	return "".join(str)


func to_bitmap(cutoff: int = 128) -> BitMap:
	var bitmap = BitMap.new()
	bitmap.create(size)
	for y in range(size.y):
		for x in range(size.x):
			var pos = Vector2i(x, y)
			bitmap.set_bitv(pos, getv(pos) >= cutoff)
	return bitmap


static func from_image(image: Image, channel: String = "a") -> PackedByteArray2D:
	var image_size = image.get_size()
	var res = PackedByteArray2D.new(image_size)
	for y in range(image_size.y):
		for x in range(image_size.x):
			var pos = Vector2i(x, y)
			var color = image.get_pixelv(pos)
			if channel == "r":
				res.setv(pos, color.r8)
			elif channel == "g":
				res.setv(pos, color.g8)
			elif channel == "b":
				res.setv(pos, color.b8)
			elif channel == "a":
				res.setv(pos, color.a8)
	return res
