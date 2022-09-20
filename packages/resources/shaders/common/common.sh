#ifndef _COMMON_SH_
#define _COMMON_SH_
#include <bgfx_shader.sh>
uniform vec4 u_time;

#define u_current_time  u_time.x
#define u_delta_time    u_time.y

vec4 fetch_texture2d_size(sampler2D tex, int lod)
{
    ivec2 s = textureSize(tex, lod);
    return vec4(s.x, s.y, 1.0/s.x, 1.0/s.y);
}

#endif //_COMMON_SH_