--http://lodev.org/cgtutor/raycasting.html
local CONST = require("libs.constants")
local M = {}

local sin = math.sin
local cos = math.cos
local floor = math.floor
local ceil = math.ceil
local abs = math.abs

M.half_fov = CONST.PLAYER_FOV/2
M.twoPI = math.pi * 2;
M.ray_angle = CONST.PLAYER_FOV/CONST.PLAYER_RAYS
--count sx and step. same logic for x and y
local function count_sx_and_step(dx, map_x, x, abs_dx)
	local sx, step_x
	if dx>0 then
		sx = (map_x - x) * abs_dx
		step_x = 1
	else
		sx = (1 + x  - map_x) * abs_dx
		step_x = - 1
	end
	return sx, step_x
end
	
local function find_ray_intersept(start_x, start_y, ray_angle, angle, cells)
	local map_x = ceil(start_x)
	local map_y = ceil(start_y)
    
    local angle_sin=sin(angle)
    local angle_cos=cos(angle)
    
	local dx = 1/angle_sin
	local dy = 1/angle_cos
	
	local abs_dx = abs(dx)
	local abs_dy = abs(dy)
	
	local sx, step_x = count_sx_and_step(dx, map_x, start_x, abs_dx)
	local sy, step_y = count_sx_and_step(dy, map_y, start_y, abs_dy)
      
    local move_x, move_y = 0 , 0
    local hit_x = true
    while true do
    	if(sx < sy) then
			map_x = map_x + step_x
			sx = sx + abs_dx
			hit_x = true
		else
			map_y = map_y + step_y
			sy = sy + abs_dy
			hit_x = false
		end
		local cell = cells[map_y][map_x]
		if cell > 0 then
			local dist, catet_x, catet_y, perp_dist, texture_x
			if(hit_x) then
				sx = sx - abs_dx --remove last dx
				dist = sx
		    else
				sy = sy - abs_dy --remove last dy
				dist = sy
			end
			catet_x = dist * angle_sin
			catet_y = dist * angle_cos
			texture_x = hit_x and (catet_y % 1) or (catet_x % 1) 
			perp_dist = dist *  math.cos(ray_angle)
			return dist, start_x + catet_x , start_y + catet_y, cell, hit_x, perp_dist, texture_x
		end	
	end	
end


function M.cast_rays(player, wall_cells, fun, go_self)
	for i=1 , CONST.PLAYER_RAYS do
		local ray_angle = M.ray_angle * (i-1) - M.half_fov
		local dist, x, y, cell, hit_x, perp_dist, texture_x = find_ray_intersept(player.position.x, player.position.y, ray_angle, player.angle + ray_angle, wall_cells)
		if fun then fun(go_self, dist, x, y, cell, i, hit_x, perp_dist, texture_x) end
	end
	return wall_cells
end

return M