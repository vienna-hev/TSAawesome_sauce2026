extends Area2D

@onready var respawn_point: Marker2D = $RespawnPoint

var players = {
	"player1": null,
	"player2": null,
	"player3": null,
	"player4": null
}
	
func _on_body_entered(body: Node2D) -> void:
	var respawn_position: Vector2 = respawn_point.position
	for key in players.keys():
		if body.is_in_group(key):  # If the player is not already in the checkpoint
			RespawnManager.update_respawn_position(key, respawn_position)
			RespawnManager.respawn_positions_updated.emit()
			return  # Exit after updating respawn position for the first matching player
