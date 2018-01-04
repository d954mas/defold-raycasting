local M = {}

M.INPUT_TOUCH = hash("touch")
M.INPUT_ACQUIRE_FOCUS = hash("acquire_input_focus")
M.INPUT_RELEASE_FOCUS = hash("release_input_focus")

M.MSG_PHYSICS_CONTACT = hash("contact_point_response")
M.MSG_PHYSICS_COLLISION = hash("collision_response")
M.MSG_PHYSICS_TRIGGER = hash("trigger_response")
M.MSG_PHYSICS_RAY_CAST= hash("ray_cast_response")

M.MSG_RENDER_CLEAR_COLOR = hash("clear_color")
M.MSG_RENDER_SET_VIEW_PROJECTION = hash("set_view_projection")
M.MSG_RENDER_WINDOW_RESIZED = hash("window_resized")
M.MSG_PLAY_SOUND = hash("play_sound")
M.MSG_ENABLE = hash("enable")
M.MSG_DISABLE = hash("disable")
M.MSG_PLAY_ANIMATION = hash("play_animation")
M.MSG_ACQUIRE_CAMERA_FOCUS = hash("acquire_camera_focus")


M.RGB = hash("rgb")


return M
