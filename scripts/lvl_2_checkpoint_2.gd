extends Area2D

@onready var marker_2d: Marker2D = $Marker2D

var players = {
	"player1": null,
	"player2": null,
	"player3": null,
	"player4": null
}
	
func _on_body_entered(body: Node2D) -> void:
	var respawn_position: Vector2 = marker_2d.position
	for key in players.keys():
		if body.is_in_group(key):  # If the player is not already in the checkpoint
			RespawnManager.update_respawn_position(key, Vector2(1500,1300))
			RespawnManager.respawn_positions_updated.emit()
			return  # Exit after updating respawn position for the first matching player
