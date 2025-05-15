class_name OutlinedNode
extends CanvasItem


@export var transparency: float = 1.0


func _ready() -> void:
	material = material.duplicate()


func _process(delta: float) -> void:
	(material as ShaderMaterial).set_shader_parameter("transparency", transparency)
