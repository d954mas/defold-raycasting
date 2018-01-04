local HASHES = require "scene_manager.hashes"
local M = {}
M.scenes = {}
local registered = false

function M.show(name, input, options)
	msg.post(M.router_url, HASHES.MSG_SHOW, {name = name, input = input, options = options})
end

function M.back()
	msg.post(M.router_url, HASHES.MSG_BACK)
end

--REGISTER SCENES
--check scene for bad values, return scene
local function check_scene(scene)
	assert(scene.name, "scene name can't be nil")
	assert(scene.url, "scene url can't be nil")
	assert(not M.scenes[scene.name], "scene " .. scene.name .. " already exist")
	--if(scene.controller == nil) then scene.controller = scene.url end
	return scene
end	

function M.register_scenes(scenes)
	assert(not registered, "register_scenes can be called only once")
	assert(scenes, "scenes can't be nil")
	assert(#scenes ~= 0, "scenes should have one or more scene")
	registered = true
	for _, scene in ipairs(scenes) do
		M.scenes[scene.name] = check_scene(scene)	
	end	
end

return M