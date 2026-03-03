extends Area2D

@onready var level_1

# Preload player scenes (ensure these paths point to the correct player scenes)
var player_scenes = {
	"player1": preload("res://scenes/player_1.tscn"),
	"player2": preload("res://scenes/player_2.tscn"),
	"player3": preload("res://scenes/player_3.tscn"),
	"player4": preload("res://scenes/player_4.tscn")
}

# Dictionary to keep track of player instances
var players = {
	"player1": null,
	"player2": null,
	"player3": null,
	"player4": null
}

@onready var camera = get_node("/root/Level_1/Camera2D")

# Track the player that is respawning to prevent multiple respawns at once
var respawn_player = null

# This function handles when a player enters the killzone (dies)
func _on_body_entered(body: Node2D) -> void:
	for key in players.keys():
		if body.is_in_group(key):  # If the body belongs to a player group
			print("Body entered:" + body.name + key + " died")  # Debugging: Show which player is dying

			# Defer queue_free() to avoid physics state errors
			body.call_deferred("queue_free")
			camera.remove_target(body)

			respawn_player = key  # Store the player to respawn
			players[key] = null
			
			var respawn_position = RespawnManager.player_respawn_positions[key]
			
			# Defer the respawn process
			call_deferred("_respawn_player", key, respawn_position)
			return
		else:
			print("Error: respawn position for " + key + " is null!")

# This function handles the actual respawn logic
func _respawn_player(player_key: String, respawn_position: Vector2) -> void:
	var new_player = player_scenes[player_key].instantiate()
	new_player.position = respawn_position  # Set position to the updated respawn point
	print(new_player.position)

	if get_parent().is_in_group("enemies"):
		get_parent().get_parent().add_child(new_player)
	else:
		get_parent().add_child(new_player)

	players[player_key] = new_player
	camera.add_target(new_player)
	print(player_key + " respawned at " + str(new_player.position))
	respawn_player = null
