@tool
class_name ContestantSpawn
extends Node2D

@export_group("Dependencies")
@export var visuals: Node2D
@export var number_label: Label

func _process(delta: float) -> void:
	number_label.text = str(get_index())
