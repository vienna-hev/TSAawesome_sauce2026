extends Camera2D

@export var move_speed = 1  # camera position lerp speed
@export var zoom_speed = .50 # camera zoom lerp speed
@export var min_zoom = 1  # camera won't zoom closer than this
@export var max_zoom = 2.42  # camera won't zoom farther than this
@export var margin = Vector2(400, 200)  # include some buffer area around target

var targets = []

@onready var screen_size = get_viewport_rect().size

func add_target(t):
	if not t in targets:
		targets.append(t)
# Called every frame. 'delta' is the elapsed time since the previous frame.
func remove_target(t):
	if t in targets:
		targets.erase(t)


func _process(_delta):
	if !targets:
		return

	var min2 = Vector2(targets[0].position);
	var max2 = Vector2(targets[0].position);
	for target in targets:
		if target.position.x > max2.x:
			max2.x = target.position.x
		if target.position.x < min2.x:
			min2.x = target.position.x
		if target.position.y > max2.y:
			max2.y = target.position.y
		if target.position.y < min2.y:
			min2.y = target.position.y
	var p = (max2 + min2)/2
	position = lerp(position, p, move_speed)
	
	var r = Rect2(position, Vector2.ONE)
	var min = Vector2(targets[0].position);
	var max = Vector2(targets[0].position);
	for target in targets:
		if target.position.x > max.x:
			max.x = target.position.x
		if target.position.x < min.x:
			min.x = target.position.x
		if target.position.y > max.y:
			max.y = target.position.y
		if target.position.y < min.y:
			min.y = target.position.y
	r = r.expand(min)
	r = r.expand(max)
	r = r.grow_individual(margin.x, margin.y, margin.x, margin.y)
	var z
	if r.size.x > r.size.y * screen_size.aspect():
		z = clamp(r.size.x / screen_size.x, min_zoom, max_zoom)
	else:
		z = clamp(r.size.y / screen_size.y, min_zoom, max_zoom)
	zoom = lerp(zoom, Vector2.ONE / z, zoom_speed)
