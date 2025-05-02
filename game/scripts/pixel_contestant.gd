class_name PixelContestant
extends CharacterBody2D


signal bumped(other: Node2D)

@export var speed: float = 100
@export var angle_jitter: Vector2 = Vector2(-10, 10)

var direction = Vector2.RIGHT


func _enter_tree() -> void:
	if visible == false:
		queue_free()


func _physics_process(delta: float) -> void:
	var collision = move_and_collide(direction * speed * delta)
	if collision:
		direction = direction.bounce(collision.get_normal()).rotated(deg_to_rad(randf_range(angle_jitter.x, angle_jitter.y)))
		bumped.emit()
