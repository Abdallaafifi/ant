local ecs = ...
local world = ecs.world
local schema = ecs.schema

schema:type "ik"
	.target "vector"
	.pole_vector "vector"
	.mid_axis "vector"
	.weight "real" (0.0)
	.soften "real" (0.0)
	.twist_angle "real" (0.0)

local m = ecs.component "ik"

function m:init()
	self.start_joint = -1
	self.mid_joint = -1
	self.end_joint = -1
	self.enable = false
end
