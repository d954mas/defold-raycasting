local CONST = require "libs.constants"
local LUME = require "libs.lume"
local M = {}

local EMPTY_COLOR = 0x000000

M.WIDTH = CONST.WINDOW_WIDTH
M.HEIGHT = CONST.WINDOW_HEIGHT
M.BUFFER = buffer.create(M.WIDTH * M.HEIGHT, { {name=hash("rgb"), type=buffer.VALUE_TYPE_UINT8, count=3} } )

native_raycasting.set_buffer(M.WIDTH, M.HEIGHT, M.BUFFER)
native_raycasting.update_plane(0, 0, M.WIDTH, M.HEIGHT)
native_raycasting.set_camera_fov(CONST.FOV)
return M