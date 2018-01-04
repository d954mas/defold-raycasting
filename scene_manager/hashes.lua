local M = {}

M.MSG_SHOW = hash("show")
M.MSG_BACK = hash("back")
M.MSG_CLEAR_STACK = hash("clear_stack")


M.MSG_INIT = hash("init")
M.MSG_DISABLE = hash("disable")
M.MSG_ENABLE = hash("enable")
M.MSG_FINAL = hash("final")
M.MSG_UNLOAD = hash("unload")
M.MSG_PROXY_LOADED = hash("proxy_loaded")
M.MSG_RELEASE_INPUT= hash("release_input_focus")
M.MSG_ACQUIRE_INPUT= hash("acquire_input_focus")
M.MSG_SEND_INPUT = hash("scene_send_input")

M.METHOD_PUSH = hash("push")

return M
