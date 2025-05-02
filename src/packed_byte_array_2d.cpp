
#include "packed_byte_array_2d.hpp"

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/templates/vector.hpp>

Vector2i PackedByteArray2D::get_size()
{
	return size;
}

void PackedByteArray2D::resize(const Vector2i &new_size)
{
	ERR_FAIL_COND_MSG(new_size.x < 0 || new_size.y < 0, "Expected new_size to be > 0");
	size = new_size;
	array.resize(new_size.x * new_size.y);
}

bool PackedByteArray2D::in_bounds(const Vector2i &pos)
{
	return pos.x >= 0 && pos.y >= 0 && pos.x < size.x && pos.y < size.y;
}

bool PackedByteArray2D::try_setv(const Vector2i &pos, uint8_t value)
{
	if (!in_bounds(pos))
		return false;
	setv(pos, value);
	return true;

	// Returns value if value exists
}

uint8_t PackedByteArray2D::try_getv(const Vector2i &pos)
{
	if (!in_bounds(pos))
		return 0;
	return getv(pos);
}

void PackedByteArray2D::setv(const Vector2i &pos, uint8_t value)
{
	array.set(pos.y * size.x + pos.x, value);
}

uint8_t PackedByteArray2D::getv(const Vector2i &pos)
{
	return array.get(pos.y * size.x + pos.x);
}

void PackedByteArray2D::apply(Callable callable)
{
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			Vector2i pos = Vector2i(x, y);
			setv(pos, callable.call(pos, getv(pos)));
		}
	}
}

Ref<PackedByteArray2D> PackedByteArray2D::duplicate()
{
	Ref<PackedByteArray2D> copy = PackedByteArray2D::from_size(size);
	copy->array = array.duplicate();
	return copy;
}

String PackedByteArray2D::_to_string()
{
	PackedStringArray str = PackedStringArray{
			/* initializer lists are unsupported */ "PackedByteArray2D: [\n",
	};
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			auto res = getv(Vector2i(x, y));
			if (res != 0)
			{
				str.append(vformat("%3d", res));
			}
			else
			{
				str.append("   ");
			}
			if (x < size.x - 1)
			{
				str.append(" ");
			}
		}
		if (y < size.y - 1)
		{
			str.append("\n");
		}
	}
	str.append("]");
	return String().join(str);
}

Ref<BitMap> PackedByteArray2D::to_bitmap(uint8_t cutoff)
{
	Ref<BitMap> bitmap;
	bitmap.instantiate();
	bitmap->create(size);
	for (int y = 0; y < size.y; y += 1)
	{
		for (int x = 0; x < size.x; x += 1)
		{
			Vector2i pos = Vector2i(x, y);
			bitmap->set_bitv(pos, getv(pos) >= cutoff);
		}
	}
	return bitmap;
}

Ref<PackedByteArray2D> PackedByteArray2D::from_size(Vector2i size)
{
	Ref<PackedByteArray2D> ref;
	ref.instantiate();
	ref->resize(size);
	return ref;
}

Ref<PackedByteArray2D> PackedByteArray2D::from_image(const Ref<Image> image, String channel)
{
	Vector2i image_size = image->get_size();
	Ref<PackedByteArray2D> res = PackedByteArray2D::from_size(image_size);
	for (int y = 0; y < image_size.y; y++)
	{
		for (int x = 0; x < image_size.x; x++)
		{
			Vector2i pos = Vector2i(x, y);
			Color color = image->get_pixelv(pos);
			if (channel == "r")
				res->setv(pos, color.get_r8());
			else if (channel == "g")
				res->setv(pos, color.get_g8());
			else if (channel == "b")
				res->setv(pos, color.get_b8());
			else if (channel == "a")
				res->setv(pos, color.get_a8());
		}
	}
	return res;
}

void PackedByteArray2D::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_to_string"), &PackedByteArray2D::_to_string);
	ClassDB::bind_method(D_METHOD("size"), &PackedByteArray2D::get_size);
	ClassDB::bind_method(D_METHOD("resize", "new_size"), &PackedByteArray2D::resize);
	ClassDB::bind_method(D_METHOD("in_bounds", "pos"), &PackedByteArray2D::in_bounds);
	ClassDB::bind_method(D_METHOD("try_setv", "pos", "value"), &PackedByteArray2D::try_setv);
	ClassDB::bind_method(D_METHOD("try_getv", "pos"), &PackedByteArray2D::try_getv);
	ClassDB::bind_method(D_METHOD("setv", "pos", "value"), &PackedByteArray2D::setv);
	ClassDB::bind_method(D_METHOD("getv", "pos"), &PackedByteArray2D::getv);
	ClassDB::bind_method(D_METHOD("apply", "callable"), &PackedByteArray2D::apply);
	ClassDB::bind_method(D_METHOD("duplicate"), &PackedByteArray2D::duplicate);
	ClassDB::bind_method(D_METHOD("to_bitmap", "cutoff"), &PackedByteArray2D::to_bitmap);

	ClassDB::bind_static_method("PackedByteArray2D", D_METHOD("from_image", "image", "channel"), &PackedByteArray2D::from_image, DEFVAL(String("a")));
	ClassDB::bind_static_method("PackedByteArray2D", D_METHOD("from_size", "size"), &PackedByteArray2D::from_size);
}
