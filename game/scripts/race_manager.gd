class_name RaceManager
extends Node


enum Mode {
	IDLE,
	COUNTDOWN,
	RACING,
	WIN
}


var map: Map
var time: float = 0
var mode: Mode = Mode.IDLE
var contestants: Array[PixelContestant]


@export_group("Dependencies")
@export var timer_label: Label
@export var name_label: Label
@export var race_settings: RaceSettings
@export var start_button: Button
@export var menu_ui: Control
@export var game_ui: Control
@export var win_ui: Control
@export var win_contestant_texture_rect: TextureRect
@export var win_color_rect: ColorRect
@export var win_name_label: Label
@export var win_sfx: AudioStreamPlayer
@export var start_sfx: AudioStreamPlayer
@export var bgm: AudioStreamPlayer
@export var map_option_button: OptionButton
@export var world: World
@export var camera: Camera2D
@export var contestants_container: Node2D
@export var sprite_contestant_prefab: PackedScene

var _win_tween: Tween


func _ready() -> void:
	menu_ui.visible = true
	game_ui.visible = false
	win_ui.visible = false
	get_window().size = Vector2i(1024, 1024)
	_create_contestants()
	start_button.pressed.connect(start_race)
	map_option_button.clear()
	for map_scene in race_settings.maps:
		var inst = map_scene.instantiate() as Map
		map_option_button.add_item(inst.map_name)
	map_option_button.item_selected.connect(_on_map_selected)
	map_option_button.select(0)
	_on_map_selected(0)

func _create_contestants():
	for contestant in contestants:
		contestants_container.add_child(contestant)
	contestants.clear()
	for child in contestants_container.get_children():
		child.queue_free()
	for texture in race_settings.sprite_contestants:
		var sprite_contestant = sprite_contestant_prefab.instantiate()
		var sprite_constructor = sprite_contestant.get_node("SpriteConstructor") as SpriteConstructor
		sprite_constructor.construct(texture)
		contestants_container.add_child(sprite_contestant)
		contestants.append(sprite_contestant)
	for prefab in race_settings.custom_contestants:
		var contestant = prefab.instantiate()
		contestants_container.add_child(contestant)
		contestants.append(contestant)

func _on_map_selected(index: int):
	if index < 0:
		return
	var map_scene = race_settings.maps[index]
	if map:
		map.queue_free()
	map = map_scene.instantiate() as Map
	map.contestant_won.connect(_on_contestant_won)
	world.add_child(map)
	for i in range(contestants.size()):
		contestants[i].active = false
		if i < map.spawns_container.get_child_count():
			var spawn = map.spawns_container.get_child(i) as ContestantSpawn
			contestants[i].global_position = spawn.global_position
		else:
			contestants[i].visible = false
	camera.zoom = 1024 / map.map_texture.get_size().y * ProjectSettings.get_setting("display/window/size/viewport_height") / 1024 * Vector2.ONE
	camera.global_position = Vector2.ZERO

func _process(delta: float) -> void:
	if Input.is_key_pressed(KEY_ESCAPE):
		end_race()
	if mode == Mode.RACING:
		time += delta
		_update_timer_label()

func _update_timer_label():
	var total_secs = int(time)
	var secs = total_secs % 60
	var mins = total_secs / 60
	timer_label.text = "%02d:%02d" % [mins, secs]

func _on_contestant_won(winner: PixelContestant):
	if mode != Mode.RACING:
		return
	get_tree().paused = true
	bgm.stop()
	
	# Tween to contestant
	_win_tween = create_tween()
	_win_tween.tween_property(camera, "global_position", winner.global_position, 2.0) \
		.set_ease(Tween.EASE_OUT) \
		.set_trans(Tween.TRANS_CUBIC)
	_win_tween.parallel()
	_win_tween.tween_property(camera, "zoom", Vector2.ONE * 5, 2.0) \
		.set_ease(Tween.EASE_OUT) \
		.set_trans(Tween.TRANS_CUBIC) 
	await _win_tween.finished
	
	# Show win screen
	win_name_label.text = winner.contestant_name
	win_color_rect.color = winner.color
	win_contestant_texture_rect.texture = winner.icon
	win_contestant_texture_rect.transparency = 0.0
	win_ui.visible = true
	win_ui.modulate = Color(Color.WHITE, 0)
	_win_tween = create_tween()
	_win_tween.tween_property(win_ui, "modulate", Color.WHITE, 2.0) \
		.set_ease(Tween.EASE_OUT) \
		.set_trans(Tween.TRANS_CUBIC)
	_win_tween.parallel()
	_win_tween.tween_property(win_contestant_texture_rect, "transparency", 1.0, 2.0) \
		.set_ease(Tween.EASE_OUT) \
		.set_trans(Tween.TRANS_CUBIC)
	mode = Mode.WIN
	await get_tree().create_timer(1.0).timeout
	if mode != Mode.WIN:
		return
	win_sfx.play()
	await get_tree().create_timer(5.0).timeout
	if mode != Mode.WIN:
		return
	get_tree().paused = false
	end_race()

func start_race():
	if mode != Mode.IDLE:
		return
	menu_ui.visible = false
	game_ui.visible = true
	win_ui.visible = false
	name_label.text = map.map_name
	time = 0
	map.spawns_container.visible = false
	_update_timer_label()
	mode = Mode.COUNTDOWN
	await get_tree().create_timer(1.0).timeout
	# TODO: Implement countdown
	start_sfx.play()
	mode = Mode.RACING
	await get_tree().create_timer(0.5).timeout
	for i in range(contestants.size()):
		contestants[i].active = true
	await get_tree().create_timer(2.0).timeout
	if mode == Mode.RACING:
		bgm.play()

func end_race():
	if mode == Mode.IDLE:
		return
	if _win_tween:
		_win_tween.kill()
		_win_tween = null
	menu_ui.visible = true
	game_ui.visible = false
	win_ui.visible = false
	bgm.stop()
	get_tree().paused = false
	mode = Mode.IDLE
	_on_map_selected(map_option_button.selected)
