class_name AnimFlagPlayerClip
extends Resource


@export var animation: String
@export var flags: String

var flags_set: Dictionary[String, bool]


func init() -> void:
	flags_set = {}
	for flag in flags.strip_edges().split(",", false):
		flags_set[flag] = true
