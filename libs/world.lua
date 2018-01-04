local MAP = require "libs.map"

local M = {}

M.map = MAP
M.player = {position = vmath.vector3(12, 7, 0), angle = 0}

return M