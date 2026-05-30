@tool
extends Node3D

@export_enum("Node", "Object3D", "Platform", "MovingPlatform", "BreakableWall", "Trap", "SpikePlatform", "Light", "Object2D", "Drone", "ChargingEnemy", "ShieldTankEnemy", "TurretEnemy", "Bullet") var class_type: String = "Platform":
	set(v):
		if class_type != v:
			class_type = v
			notify_property_list_changed()
			_update_metadata()

@export_group("Silnik - Główne Ustawienia")
@export var model_name: String = "":
	set(v): model_name = v; _update_metadata()
@export var sprite_name: String = "":
	set(v): sprite_name = v; _update_metadata()
	
@export var enable_input: bool = true:
	set(v): enable_input = v; _update_metadata()
@export var enable_physics: bool = true:
	set(v): enable_physics = v; _update_metadata()
@export var enable_draw: bool = true:
	set(v): enable_draw = v; _update_metadata()
	
@export var is_static: bool = true:
	set(v): is_static = v; _update_metadata()
@export var ignore_parent: bool = true:
	set(v): ignore_parent = v; _update_metadata()
@export var req_perspective: bool = true:
	set(v): req_perspective = v; _update_metadata()
	
@export var cull: float = 0.5:
	set(v): cull = v; _update_metadata()
@export var z_index: int = 0:
	set(v): z_index = v; _update_metadata()
@export var shader: String = "standard":
	set(v): shader = v; _update_metadata()

@export_group("Silnik - Collider")
@export var add_collider: bool = true:
	set(v): add_collider = v; _update_metadata()
@export var resolve_collision: bool = true:
	set(v): resolve_collision = v; _update_metadata()
	
@export var collider_type: bool = false:
	set(v): 
		if collider_type != v:
			collider_type = v
			notify_property_list_changed()
			_update_metadata()
		
@export var collider_pos_x: float = 0.0:
	set(v): collider_pos_x = v; _update_metadata()
@export var collider_pos_y: float = 0.0:
	set(v): collider_pos_y = v; _update_metadata()
	
@export var width: float = 1.0:
	set(v): width = v; _update_metadata()
@export var height: float = 1.0:
	set(v): height = v; _update_metadata()
@export var radius: float = 0.2:
	set(v): radius = v; _update_metadata()

@export_group("Silnik - Ruchoma Platforma")
@export var moving_duration: float = 2.0:
	set(v): moving_duration = v; _update_metadata()
@export var stop_duration: float = 1.0:
	set(v): stop_duration = v; _update_metadata()
@export var x_end_position: float = 0.0:
	set(v): x_end_position = v; _update_metadata()
@export var y_end_position: float = 0.0:
	set(v): y_end_position = v; _update_metadata()

@export_group("Silnik - Kolce (Spike Platform)")
@export var extend_time: float = 0.1:
	set(v): extend_time = v; _update_metadata()
@export var retract_time: float = 1.0:
	set(v): retract_time = v; _update_metadata()
@export var extended_time: float = 0.2:
	set(v): extended_time = v; _update_metadata()
@export var retracted_time: float = 1.0:
	set(v): retracted_time = v; _update_metadata()
@export var up_down: bool = false:
	set(v): up_down = v; _update_metadata()
@export var retracted_pos: float = 0.0:
	set(v): retracted_pos = v; _update_metadata()
@export var extended_pos: float = 0.3:
	set(v): extended_pos = v; _update_metadata()

@export_group("Silnik - Przeciwnik: Dron (Drone)")
@export var direction: int = -1:
	set(v): direction = v; _update_metadata()
@export var patrol_speed: float = 1.0:
	set(v): patrol_speed = v; _update_metadata()
@export var patrol_distance: float = 6.0:
	set(v): patrol_distance = v; _update_metadata()
@export var dive_speed: float = 2.0:
	set(v): dive_speed = v; _update_metadata()
@export var vision_range: float = 4.0:
	set(v): vision_range = v; _update_metadata()
@export var vision_angle: float = 60.0:
	set(v): vision_angle = v; _update_metadata()
@export var explosion_radius: float = 1.0:
	set(v): explosion_radius = v; _update_metadata()
@export var explosion_damage: float = 100.0:
	set(v): explosion_damage = v; _update_metadata()
@export var turn_speed: float = 4.5:
	set(v): turn_speed = v; _update_metadata()

@export_group("Silnik - Światło (Light)")
@export var manual_light_direction: bool = false:
	set(v): manual_light_direction = v; _update_metadata()
@export var light_type_int: int = 0:
	set(v): light_type_int = v; _update_metadata()
@export var color_ambient: Vector3 = Vector3(0.5, 1.0, 0.5):
	set(v): color_ambient = v; _update_metadata()
@export var color_diffuse: Vector3 = Vector3(0.5, 1.0, 0.5):
	set(v): color_diffuse = v; _update_metadata()
@export var color_specular: Vector3 = Vector3(0.5, 1.0, 0.5):
	set(v): color_specular = v; _update_metadata()
@export var data1: Vector3 = Vector3(-0.58, -0.58, -0.58):
	set(v): data1 = v; _update_metadata()
@export var data2: Vector3 = Vector3(0.0, 0.0, 0.0):
	set(v): data2 = v; _update_metadata()
@export var data3: Vector3 = Vector3(0.0, 0.0, 0.0):
	set(v): data3 = v; _update_metadata()
@export var data4: float = 0.0:
	set(v): data4 = v; _update_metadata()

func _validate_property(property: Dictionary):
	var prop = property.name
	var classes_2d = ["Object2D", "Drone", "ChargingEnemy", "ShieldTankEnemy", "TurretEnemy", "Bullet"]
	var classes_3d = ["Object3D", "Platform", "MovingPlatform", "BreakableWall", "Trap", "SpikePlatform"]
	
	if prop in ["sprite_name", "req_perspective"]:
		if not class_type in classes_2d: property.usage = PROPERTY_USAGE_NO_EDITOR
	elif prop == "model_name":
		if not class_type in classes_3d: property.usage = PROPERTY_USAGE_NO_EDITOR
		
	elif prop == "width":
		if collider_type == true or class_type in ["Light", "Node"]: property.usage = PROPERTY_USAGE_NO_EDITOR
	elif prop == "radius":
		if collider_type == false or class_type in ["Light", "Node"]: property.usage = PROPERTY_USAGE_NO_EDITOR
		
	elif prop in ["moving_duration", "stop_duration", "x_end_position", "y_end_position"]:
		if class_type != "MovingPlatform": property.usage = PROPERTY_USAGE_NO_EDITOR
	elif prop in ["extend_time", "retract_time", "extended_time", "retracted_time", "up_down", "retracted_pos", "extended_pos"]:
		if class_type != "SpikePlatform": property.usage = PROPERTY_USAGE_NO_EDITOR
	elif prop in ["direction", "patrol_speed", "patrol_distance", "dive_speed", "vision_range", "vision_angle", "explosion_radius", "explosion_damage", "turn_speed"]:
		if class_type != "Drone": property.usage = PROPERTY_USAGE_NO_EDITOR
	elif prop in ["light_type_int", "color_ambient", "color_diffuse", "color_specular", "data1", "data2", "data3", "data4"]:
		if class_type != "Light": property.usage = PROPERTY_USAGE_NO_EDITOR
	elif prop in ["height", "collider_type", "collider_pos_x", "collider_pos_y", "add_collider", "resolve_collision", "shader", "cull", "z_index", "is_static", "ignore_parent", "enable_input", "enable_physics", "enable_draw"]:
		if class_type in ["Light", "Node"]: property.usage = PROPERTY_USAGE_NO_EDITOR

func _notification(what):
	if Engine.is_editor_hint() and what == NOTIFICATION_TRANSFORM_CHANGED:
		_update_metadata()

func _ready():
	set_notify_transform(true)
	_update_metadata()

func _update_metadata():
	for m in get_meta_list():
		remove_meta(m)

	set_meta("class", class_type)
	set_meta("process", true)

	if class_type == "Node":
		set_meta("input", false)
		set_meta("physics", false)
		return

	if class_type != "Light":
		set_meta("input", enable_input)
		set_meta("physics", enable_physics)
		set_meta("draw", enable_draw)
		set_meta("ignoreParent", ignore_parent)
		set_meta("static", is_static)
		set_meta("addCollider", add_collider)
		set_meta("ResolveCollision", resolve_collision)
		
		set_meta("colliderPosX", collider_pos_x)
		set_meta("colliderPosY", collider_pos_y)
		set_meta("colliderType", collider_type)
		set_meta("cull", cull)
		set_meta("zIndex", z_index)
		set_meta("shader", shader)
		
		var classes_2d = ["Object2D", "Drone", "ChargingEnemy", "ShieldTankEnemy", "TurretEnemy", "Bullet"]
		if class_type in classes_2d:
			var s_name = str(sprite_name)
			if s_name == "":
				s_name = str(name)
				while s_name.length() > 0 and s_name.substr(s_name.length() - 1, 1).is_valid_int():
					s_name = s_name.left(s_name.length() - 1)
			set_meta("sprite", s_name)
			set_meta("reqPerspective", req_perspective)
		else:
			var m_name = str(model_name)
			if m_name == "":
				m_name = str(name)
				while m_name.length() > 0 and m_name.substr(m_name.length() - 1, 1).is_valid_int():
					m_name = m_name.left(m_name.length() - 1)
			set_meta("model", m_name)

		set_meta("height", height)
		if collider_type == true:
			set_meta("radius", radius)
		else:
			set_meta("width", width)

		if class_type == "MovingPlatform":
			set_meta("movingDuration", moving_duration)
			set_meta("stopDuration", stop_duration)
			set_meta("XendPosition", x_end_position)
			set_meta("YendPosition", y_end_position)
		elif class_type == "SpikePlatform":
			set_meta("extendTime", extend_time)
			set_meta("retractTime", retract_time)
			set_meta("extendedTime", extended_time)
			set_meta("retractedTime", retracted_time)
			set_meta("upDown", up_down)
			set_meta("retractedPos", retracted_pos)
			set_meta("extendedPos", extended_pos)
		elif class_type == "Drone":
			set_meta("direction", direction)
			set_meta("patrolSpeed", patrol_speed)
			set_meta("patrolDistance", patrol_distance)
			set_meta("diveSpeed", dive_speed)
			set_meta("visionRange", vision_range)
			set_meta("visionAngle", vision_angle)
			set_meta("explosionRadius", explosion_radius)
			set_meta("explosionDamage", explosion_damage)
			set_meta("turnSpeed", turn_speed)
			
	elif class_type == "Light":
		set_meta("input", true)
		set_meta("physics", true)
		set_meta("type", light_type_int)
		
		var out_data1 = data1
		var out_data2 = data2
		
		if Engine.is_editor_hint() and not manual_light_direction:
			if light_type_int == 0:
				out_data1 = -transform.basis.z.normalized()
			elif light_type_int == 1:
				out_data1 = position
			elif light_type_int == 2:
				out_data1 = position
				out_data2 = -transform.basis.z.normalized()
			
		set_meta("colorAmbient", [ color_ambient.x, color_ambient.y, color_ambient.z ])
		set_meta("colorDiffuse", [ color_diffuse.x, color_diffuse.y, color_diffuse.z ])
		set_meta("colorSpecular", [ color_specular.x, color_specular.y, color_specular.z ])
		set_meta("data1", [ out_data1.x, out_data1.y, out_data1.z ])
		set_meta("data2", [ out_data2.x, out_data2.y, out_data2.z ])
		set_meta("data3", [ data3.x, data3.y, data3.z ])
		set_meta("data4", data4)
