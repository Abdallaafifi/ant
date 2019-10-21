$input a_position
$output v_position
#include <bgfx_shader.sh>

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));	
#ifdef SM_LINEAR
	v_position = gl_Position;
#endif //SM_LINEAR
}