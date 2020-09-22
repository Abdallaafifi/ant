local ecs      = import_package "ant.ecs"
local inputmgr = import_package "ant.inputmgr"
local imgui    = require "imgui"

local function create_world(config)
    local rect_w, rect_h = config.width, config.height
    local world = ecs.new_world (config)
    local ev = inputmgr.create(world)
    local irender = world:interface "ant.render|irender"
    irender.create_blit_queue{w=config.width, h=config.height}
    local world_update = world:update_func "update"
    local m = {}
    function m.init()
        world:pub {"resize", rect_w, rect_h}
        world:update_func "init" ()
    end
    function m.update()
        world_update()
        world:clear_removed()
    end
    function m.mouse_wheel(...)
        if not imgui.IO.WantCaptureMouse then
            ev.mouse_wheel(...)
        end
    end
    function m.mouse(...)
        if not imgui.IO.WantCaptureMouse then
            ev.mouse(...)
        end
    end
    function m.keyboard(...)
        if not imgui.IO.WantCaptureKeyboard then
            ev.keyboard(...)
        end
    end
    function m.size(width, height)
        world:pub {"resize", width, height}
    end
    return m, world
end

local worlds = {}

function worlds.create(name)
    return function (config)
        local w, world = create_world(config)
        worlds[#worlds+1] = w
        return w, world
    end
end

return worlds
