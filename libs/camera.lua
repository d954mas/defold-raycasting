local CONST = require "libs.constants"
local LUME = require "libs.lume"
local M = {}
M.__index = M
local MIN_FOV = 30 * math.pi / 180 
local MAX_FOV = 180 * math.pi / 180 

function M.new()
	local self = setmetatable({}, M)
	self.bounds = vmath.vector4(0, 0, CONST.CAMERA_WIDTH, CONST.CAMREA_HEIGHT)
	self.size = vmath.vector3(self.bounds.z - self.bounds.x, self.bounds.w - self.bounds.y, 0)
	self.center = vmath.vector3(self.bounds.x + self.size.x/2, self.bounds.y + self.size.y/2,0)
	self.rays = self.size.x;
	self.fov = 80 * math.pi / 180 
	self.ray_angle = self.fov / self.rays
	self.angle = 0
	self.position = vmath.vector3(0)
	return self
end

function M:add_to_fov(value)
	self.fov = LUME.clamp(self.fov + value, MIN_FOV, MAX_FOV)
	self.ray_angle = self.fov / self.rays
end	

return M

