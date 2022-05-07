local math3d = require "math3d"

local constant = {}
-- color
constant.RED            = math3d.constant("v4", {1.0, 0.0, 0.0, 1.0})
constant.GREEN          = math3d.constant("v4", {0.0, 1.0, 0.0, 1.0})
constant.BLUE           = math3d.constant("v4", {0.0, 0.0, 1.0, 1.0})
constant.BLACK          = math3d.constant("v4", {0.0, 0.0, 0.0, 1.0})
constant.WHITE          = math3d.constant("v4", {1.0, 1.0, 1.0, 1.0})
constant.YELLOW         = math3d.constant("v4", {1.0, 1.0, 0.0, 1.0})
constant.YELLOW_HALF    = math3d.constant("v4", {0.5, 0.5, 0.0, 1.0})
constant.GRAY_HALF      = math3d.constant("v4", {0.5, 0.5, 0.5, 1.0})
constant.GRAY           = math3d.constant("v4", {0.8, 0.8, 0.8, 1.0})
constant.DEFAULT_INTENSITY = 1

-- value
constant.T_ZERO     = {0, 0, 0, 0}
constant.T_ZERO_PT  = {0, 0, 0, 1}

constant.T_ONE      = {1, 1, 1, 0}
constant.T_ONE_PT   = {1, 1, 1, 1}

constant.T_XAXIS    = {1, 0, 0, 0}
constant.T_NXAXIS   = {-1, 0, 0, 0}

constant.T_YAXIS    = {0, 1, 0, 0}
constant.T_NYAXIS   = {0, -1, 0, 0}

constant.T_ZAXIS    = {0, 0, 1, 0}
constant.T_NZAXIS   = {0, 0, -1, 0}

constant.W_AXIS     = {0, 0, 0, 1}

constant.T_IDENTITY_MAT = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
}
constant.T_IDENTITY_QUAT    = {0, 0, 0, 1}

constant.ZERO       = math3d.constant("v4", constant.T_ZERO)
constant.ZERO_PT    = math3d.constant("v4", constant.T_ZERO_PT)

constant.ONE        = math3d.constant("v4", constant.T_ONE)
constant.ONE_PT     = math3d.constant("v4", constant.T_ONE_PT)

constant.XAXIS      = math3d.constant("v4", constant.T_XAXIS)
constant.NXAXIS     = math3d.constant("v4", constant.T_NXAXIS)

constant.YAXIS      = math3d.constant("v4", constant.T_YAXIS)
constant.NYAXIS     = math3d.constant("v4", constant.T_NYAXIS)

constant.ZAXIS      = math3d.constant("v4", constant.T_ZAXIS)
constant.NZAXIS     = math3d.constant("v4", constant.T_NZAXIS)

constant.IDENTITY_MAT   = math3d.constant("m4",     constant.T_IDENTITY_MAT)
constant.IDENTITY_QUAT  = math3d.constant("quat",   constant.T_IDENTITY_QUAT)

constant.R2L_MAT    = math3d.constant("m4", {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0,-1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
})

return constant