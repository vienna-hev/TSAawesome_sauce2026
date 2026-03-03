extends Node2D

@onready var left: Marker2D = $"Camera2D/left limit"
@onready var right: Marker2D = $"Camera2D/right limit"

var player_scenes = {
	"player1": "res://scenes/player_1.tscn",
	"player2": "res://scenes/player_2.tscn",
	"player3": "res://scenes/player_3.tscn",
	"player4": "res://scenes/player_4.tscn"
}

func _ready() -> void:
	var spawn_points = $"Initial Spawn Points".get_children()  # Get all Marker2D nodes

	var side_screen_left: Vector2 = left.global_position
	var side_screen_right: Vector2 = right.global_position

	for i in range(GameManager.player_count):
		var player_key = "player" + str(i + 1)
		var scene = load(player_scenes[player_key])
		if scene and i < spawn_points.size():
			var player_instance = scene.instantiate()
			player_instance.position = spawn_points[i].position  # Set player spawn point
			add_child(player_instance)
			$Camera2D.add_target(player_instance)
	$Camera2D.limit_left = side_screen_left.x
	$Camera2D.limit_right = side_screen_right.x
