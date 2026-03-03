extends Control

func _ready() -> void:
	$"MarginContainer/VBoxContainer/two player button".grab_focus()

func _on_two_player_button_pressed() -> void:
	GameManager.player_count = 2
	get_tree().change_scene_to_file("res://scenes/Levels/level_1.tscn")

func _on_three_player_button_pressed() -> void:
	GameManager.player_count = 3
	get_tree().change_scene_to_file("res://scenes/Levels/level_1.tscn")

func _on_four_player_button_pressed() -> void:
	GameManager.player_count = 4
	get_tree().change_scene_to_file("res://scenes/Levels/level_1.tscn")
