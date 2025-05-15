class_name FX
extends Node


signal played
signal finished


@export var duration: Vector2 = Vector2.ONE
# Minimum distance between repeats of this sound
@export var min_interval: float = 0.3
@export var unparent_on_play: bool = false
@export var free_on_finished: bool = false
@export var is_looping: bool = false
@export var play_on_awake: bool = false

@export_group("Dependencies")
@export var cond_target: Node
@export var cond_property: String
@export var fxes: Array[FX]
@export var sfxes: Array[SFX]
@export var anim_flag_player_fxes: Array[AnimFlagPlayerFX]
@export var audio_stream_players: Array[AudioStreamPlayer]
@export var cpu_particles: Array[CPUParticles2D]
@export var gpu_particles: Array[GPUParticles2D]

var start_duration: float = 1000000
var time: float = 0
var is_playing: bool


func _ready() -> void:
	if play_on_awake:
		play()
	var tracked_children = {}
	for nodes in [fxes, sfxes, anim_flag_player_fxes, audio_stream_players, cpu_particles, gpu_particles]:
		for node in nodes:
			tracked_children[node] = true
	for child in get_children():
		if child in tracked_children:
			continue
		if child is FX:
			fxes.append(child)
		elif child is SFX:
			sfxes.append(child)
		elif child is AnimFlagPlayerFX:
			anim_flag_player_fxes.append(child)
		elif child is AudioStreamPlayer:
			audio_stream_players.append(child)
		elif child is CPUParticles2D:
			cpu_particles.append(child)
		elif child is GPUParticles2D:
			gpu_particles.append(child)


func play():
	if time > (start_duration - min_interval):
		return
	if unparent_on_play:
		reparent(World.global)
	is_playing = true
	for fx in fxes:
		fx.play()
	for sfx in sfxes:
		sfx.play_rand()
	for anim_flag_player_fx in anim_flag_player_fxes:
		anim_flag_player_fx.play()
	for player in audio_stream_players:
		player.play()
	for particle in cpu_particles:
		particle.emitting = true
	for particle in gpu_particles:
		particle.emitting = true
	played.emit()
	start_duration = randf_range(duration.x, duration.y)
	time = start_duration


func stop():
	for fx in fxes:
		fx.stop()
	for sfx in sfxes:
		sfx.stop()
	for player in audio_stream_players:
		player.stop()
	for particle in cpu_particles:
		particle.emitting = false
	for particle in gpu_particles:
		particle.emitting = false
	is_playing = false
	finished.emit()
	if free_on_finished:
		queue_free()


func _process(delta: float) -> void:
	if cond_target and cond_property in cond_target:
		is_playing = cond_target.get(cond_property)
	if is_playing:
		time -= delta
		if time <= 0:
			if is_looping:
				play()
			else:
				is_playing = false
				stop()
