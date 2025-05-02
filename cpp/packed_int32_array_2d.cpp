
#include "packed_int32_array_2d.hpp"

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void PackedInt32Array2D::_init(Vector2i size)
{
	this->set_size(size);
	array = PackedInt32Array();
	resize(size);
}

void PackedInt32Array2D::resize(Vector2i new_size)
{
	assert(new_size.x > 0 && new_size.y > 0);
	array.resize(new_size.x * new_size.y);
}

bool PackedInt32Array2D::in_bounds(Vector2i pos)
{
	return pos.x >= 0 && pos.y >= 0 && pos.x < size.x && pos.y < size.y;
}

bool PackedInt32Array2D::try_setv(Vector2i pos, int value)
{
	if(!in_bounds(pos))
	{
		return false;
	}
	setv(pos, value);
	return true;

	// Returns value if value exists

}

void PackedInt32Array2D::try_getv(Vector2i pos)
{
	if(!in_bounds(pos))
	{
		return null;
	}
	return getv(pos);
}

void PackedInt32Array2D::setv(Vector2i pos, int value)
{
	array[pos.y * size.x + pos.x] = value;
}

void PackedInt32Array2D::getv(Vector2i pos)
{
	return array[pos.y * size.x + pos.x];
}

void PackedInt32Array2D::apply(Callable callable)
{
	for(int y=0; y<size.y; y+=1)
	{
		for(int x=0; x<size.x; x+=1)
		{
			Vector2i pos = Vector2i(x, y);
			setv(pos, callable.call(pos, getv(pos)));
		}
	}
}

Ref<PackedByteArray2D> PackedInt32Array2D::duplicate()
{
	Variant copy = PackedByteArray2D->new(size);
	copy->array = array.duplicate();
	return copy;
}

String PackedInt32Array2D::_to_string()
{
	Array str = Array {/* initializer lists are unsupported */ "PackedInt32Array2D: [\n",  };
	for(int y=0; y<size.y; y+=1)
	{
		for(int x=0; x<size.x; x+=1)
		{
			Variant res = getv(Vector2i(x, y));
			if(res != 0)
			{
				str.append("%3d" % res);
			}
			else
			{
				str.append("   ");
			}
			if(x < size.x - 1)
			{
				str.append(" ");
			}
		}
		if(y < size.y - 1)
		{
			str.append("\n");
		}
	}
	str.append("]");
	return "".join(str);
}

Ref<BitMap> PackedInt32Array2D::to_bitmap(int cutoff)
{
	Variant bitmap = BitMap->new();
	bitmap->create(size);
	for(int y=0; y<size.y; y+=1)
	{
		for(int x=0; x<size.x; x+=1)
		{
			Vector2i pos = Vector2i(x, y);
			bitmap->set_bitv(pos, getv(pos) >= cutoff);
		}
	}
	return bitmap;
}

Ref<PackedInt32Array2D> PackedInt32Array2D::from_image(Ref<Image> image, String channel)
{
	Vector2i image_size = image->get_size();
	Variant res = PackedInt32Array2D->new(image_size);
	for(int y=0; y<image_size.y; y+=1)
	{
		for(int x=0; x<image_size.x; x+=1)
		{
			Vector2i pos = Vector2i(x, y);
			Color color = image->get_pixelv(pos);
			if(channel == "r")
			{
				res->setv(pos, color.r8);
			}
			else if(channel == "g")
			{
				res->setv(pos, color.g8);
			}
			else if(channel == "b")
			{
				res->setv(pos, color.b8);
			}
			else if(channel == "a")
			{
				res->setv(pos, color.a8);
			}
		}
	}
	return res;
}

void PackedInt32Array2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("resize", "new_size"), &PackedInt32Array2D::resize);
	ClassDB::bind_method(D_METHOD("in_bounds", "pos"), &PackedInt32Array2D::in_bounds);
	ClassDB::bind_method(D_METHOD("try_setv", "pos", "value"), &PackedInt32Array2D::try_setv);
	ClassDB::bind_method(D_METHOD("try_getv", "pos"), &PackedInt32Array2D::try_getv);
	ClassDB::bind_method(D_METHOD("setv", "pos", "value"), &PackedInt32Array2D::setv);
	ClassDB::bind_method(D_METHOD("getv", "pos"), &PackedInt32Array2D::getv);
	ClassDB::bind_method(D_METHOD("apply", "callable"), &PackedInt32Array2D::apply);
	ClassDB::bind_method(D_METHOD("duplicate"), &PackedInt32Array2D::duplicate);
	ClassDB::bind_method(D_METHOD("to_bitmap", "cutoff"), &PackedInt32Array2D::to_bitmap);
	ClassDB::bind_method(D_METHOD("from_image", "image", "channel"), &PackedInt32Array2D::from_image);

}

