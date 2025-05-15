class_name PixelContestant
extends CharacterBody2D


signal bumped(other: Node2D)
signal active_changed(active: bool)

@export var color: Color
@export var contestant_name: String
@export var icon: Texture2D
@export var speed: float = 100
@export var angle_jitter: Vector2 = Vector2(-10, 10)
@export var active: bool = false :
	get:
		return active
	set(value):
		active = value
		if name_label:
			name_label.visible = not active
		active_changed.emit(active)
@export_group("Dependencies")
@export var name_label: Label

var direction = Vector2.RIGHT


func _ready() -> void:
	name_label.text = contestant_name

func _physics_process(delta: float) -> void:
	if not active:
		return
	var collision = move_and_collide(direction * speed * delta)
	if collision:
		direction = direction.bounce(collision.get_normal()).rotated(deg_to_rad(randf_range(angle_jitter.x, angle_jitter.y)))
		bumped.emit()
