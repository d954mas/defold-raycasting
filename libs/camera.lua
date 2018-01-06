local CONST = require "libs.constants"
local LUME = require "libs.lume"
local M = {}
M.__index = M
local MIN_FOV = 30 * math.pi / 180 
local MAX_FOV = 180 * math.pi / 180 

function M.new()
	local self = setmetatable({}, M)
	self.bounds = vmath.vector4(1, 1, CONST.CAMERA_WIDTH, CONST.CAMREA_HEIGHT)
	self.strip_size = 1
	self.rays = M.get_width(self) / self.strip_size
	self.fov = 80 * math.pi / 180 
	self.ray_angle = self.fov / self.rays
	self.position = vmath.vector3(0)
	self.angle = 0
	return self
end

function M:add_to_fov(value)
	self.fov = LUME.clamp(self.fov + value, MIN_FOV, MAX_FOV)
	self.ray_angle = self.fov / self.rays
end	

function M:get_width()
	return self.bounds.z - self.bounds.x + 1
end	

function M:get_height()
	return self.bounds.w - self.bounds.y + 1
end	

return M

