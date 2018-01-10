--http://lodev.org/cgtutor/raycasting.html
local CONST = require("libs.constants")
local M = {}

function M.cast_rays(camera, wall_cells, fun, go_self)
	--local time = os.clock()
	local half_fov = camera.fov / 2
	local ray_angle = - half_fov
	if fun then
		for i=1 , camera.rays do
			local perp_dist, catet_x, catet_y,  map_x, map_y, texture_x = native_raycasting.cast_ray(camera.position.x, camera.position.y, camera.angle, ray_angle)
			if fun then fun(go_self, camera, wall_cells, perp_dist, catet_x, catet_y,  map_x, map_y, side, i, texture_x) end
			ray_angle = ray_angle + camera.ray_angle
		end	
	else
		native_raycasting.cast_rays(camera.position.x, camera.position.y, camera.angle,  camera.fov)
	end
	--print("native cast_rays:" .. os.clock() - time)
end

return M