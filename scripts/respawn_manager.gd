extends Node

signal respawn_positions_updated

var player_respawn_positions = {
	"player1": null,
	"player2": null,
	"player3": null,
	"player4": null
}

func _ready():
	player_respawn_positions["player1"] = Vector2(-350, 92)
	player_respawn_positions["player2"] = Vector2(-350, 92)
	player_respawn_positions["player3"] = Vector2(-350, 92)
	player_respawn_positions["player4"] = Vector2(-350, 92)
	RespawnManager.respawn_positions_updated.connect(update_respawn_position)
	
func update_respawn_position(player_key: String, checkpoint_position: Vector2):
	if player_respawn_positions.has(player_key):
		player_respawn_positions[player_key] = checkpoint_position
		return
		
