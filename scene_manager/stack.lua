local M={}
M.__index = M

function M.new()
	local self = setmetatable({}, M)
	self.stack = {}
	return self
end

-- Pushes a value at the head of the heap
function M:push(value)
	assert(value, "value can't be nil")
	table.insert(self.stack, value) 
end

-- Remove and return the value at the head of the heap
function M:pop() return table.remove(self.stack) end

--Looks at the object of this stack without removing it from the stack.
function M:peek(value)
	value=value or 1
	return self.stack[#self.stack+1-value]
end

-- Clears the heap
function M:clear() self._stack = {} end

-- Checks if the heap is empty
function M:is_empty() 
	return #self.stack == 0 
end

function M:size() return #self.stack end

return M
