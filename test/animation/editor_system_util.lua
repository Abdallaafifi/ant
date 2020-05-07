local mathpkg   = import_package "ant.math"
local mu = mathpkg.util
local math3d = require "math3d"
local assetmgr = import_package "ant.asset"
local RES_IDX = 10080

local renderpkg = import_package "ant.render"
local computil = renderpkg.components

local function euler2quat(euler)
    return math3d.tovalue(math3d.quaternion(euler))
end

local function line(start_pos, end_pos, color)
    local vb, ib = {}, {}
    local function add_vertex(pos, clr)
        local x,y,z = table.unpack(pos)
        table.insert(vb, {x,y,z, clr})
    end

    local function add_line(p1, p2, color)
        add_vertex(p1, color)
        add_vertex(p2, color)
        -- call 2 times
        table.insert(ib, #ib)
        table.insert(ib, #ib)
    end

    -- center lines
    add_line(start_pos, end_pos, color)

    return vb, ib
end


local function circle(color)
    local vb, ib = {}, {}      
    local radius = 1 
    local function add_vertex(pos, clr)
        local x,y,z = table.unpack(pos)
        table.insert(vb, {x,y,z, clr})          
    end
    local function add_line(p1, p2, color)
        add_vertex(p1, color)
        add_vertex(p2, color)
        -- call 2 times
        table.insert(ib, #ib)
        table.insert(ib, #ib)
    end
    local start_p = {0,radius,0}
    for i = 1,360 do
        local r = i/180*math.pi
        local cos = radius*math.cos(r)
        local sin = radius*math.sin(r)
        local p = {0,cos,sin}
        add_line(start_p, p, color)
        start_p = p
    end
    return vb, ib
end

local function create_ring_entity(world,color,size,rot,name,parent,dir)
    color[4] = 0.6
    return world:create_entity {
        policy={
            "ant.general|name",
            "ant.render|mesh",
            "ant.render|render",
            "ant.scene|hierarchy_policy",
            "ant.objcontroller|select",
            "ant.test.animation|gizmo_object",
        },
        data = {
            transform = computil.create_transform(world,{
                srt = {s=size or {1, 1, 1}, r=euler2quat(rot or {0, 0, 0})},
            }),
            mesh = world.component:resource "/pkg/ant.resources/meshes/ring.mesh:scenes.scene1.mesh0.1",
            material = world.component:resource (([[
                ---
                /pkg/ant.resources/materials/gizmo_front_singlecolor.material
                ---
                op: replace
                path: /properties/uniforms/u_color
                value:
                    type:v4
                    value:
                      {%f, %f, %f, %f}
            ]]):format(color[1], color[2], color[3], color[4])),
            --can_cast = true,
            can_render = true,
            name = name,
            can_select = true,
            gizmo_object = {dir = dir},
            scene_entity = true,
        },
        connection = {
            {"mount", parent}
        }
    }
end

local function create_line_entity(world, name, start_pos,end_pos,color,parent,dir)
    local util  = import_package "ant.render".components
    local gridid = world:create_entity {
        policy = {
            "ant.general|name",
            "ant.render|render",
            "ant.scene|hierarchy_policy",
            "ant.objcontroller|select",
            "ant.test.animation|gizmo_object",
        },
        data = {
            transform = computil.create_transform(world),
            material = world.component:resource "/pkg/ant.resources/materials/gizmo_line.material",
            name = name,
            can_render = true,
            can_select = true,
            gizmo_object = {dir = dir},
            scene_entity = true,
        },
        connection = {
            {"mount", parent}
        }
    }
    local vb, ib = line(start_pos, end_pos, color)
    local gvb = {"fffd"}
    for _, v in ipairs(vb) do
        for _, vv in ipairs(v) do
            table.insert(gvb, vv)
        end
    end

    local num_vertices = #vb
    local num_indices = #ib

    local filename = string.format("//res.mesh/line_%s.rendermesh",RES_IDX)
    RES_IDX = RES_IDX + 1
    world:add_component(gridid, "rendermesh", assetmgr.load(filename, util.create_simple_mesh( "p3|c40niu", gvb, num_vertices, ib, num_indices)))
    return gridid
end

local function create_circle_entity(world, name,color,rot,parent,dir)
    local util  = import_package "ant.render".components

    local gridid = world:create_entity {
        policy = {
            "ant.general|name",
            "ant.render|render",
            "ant.scene|hierarchy_policy",
            "ant.objcontroller|select",
            "ant.test.animation|gizmo_object",
        },
        data = {
            transform = computil.create_transform(world,{
                srt = {r = euler2quat(rot or {0, 0, 0})},
            }),
            material = world.component:resource "/pkg/ant.resources/materials/gizmo_front_line.material",
            name = name,
            can_render = true,
            can_select = true,
            gizmo_object = {dir = dir},
            scene_entity = true,
        },
        connection = {
            {"mount", parent}
        }
    }
    local vb, ib = circle(color)
    local gvb = {"fffd"}
    for _, v in ipairs(vb) do
        for _, vv in ipairs(v) do
            table.insert(gvb, vv)
        end
    end

    local num_vertices = #vb
    local num_indices = #ib

    local filename = string.format("//res.mesh/circle_%s.rendermesh",RES_IDX)
    RES_IDX = RES_IDX + 1
    world:add_component(gridid, "rendermesh", assetmgr.load(filename, util.create_simple_mesh( "p3|c40niu", gvb, num_vertices, ib, num_indices)))
    return gridid
end

local function create_cone_entity(world, color, size,rot,pos, name,parent,dir)
    return world:create_entity {
        policy = {
            "ant.general|name",
            "ant.render|render",
            "ant.render|mesh",
            "ant.scene|hierarchy_policy",
            "ant.objcontroller|select",
            "ant.test.animation|gizmo_object",
        },
        data = {
            transform = computil.create_transform(world,{
                srt = {s=size, r=euler2quat(rot or {0, 0, 0}), t=pos,}
            }),
            mesh = world.component:resource"/pkg/ant.resources/meshes/cone.mesh:scenes.scene1.pCone1.1",
            material = world.component:resource (([[
                ---
                /pkg/ant.resources/materials/gizmo_singlecolor.material
                ---
                op:replace
                path:/properties/uniforms/u_color
                value:
                    type:v4
                    value:
                      {%f,%f,%f,%f}
            ]]):format(color[1], color[2], color[3], color[3])),
            can_render = true,
            can_select = true,
            name = name,
            gizmo_object = {dir=dir},
            scene_entity = true,
        },
        connection = {
            {"mount", parent}
        }
    }
end

local function create_box_entity(world, color, size, pos, name,parent,dir)
    return world:create_entity {
        policy = {
            "ant.general|name",
            "ant.scene|hierarchy_policy",
            "ant.render|render",
            "ant.render|mesh",
            "ant.objcontroller|select",
            "ant.test.animation|gizmo_object",
        },
        data = {
            transform = computil.create_transform(world,{
                srt = {s=size, t=pos},
            }),
            mesh = world.component:resource "/pkg/ant.resources/meshes/cube.mesh:scenes.scene1.pCube1.1",
            material = world.component:resource (([[
                ---
                /pkg/ant.resources/materials/gizmo_singlecolor.material
                ---
                op:replace
                path:/properties/uniforms/u_color
                value:
                    type:v4
                    value:
                      {%f, %f, %f,%f}
            ]]):format(color[1], color[2], color[3], color[4])),
            can_render = true,
            name = name,
            can_select = true,
            gizmo_object = {dir=dir},
            scene_entity = true,
        },
        connection = {
            {"mount", parent}
        }
    }
end

local Util = {}
function Util.create_gizmo(world)
    local function create_gizmo_object(name,parent)
        local trans = computil.create_transform(world)
        local args = {
            policy={
                "ant.general|name",
                "ant.scene|hierarchy_policy",
                "ant.test.animation|gizmo_object",
            },
            data={
                transform = trans,
                name = name,
                gizmo_object = {},
            },
            connection = {
                {"mount", parent}
            }
        }

        if parent then
            args.policy[#args.policy+1] = "ant.scene|hierarchy_policy"
        end

        return world:create_entity(args)
    end
    local root = create_gizmo_object("gizmo",nil)
    local result = {eid = root}

    --
    do
        local line_length = 1.3
        local position = {}
        result.position = position
        position.line_length = line_length
        local parent = create_gizmo_object("position",root)
        position.eid = parent
        position.line_x = create_line_entity(world,"line_x",{0,0,0},{line_length,0,0},0xff0000ff,parent,"x")
        position.cone_x = create_cone_entity(world,{1,0,0,1},{0.1,0.13,0.1},{0,0,-0.5*math.pi,0}, {line_length,0,0}, "cone_x",parent,"x")
        position.line_y = create_line_entity(world,"line_y",{0,0,0},{0,line_length,0},0xff00ff00,parent,"y")
        position.cone_y = create_cone_entity(world,{0,1,0,1},{0.1,0.13,0.1},{0,0,0},{0,line_length,0}, "cone_y",parent,"y")
        position.line_z = create_line_entity(world,"line_z",{0,0,0},{0,0,line_length},0xffff0000,parent,"z")
        position.cone_z = create_cone_entity(world,{0,0,1,1},{0.1,0.13,0.1},{0.5*math.pi,0,0}, {0,0,line_length,1}, "cone_z",parent,"z")
        position.center = create_box_entity(world,{1,1,1,1},{0.15,0.15,0.15,0}, {0,0,0,1}, "box_o",parent)
    end

    do
        local line_length = 1.3
        local scale = {}
        result.scale = scale
        scale.line_length = line_length
        local parent = create_gizmo_object("scale",root)
        scale.eid = parent
        scale.line_x = create_line_entity(world,"line_x",{0,0,0},{line_length,0,0},0xff0000ff,parent,"x")
        scale.box_x = create_box_entity(world,{1,0,0,1},{0.15,0.15,0.15}, {line_length,0,0}, "box_x",parent,"x")
        scale.line_y = create_line_entity(world,"line_y",{0,0,0},{0,line_length,0},0xff00ff00,parent,"y")
        scale.box_y = create_box_entity(world,{0,1,0,1},{0.15,0.15,0.15},{0,line_length,0}, "box_y",parent,"y")
        scale.line_z = create_line_entity(world,"line_z",{0,0,0},{0,0,line_length},0xffff0000,parent,"z")
        scale.box_z = create_box_entity(world,{0,0,1,1},{0.15,0.15,0.15}, {0,0,line_length}, "box_z",parent,"z")
        scale.center = create_box_entity(world,{1,1,1,1},{0.18,0.18,0.18}, {0,0,0}, "box_o",parent)
    end
    do
        local rotation = {}
        result.rotation = rotation
        local parent = create_gizmo_object("rotation",root)
        rotation.eid = parent
        rotation.line_x = create_circle_entity(world,"line_x",0xff0000ff,{0,0,0},parent,"x")
        rotation.ring_x = create_ring_entity(world,{1,0,0,1},{1,1,1},{0,0,0.5*math.pi}, "cylinder_x",parent,"x")
        rotation.line_y = create_circle_entity(world,"line_y",0xff00ff00,{0,0,0.5*math.pi},parent,"y")
        rotation.ring_y = create_ring_entity(world,{0,1,0,1},{1,1,1},{0,0,0}, "cylinder_y",parent,"y")
        rotation.line_z = create_circle_entity(world,"line_z",0xffff0000,{0,-0.5*math.pi,0},parent,"z")
        rotation.ring_z = create_ring_entity(world,{0,0,1,1},{1,1,1},{0.5*math.pi,0, 0}, "cylinder_z",parent,"z")
    end
    return result
end

local function project_screen_onto_plane(screen_pos,plane_point,plane_normal,view_proj)
    screen_pos[3] = 0
    local ray_origin = math3d.transform(view_proj, screen_pos, 1)
    screen_pos[3] = 1.0
    local ray_end   = math3d.transform(view_proj, screen_pos, 1)

    local ray_normal = math3d.normalize(math3d.sub(ray_end, ray_origin))
    local b = math3d.dot(ray_normal, plane_normal)
    local a = math3d.dot(plane_normal, math3d.sub(plane_point, ray_origin))
    return math3d.muladd(ray_normal, a / b, ray_origin)
end

function Util.mouse_project_onto_plane(world,mouse_pos,plane_point,plane_normal)
    local mq = world:singleton_entity "main_queue"
    local viewport = mq.render_target.viewport
    local win_w,win_h = viewport.rect.w,viewport.rect.h
    local screen_pos = {2.0*mouse_pos[1]/win_w-1, 1.0-2.0*mouse_pos[2]/win_h}

    local camera = world[mq.camera_eid].camera
    local view_proj = mu.view_proj(camera)
    local p = project_screen_onto_plane(screen_pos,plane_point,plane_normal,view_proj)
    return p
end

return Util
