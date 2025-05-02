class_name AnimFlagPlayerFX
extends Node


@export var anim_flag_player: AnimFlagPlayer
@export var set_flags: String
@export var unset_flags: String


func play():
	anim_flag_player.set_flags(set_flags.strip_edges().split(",", false), true, false)
	anim_flag_player.set_flags(unset_flags.strip_edges().split(",", false), false, false)
	anim_flag_player.update_animation()
