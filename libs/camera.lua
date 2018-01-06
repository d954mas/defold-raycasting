local CONST = require "libs.constants"
local M = {}
M.__index = M

function M.new()
	local self = setmetatable({}, M)
	self.bounds = vmath.vector4(1, 1, CONST.CAMERA_WIDTH, CONST.CAMREA_HEIGHT)
	self.strip_size = 1
	self.rays = (self.bounds.z - self.bounds.x + 1) / self.strip_size
	self.fov = 80 * math.pi / 180 
	self.ray_angle = self.fov / self.rays
	
	self.position = vmath.vector3(0)
	self.angle = 0
	return self
end

return M

