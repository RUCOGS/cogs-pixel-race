class_name Goal
extends Area2D


signal contestant_won(contestant: PixelContestant)


func _ready() -> void:
	body_entered.connect(_on_body_entered)

func _on_body_entered(body: Node2D):
	if body is PixelContestant:
		contestant_won.emit(body)
