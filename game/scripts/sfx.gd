class_name SFX
extends AudioStreamPlayer


@export var clips: Array[AudioStream]
@export var rand_pitch: Vector2 = Vector2.ONE
@export var rand_volume: Vector2 = Vector2.ZERO


func play_rand():
	stream = clips.pick_random()
	pitch_scale = randf_range(rand_pitch.x, rand_pitch.y)
	volume_db = randf_range(rand_volume.x, rand_volume.y)
	play()
