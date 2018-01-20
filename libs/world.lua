local MAP = require "libs.map"
local Camera = require "libs.camera"

local M = {}

M.map = MAP
M.player = {position = vmath.vector3(18, 17, 0), angle = 0, camera = Camera.new()}

function M.update_camera()
	M.player.camera.position = M.player.position
	M.player.camera.angle = M.player.angle
end	

return M