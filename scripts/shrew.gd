extends Node2D

const SPEED = 200
var DIRECTION = 1

@onready var animated_sprite_2d: AnimatedSprite2D = $AnimatedSprite2D

@onready var left_ray_cast: RayCast2D = $"Left RayCast"
@onready var right_ray_cast: RayCast2D = $"Right RayCast"

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if left_ray_cast.is_colliding():
		DIRECTION = 1 
		animated_sprite_2d.flip_h = true
	if right_ray_cast.is_colliding():
		DIRECTION =  -1   
		animated_sprite_2d.flip_h = false
		
	position.x += DIRECTION * SPEED * delta
	
