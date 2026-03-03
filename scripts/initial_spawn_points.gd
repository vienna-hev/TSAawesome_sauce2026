extends Node2D

var player_scenes = {
	"player1": "res://scenes/player_1.tscn",
	"player2": "res://scenes/player_2.tscn",
	"player3": "res://scenes/player_3.tscn",
	"player4": "res://scenes/player_4.tscn"
}

@onready var many_spawn_points: Node2D = $"."  # Ensure this is correctly pointing to the SpawnPoints node

func _ready() -> void:
	var spawn_parent = many_spawn_points  # Safely get the node
	var spawn_points = spawn_parent.get_children()  # Get all Marker2D nodes

	for i in range(GameManager.player_count):
		var player_key = "player" + str(i + 1)
		var scene = load(player_scenes[player_key])
		if scene and i < spawn_points.size():
			var player_instance = scene.instantiate()
			player_instance.position = spawn_points[i].position  # Set player spawn point
			add_child(player_instance)
