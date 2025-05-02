class_name AnimFlagPlayer
extends AnimationPlayer


@export var flags: String
@export var oneshot_flags: String
@export var clips: Array[AnimFlagPlayerClip]

var flags_set: Dictionary[String, bool] = {}
var oneshot_flags_set: Dictionary[String, bool] = {}
# Map flag -> true/false
var flag_states: Dictionary[String, bool] = {}
var current_clip: AnimFlagPlayerClip = null
var default_clip: AnimFlagPlayerClip = null


func _ready() -> void:
	animation_finished.connect(_on_animation_finished)
	for flag in flags.strip_edges().split(",", false):
		flags_set[flag] = true
		flag_states[flag] = false
	for flag in oneshot_flags.strip_edges().split(",", false):
		oneshot_flags_set[flag] = true
		flag_states[flag] = false
	for clip in clips:
		clip.init()
		if clip.flags_set.is_empty():
			default_clip = clip
	update_animation()


func _on_animation_finished(anim_name: String):
	if current_clip.animation == anim_name:
		for flag in current_clip.flags_set:
			if flag in oneshot_flags_set:
				flag_states[flag] = false
		current_clip = null
	update_animation()


func set_flag(flag: String, res: bool, update: bool = true):
	flag_states[flag] = res
	if update:
		update_animation()


func set_flags(flags: PackedStringArray, res: bool, update: bool = true):
	for flag in flags:
		flag_states[flag] = res
	if update:
		update_animation()


func update_animation():
	# Play the next animation that meets all the flags (excluding default clips)
	for clip in clips:
		if clip == default_clip:
			continue
		var flags_met = true
		for clip_flag in clip.flags_set:
			if not flag_states[clip_flag]:
				flags_met = false
				break
		if flags_met:
			current_clip = clip
			play(clip.animation)
			return
	# Play default clip if nothing else is available
	if default_clip:
		play(default_clip.animation)
