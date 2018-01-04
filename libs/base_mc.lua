local CLASS = require "libs.middleclass"
local LUME = require "libs.lume"

local base_mc = CLASS("BaseMC")

--[[
USED TO CHANGE MODEL CLASSES.
IF SOMEONE NEED HE CAN OBSERVE CHANGES BY MESSAGES
model_controller:registerObserver({model_controller.events.FUEL_CHANGED},msg.url())
BE CAREFULLY WITH SAME EBVENTS NAMES IN DIFFERENT CONTROLLERS
]]

--[[
local events={
		FUEL_CHANGED=hash("fuel_changed"),
		HULL_CHANGED=hash("hull_changed"),
		METALL_CHANGED=hash("metall_changed"),
		RAR_CHANGED=hash("rar_changed"),
		MODULE_BROKE=hash("module_broke"),
		POSITION_CHANGED=hash("position_changed")
	}
	.initialize(self,model,events)
]]--
function base_mc:initialize(model, events)
	assert(model, "model can't be nil")
	assert(events, "events can't be nil")
	self.events = events
	self.model = model
	self.events_observers={}
	for k,v in pairs(self.events) do
		self.events_observers[v] = {}
	end
end

--@section observable
--OBSERVABLE METHODS
function base_mc:registerObserver(events, url)
	assert(events, "events can't be nil")
	assert(url, "url can't be nil")
	for i,v in ipairs(events) do
		table.insert(self.events_observers[v], url)
	end
end

function base_mc:removeObserver(url)
	assert(url, "url can't be nil")
	for i,v in pairs(self.events_observers) do
		LUME.remove(v, url)
	end
end

function base_mc:notify(event, data)
	assert(event, "event can't be nil")
	for i,v in pairs(self.events_observers[event]) do
		if data then msg.post(v, event, data)
		else msg.post(v, event)
		end
	end
end

function base_mc:clear()
	for i,v in pairs(self.events_observers) do
		LUME.clear(v)
	end
end
--END OBSERVABLE METHODS
--@section end

return base_mc
