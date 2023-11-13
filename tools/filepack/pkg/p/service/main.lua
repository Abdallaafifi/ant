local ltask = require "ltask"
local fs = require "bee.filesystem"
local platform = require "bee.platform"
local vfs = require "vfs"
local fastio = require "fastio"
local datalist = require "datalist"
local zip = require "zip"
local vfsrepo = import_package "ant.vfs"
local cr = import_package "ant.compile_resource"

local arg = ...
local repopath = fs.absolute(arg[1]):lexically_normal()
local resource_cache = {}

local config_os
local config_resource
do
    local lpath = vfs.realpath "/resource.settings"
    if lpath then
        local config = datalist.parse(fastio.readall(lpath, "/resource.settings"))
        config_os = config.target or platform.os
        config_resource = config.resource
    else
        config_os = platform.os
    end
    if not config_resource then
        local platform_relates <const> = {
            windows = "direct3d11",
            macos = "metal",
            ios = "metal",
            android = "vulken",
        }
        config_resource = {
            ("%s-%s"):format(config_os, platform_relates[config_os]),
        }
    end
end

do print "step1. check resource cache."
    for _, setting in ipairs(config_resource) do
        if fs.exists(repopath / "res" / setting) then
            for path, status in fs.pairs(repopath / "res" / setting) do
                if status:is_directory() then
                    for res in fs.pairs(path) do
                        resource_cache[res:string()] = true
                    end
                end
            end
        end
    end
end

do print "step2. compile resource."
    local std_vfs <close> = vfsrepo.new_std {
        rootpath = repopath,
        nohash = true,
    }
    local tiny_vfs = vfsrepo.new_tiny(repopath)
    local names, paths = std_vfs:export_resources()
    local tasks = {}
    local function compile_resource(cfg, name, path)
        local lpath = cr.compile_file(cfg, name, path)
        resource_cache[lpath] = nil
    end
    for _, setting in ipairs(config_resource) do
        local cfg = cr.init_setting(tiny_vfs, setting)
        for i = 1, #names do
            tasks[#tasks+1] = { compile_resource, cfg, names[i], paths[i] }
        end
    end
    for _ in ltask.parallel(tasks) do
    end
end

do print "step3. clean resource."
    for path in pairs(resource_cache) do
        fs.remove_all(path)
    end
end

local writer = {}

function writer.zip()
    local zippath = vfs.repopath() .. ".repo.zip"
    fs.remove_all(zippath)
    local zipfile = assert(zip.open(zippath, "w"))
    local m = {}
    function m.writefile(path, content)
        zipfile:add(path, content)
    end
    function m.copyfile(path, localpath)
        zipfile:addfile(path, localpath)
    end
    function m.close()
        zipfile:close()
    end
    return m
end

function writer.dir(rootpath)
    fs.create_directories(rootpath)
    local cache = {}
    for file, status in fs.pairs(rootpath) do
        if status:is_directory() then
            fs.remove_all(file)
        else
            cache[file:filename():string()] = true
        end
    end
    local m = {}
    function m.writefile(path, content)
        local pathobj = rootpath / path
        if path ~= "root" and fs.exists(pathobj) then
            return
        end
        local f <close> = assert(io.open(pathobj:string(), "wb"))
        f:write(content)
        cache[path] = nil
    end
    function m.copyfile(path, localpath)
        fs.copy_file(localpath, rootpath / path, fs.copy_options.skip_existing)
        cache[path] = nil
    end
    function m.close()
        for file in pairs(cache) do
            fs.remove(rootpath / file)
        end
    end
    return m
end

do print "step4. pack file and dir."
    local std_vfs <close> = vfsrepo.new_std {
        rootpath = repopath,
        resource_settings = config_resource,
    }
    local function app_path()
        if config_os == "windows" then
            return fs.path(os.getenv "LOCALAPPDATA")
        elseif config_os == "linux" then
            return fs.path(os.getenv "XDG_DATA_HOME" or (os.getenv "HOME" .. "/.local/share"))
        elseif config_os == "macos" then
            return fs.path(os.getenv "HOME" .. "/Library/Caches")
        else
            error "unknown os"
        end
    end
    local function target_path()
        if config_os == "ios" or config_os == "android" then
            return repopath / ("."..config_os)
        else
            return app_path() / "ant" / ".repo"
        end
    end
    local w = writer.dir(target_path())
    w.writefile("root", std_vfs:root())
    for hash, v in pairs(std_vfs._filehash) do
        if v.dir then
            w.writefile(hash, v.dir)
        else
            w.copyfile(hash, v.path)
        end
    end
    w.close()
end

print "step5. done."
