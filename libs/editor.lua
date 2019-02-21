package.path = table.concat({
    "libs/?.lua",
    "libs/?/?.lua",
}, ";")

package.cpath = table.concat({
	"projects/msvc/vs_bin/x64/Debug/?.dll",
    "clibs/?.dll",
	"bin/?.dll",
}, ";")

require "editor.vfs"

require "common.log"
import_package = (require "antpm").import

local print_func = require "common.print_r"
print_r = print_func.print_r
print_a = print_func.print_a
function dprint(...) print(...) end
