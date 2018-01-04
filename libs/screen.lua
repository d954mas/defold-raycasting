local CONST = require "libs.constants"
local LUME = require "libs.lume"
local M = {}

local EMPTY_COLOR = 0x000000

M.WIDTH = CONST.WINDOW_WIDTH
M.HEIGHT = CONST.WINDOW_HEIGHT
M.BUFFER = buffer.create(M.WIDTH * M.HEIGHT, { {name=hash("rgb"), type=buffer.VALUE_TYPE_UINT8, count=3} } )
M.NODES = {}

function M.clear()
	for id = 1, M.WIDTH * M.HEIGHT do
		M.NODES[id] = EMPTY_COLOR
	end	
end
M.clear()

function M.get_node(x, y)
	return M.NODES[(y-1) * M.WIDTH + x]
end	

function M.set_node(x, y, color)
	assert(color~=nil, "color can't be nil")
	if x > 0 and x <= M.WIDTH and y > 0 and y <= M.HEIGHT then
		M.NODES[(y-1) * M.WIDTH + x] =  color or COLORS.EMPTY
	end	
end	