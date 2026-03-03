# VIENNA YOU ARE SO INCREDIBLY AMAZING I LOVE YOUR YOURE THE BEST AHHHH THANK YOU THUMBS UP SMILEY FACE
#Example comment
extends CharacterBody2D

const SPEED = 150.0
const JUMP_VELOCITY = -350.0
var DJUMP

@onready var animated_sprite_1: AnimatedSprite2D = $AnimatedSprite1


func _physics_process(delta: float) -> void:
	# Add the gravity.
	if is_on_floor():
		DJUMP = 0
	if not is_on_floor():
		velocity += get_gravity() * delta
		animated_sprite_1.animation = "jumping"
		
		
	# Handle jump.
	if Input.is_action_just_pressed("jump1") and is_on_floor():
		velocity.y = JUMP_VELOCITY
	if Input.is_action_just_pressed("jump1") and not is_on_floor():
		if DJUMP == 0:
			velocity.y = JUMP_VELOCITY
			DJUMP = 1

	# Get the input direction and handle the movement/deceleration.
	# As good practice, you should replace UI actions with custom gameplay actions.
	var direction := Input.get_axis("move_left1", "move_right1")
	if direction and is_on_floor():
		velocity.x = direction * SPEED
		animated_sprite_1.animation = "moving"
	else:
		velocity.x = move_toward(velocity.x, 0, SPEED)
		animated_sprite_1.animation = "idle"
		
	if not is_on_floor():
		velocity.x = direction * SPEED
		animated_sprite_1.animation = "jumping"
		
	
	move_and_slide()

	if velocity.x > 0:
		animated_sprite_1.flip_h = true
	if velocity.x < 0:
		animated_sprite_1.flip_h = false
		
func _ready() -> void:
	add_to_group("player1")
