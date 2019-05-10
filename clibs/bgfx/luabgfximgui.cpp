#define LUA_LIB

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "imgui/imgui.h"
#include <cstring>

static int
lcreate(lua_State *L) {
	float fontSize = luaL_checknumber(L, 1);
	imguiCreate(fontSize);
	return 0;
}

static int
ldestroy(lua_State *L) {
	imguiDestroy();
	return 0;
}

static int
lbeginFrame(lua_State *L) {
	int32_t mx = luaL_checkinteger(L, 1);
	int32_t my = luaL_checkinteger(L, 2);
	int button1 = lua_toboolean(L, 3);
	int button2 = lua_toboolean(L, 4);
	int button3 = lua_toboolean(L, 5);
	int32_t scroll = luaL_checkinteger(L, 6);
	uint16_t width = luaL_checkinteger(L, 7);
	uint16_t height = luaL_checkinteger(L, 8);
	int inputChar = luaL_checkinteger(L, 9);
	bgfx::ViewId view = luaL_checkinteger(L, 10);
	uint8_t button = 
		(button1 ? IMGUI_MBUT_LEFT : 0) |
		(button2 ? IMGUI_MBUT_RIGHT : 0) |
		(button3 ? IMGUI_MBUT_MIDDLE : 0);
	imguiBeginFrame(mx, my, button, scroll, width, height, inputChar, view);
	return 0;
}

static int
lendFrame(lua_State *L) {
	imguiEndFrame();
	return 0;
}

// Widgets bindings
static int
wButton(lua_State *L) {
	const char * text = luaL_checkstring(L, 1);
	int w = luaL_optnumber(L, 2, 0);
	int h = luaL_optnumber(L, 3, 0);
	bool click = ImGui::Button(text, ImVec2(w,h));
	lua_pushboolean(L, click);
	return 1;
}

static int
wSmallButton(lua_State *L) {
	const char * text = luaL_checkstring(L, 1);
	bool click = ImGui::SmallButton(text);
	lua_pushboolean(L, click);
	return 1;
}

static int
wInvisibleButton(lua_State *L) {
	const char * text = luaL_checkstring(L, 1);
	int w = luaL_optnumber(L, 2, 0);
	int h = luaL_optnumber(L, 3, 0);
	bool click = ImGui::InvisibleButton(text, ImVec2(w,h));
	lua_pushboolean(L, click);
	return 1;
}

static int
wArrowButton(lua_State *L) {
	const char * text = luaL_checkstring(L, 1);
	const char * dir = luaL_checkstring(L, 2);
	ImGuiDir d;
	switch (dir[0]) {
	case 'l':case 'L':
		d = ImGuiDir_Left;
		break;
	case 'r':case 'R':
		d = ImGuiDir_Right;
		break;
	case 'u':case 'U':
		d = ImGuiDir_Up;
		break;
	case 'd':case 'D':
		d = ImGuiDir_Down;
		break;
	default:
		d = ImGuiDir_None;
		break;
	}

	bool click = ImGui::ArrowButton(text, d);
	lua_pushboolean(L, click);
	return 1;
}

static int
wColorButton(lua_State *L) {
	const char * desc = luaL_checkstring(L, 1);
	float c1 = luaL_checknumber(L, 2);
	float c2 = luaL_checknumber(L, 3);
	float c3 = luaL_checknumber(L, 4);
	float c4 = luaL_optnumber(L, 5, 1.0f);
	ImGuiColorEditFlags flags = luaL_optinteger(L, 6, 0);
	float w = luaL_optnumber(L, 7, 0);
	float h = luaL_optnumber(L, 8, 0);
	bool click = ImGui::ColorButton(desc, ImVec4(c1,c2,c3,c4), flags, ImVec2(w,h));
	lua_pushboolean(L, click);
	return 1;
}

// Todo:  Image ,  ImageButton, CheckboxFlags, Combo

static int
wCheckbox(lua_State *L) {
	const char * text = luaL_checkstring(L, 1);
	bool v = lua_toboolean(L, 2);
	bool change = ImGui::Checkbox(text, &v);
	lua_pushboolean(L, change);
	lua_pushboolean(L, v);
	return 2;
}

static int
wRadioButton(lua_State *L) {
	const char * text = luaL_checkstring(L, 1);
	bool v = lua_toboolean(L, 2);
	v = ImGui::RadioButton(text, v);
	lua_pushboolean(L, v);
	return 1;
}

static int
wProgressBar(lua_State *L) {
	float fraction = luaL_checknumber(L, 1);
	float w = -1;
	float h = 0; 
	const char *overlay = NULL;
	if (lua_isstring(L, 2)) {
		overlay = lua_tostring(L, 2);
	} else {
		w = luaL_optnumber(L, 2, -1);
		h = luaL_optnumber(L, 3, 0);
		if (lua_isstring(L, 4)) {
			overlay = lua_tostring(L, 4);
		}
	}
	ImGui::ProgressBar(fraction, ImVec2(w,h), overlay);
	return 0;
}

static int
wBullet(lua_State *L) {
	ImGui::Bullet();
	return 0;
}

static float
read_field_float(lua_State *L, const char * field, float v) {
	if (lua_getfield(L, 1, field) == LUA_TNUMBER) {
		v = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
	return v;
}

static float
read_field_checkfloat(lua_State *L, const char * field) {
	float v;
	if (lua_getfield(L, 1, field) == LUA_TNUMBER) {
		v = lua_tonumber(L, -1);
	} else {
		v = 0;
		luaL_error(L, "no float %s", field);
	}
	lua_pop(L, 1);
	return v;
}

static int
read_field_int(lua_State *L, const char * field, int v) {
	if (lua_getfield(L, 1, field) == LUA_TNUMBER) {
		if (!lua_isinteger(L, -1)) {
			luaL_error(L, "Not an integer");
		}
		v = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return v;
}

static int
read_field_checkint(lua_State *L, const char * field) {
	int v;
	if (lua_getfield(L, 1, field) == LUA_TNUMBER) {
		if (!lua_isinteger(L, -1)) {
			luaL_error(L, "Not an integer");
		}
		v = lua_tointeger(L, -1);
	} else {
		v = 0;
		luaL_error(L, "no int %s", field);
	}
	lua_pop(L, 1);
	return v;
}

static const char *
read_field_string(lua_State *L, const char * field, const char *v) {
	if (lua_getfield(L, 1, field) == LUA_TSTRING) {
		v = lua_tostring(L, -1);
	}
	lua_pop(L, 1);
	return v;
}

static bool
read_field_boolean(lua_State *L, const char *field) {
	int v = false;
	if (lua_getfield(L, 1, field) == LUA_TBOOLEAN) {
		v = lua_toboolean(L, 1);
	}
	lua_pop(L, 1);
	return v;
}

static bool
drag_float(lua_State *L, int n) {
	float v[4];
	int i;
	for (i=0;i<n;i++) {
		if (lua_geti(L, 1, i+1) != LUA_TNUMBER) {
			luaL_error(L, "Need float [%d]", i+1);
		}
		v[i] = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	const char * label = read_field_string(L, "label", "DragFloat");
	float speed = read_field_float(L, "speed", 1.0f);
	float min = read_field_float(L, "min", 0.0f);
	float max = read_field_float(L, "max", 0.0f);
	const char * format = read_field_string(L, "format", "%.3f");
	float power = read_field_float(L, "power", 1.0f);
	bool change = false;
	switch(n) {
	case 1:
		change = ImGui::DragFloat(label, v, speed, min, max, format, power);
		break;
	case 2:
		if (read_field_boolean(L, "range")) {
			const char *format_max = read_field_string(L, "format_max", NULL);
			change = ImGui::DragFloatRange2(label, v+0, v+1, speed, min, max, format, format_max, power);
		} else {
			change = ImGui::DragFloat2(label, v, speed, min, max, format, power);
		}
		break;
	case 3:
		change = ImGui::DragFloat3(label, v, speed, min, max, format, power);
		break;
	case 4:
		change = ImGui::DragFloat4(label, v, speed, min, max, format, power);
		break;
	}
	for (i=0;i<n;i++) {
		lua_pushnumber(L, v[i]);
		lua_seti(L, 1, i+1);
	}
	return change;
}

static bool
drag_int(lua_State *L, int n) {
	int v[4];
	int i;
	for (i=0;i<n;i++) {
		if (lua_geti(L, 1, i+1) != LUA_TNUMBER || !lua_isinteger(L, -1)) {
			luaL_error(L, "Need integer [%d]", i+1);
		}
		v[i] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	const char * label = read_field_string(L, "label", "DragInt");
	float speed = read_field_float(L, "speed", 1.0f);
	int min = read_field_int(L, "min", 0);
	int max = read_field_int(L, "max", 0);
	const char * format = read_field_string(L, "format", "%d");
	bool change = false;
	switch(n) {
	case 1:
		change = ImGui::DragInt(label, v, speed, min, max, format);
		break;
	case 2:
		if (read_field_boolean(L, "range")) {
			const char *format_max = read_field_string(L, "format_max", NULL);
			change = ImGui::DragIntRange2(label, v+0, v+1, speed, min, max, format, format_max);
		} else {
			change = ImGui::DragInt2(label, v, speed, min, max, format);
		}
		break;
	case 3:
		change = ImGui::DragInt3(label, v, speed, min, max, format);
		break;
	case 4:
		change = ImGui::DragInt4(label, v, speed, min, max, format);
		break;
	}
	for (i=0;i<n;i++) {
		lua_pushinteger(L, v[i]);
		lua_seti(L, 1, i+1);
	}
	return change;
}

static bool
slider_float(lua_State *L, int n) {
	float v[4];
	int i;
	for (i=0;i<n;i++) {
		if (lua_geti(L, 1, i+1) != LUA_TNUMBER) {
			luaL_error(L, "Need float [%d]", i+1);
		}
		v[i] = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	const char * label = read_field_string(L, "label", "SliderFloat");
	float min = read_field_checkfloat(L, "min");
	float max = read_field_checkfloat(L, "max");
	const char * format = read_field_string(L, "format", "%.3f");
	bool change = false;
	switch(n) {
	case 1:
		change = ImGui::SliderFloat(label, v, min, max, format);
		break;
	case 2:
		change = ImGui::SliderFloat2(label, v, min, max, format);
		break;
	case 3:
		change = ImGui::SliderFloat3(label, v, min, max, format);
		break;
	case 4:
		change = ImGui::SliderFloat4(label, v, min, max, format);
		break;
	}
	for (i=0;i<n;i++) {
		lua_pushnumber(L, v[i]);
		lua_seti(L, 1, i+1);
	}
	return change;
}

static bool
slider_int(lua_State *L, int n) {
	int v[4];
	int i;
	for (i=0;i<n;i++) {
		if (lua_geti(L, 1, i+1) != LUA_TNUMBER || !lua_isinteger(L, -1)) {
			luaL_error(L, "Need integer [%d]", i+1);
		}
		v[i] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	const char * label = read_field_string(L, "label", "SliderInt");
	int min = read_field_checkint(L, "min");
	int max = read_field_checkint(L, "max");
	const char * format = read_field_string(L, "format", "%d");
	bool change = false;
	switch(n) {
	case 1:
		change = ImGui::SliderInt(label, v, min, max, format);
		break;
	case 2:
		change = ImGui::SliderInt2(label, v, min, max, format);
		break;
	case 3:
		change = ImGui::SliderInt3(label, v, min, max, format);
		break;
	case 4:
		change = ImGui::SliderInt4(label, v, min, max, format);
		break;
	}
	for (i=0;i<n;i++) {
		lua_pushinteger(L, v[i]);
		lua_seti(L, 1, i+1);
	}
	return change;
}

static bool
slider_angle(lua_State *L) {
	float r;
	if (lua_geti(L, 1, 1) != LUA_TNUMBER) {
		luaL_error(L, "Need float deg");
	}
	r = lua_tonumber(L, -1);
	lua_pop(L, 1);
	const char * label = read_field_string(L, "label", "SliderAngle");
	float min = read_field_float(L, "min", -360.0f);
	float max = read_field_float(L, "max", +360.0f);
	const char * format = read_field_string(L, "format", "%.0f deg");
	float change = ImGui::SliderAngle(label, &r, min, max, format);
	lua_pushnumber(L, r);
	lua_seti(L, -2, 1);
	return change;
}

static bool
vslider_float(lua_State *L) {
	float r;
	if (lua_geti(L, 1, 1) != LUA_TNUMBER) {
		luaL_error(L, "Need float");
	}
	r = lua_tonumber(L, -1);
	lua_pop(L, 1);
	const char * label = read_field_string(L, "label", "VSliderFloat");
	float width = read_field_checkfloat(L, "width");
	float height = read_field_checkfloat(L, "height");
	float min = read_field_checkfloat(L, "min");
	float max = read_field_checkfloat(L, "max");
	const char * format = read_field_string(L, "format", "%.3f");
	float power = read_field_float(L, "power", 1.0f);
	float change = ImGui::VSliderFloat(label, ImVec2(width, height), &r, min, max, format, power);
	lua_pushnumber(L, r);
	lua_seti(L, -2, 1);
	return change;
}

static bool
vslider_int(lua_State *L) {
	int r;
	if (lua_geti(L, 1, 1) != LUA_TNUMBER) {
		luaL_error(L, "Need float");
	}
	r = lua_tointeger(L, -1);
	lua_pop(L, 1);
	const char * label = read_field_string(L, "label", "VSliderInt");
	float width = read_field_checkfloat(L, "width");
	float height = read_field_checkfloat(L, "height");
	int min = read_field_checkint(L, "min");
	int max = read_field_checkint(L, "max");
	const char * format = read_field_string(L, "format", "%d");
	float change = ImGui::VSliderInt(label, ImVec2(width, height), &r, min, max, format);
	lua_pushinteger(L, r);
	lua_seti(L, -2, 1);
	return change;
}

#define DRAG_FLOAT 0
#define DRAG_INT 1
#define SLIDER_FLOAT 2
#define SLIDER_INT 3
#define SLIDER_ANGLE 4
#define VSLIDER_FLOAT 5
#define VSLIDER_INT 6

#define COLOR_EDIT 0
#define COLOR_PICKER 1

static int
wDrag(lua_State *L, int type) {
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_len(L, 1);
	int n = lua_tointeger(L, -1);
	lua_pop(L, 1);
	if (n < 1 || n > 4)
		return luaL_error(L, "Need 1-4 numbers");
	bool change = false;
	// todo: DragScalar/DragScalarN/SliderScalar/SliderScalarN/VSliderScalar
	switch(type) {
	case DRAG_FLOAT:
		change = drag_float(L, n);
		break;
	case DRAG_INT:
		change = drag_int(L, n);
		break;
	case SLIDER_FLOAT:
		change = slider_float(L, n);
		break;
	case SLIDER_INT:
		change = slider_int(L, n);
		break;
	case SLIDER_ANGLE:
		change = slider_angle(L);
		break;
	case VSLIDER_FLOAT:
		change = vslider_float(L);
		break;
	case VSLIDER_INT:
		change = vslider_int(L);
		break;
	}
	lua_pushboolean(L, change);
	return 1;
}

static int
wDragFloat(lua_State *L) {
	return wDrag(L, DRAG_FLOAT);
}

static int
wDragInt(lua_State *L) {
	return wDrag(L, DRAG_INT);
}

static int
wSliderFloat(lua_State *L) {
	return wDrag(L, SLIDER_FLOAT);
}

static int
wSliderInt(lua_State *L) {
	return wDrag(L, SLIDER_INT);
}

static int
wSliderAngle(lua_State *L) {
	return wDrag(L, SLIDER_ANGLE);
}

static int
wVSliderFloat(lua_State *L) {
	return wDrag(L, VSLIDER_FLOAT);
}

static int
wVSliderInt(lua_State *L) {
	return wDrag(L, VSLIDER_INT);
}

static int
wColor(lua_State *L, int type) {
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_len(L, 1);
	int n = lua_tointeger(L, -1);
	lua_pop(L, 1);
	if (n < 3 || n > 4)
		return luaL_error(L, "Need 3-4 numbers");
	const char * label = read_field_string(L, "label", "Color");
	ImGuiColorEditFlags flags = read_field_int(L, "flags", 0);
	float v[4];
	int i;
	for (i=0;i<n;i++) {
		if (lua_geti(L, 1, i+1) != LUA_TNUMBER) {
			luaL_error(L, "Color should be a number");
		}
		v[i] = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	bool change;
	if (type == COLOR_EDIT) {
		if (n == 3) {
			change = ImGui::ColorEdit3(label, v, flags);
		} else {
			change = ImGui::ColorEdit4(label, v, flags);
		}
	} else {
		if (n == 3) {
			change = ImGui::ColorPicker3(label, v, flags);
		} else {
			const char * ref = NULL;
			if (lua_getfield(L, 1, "ref") == LUA_TSTRING) {
				size_t sz;
				ref = lua_tolstring(L, -1, &sz);
				if (sz != 4 * sizeof(float)) {
					luaL_error(L, "Color ref should be 4 float string");
				}
			}
			lua_pop(L, 1);
			change = ImGui::ColorPicker4(label, v, flags, (const float *)ref);
		}
	}
	for (i=0;i<n;i++) {
		lua_pushnumber(L, v[i]);
		lua_seti(L, 1, i+1);
	}
	lua_pushboolean(L, change);
	return 1;
}

static int
wColorEdit(lua_State *L) {
	return wColor(L, COLOR_EDIT);
}

static int
wColorPicker(lua_State *L) {
	return wColor(L, COLOR_PICKER);
}

// enums
struct enum_pair {
	const char * name;
	int value;
};

#define ENUM(prefix, name) { #name, prefix##_##name }

static int
make_enum(lua_State *L) {
	luaL_checktype(L, 1, LUA_TTABLE);
	int i,t;
	int r = 0;

	for (i=1;(t = lua_geti(L, 1, i)) != LUA_TNIL;i++) {
		if (t != LUA_TSTRING)
			luaL_error(L, "Enum name should be string, it's %s", lua_typename(L, t));
		if (lua_gettable(L, lua_upvalueindex(1)) != LUA_TNUMBER) {
			lua_geti(L, 1, i);
			luaL_error(L, "Invalid enum %s.%s", lua_tostring(L, lua_upvalueindex(2)), lua_tostring(L, -1));
		}
		int v = lua_tointeger(L, -1);
		lua_pop(L, 1);
		r |= v;
	}
	lua_pushinteger(L, r);
	return 1;
}

static void
enum_gen(lua_State *L, const char *name, struct enum_pair *enums) {
	int i;
	lua_newtable(L);
	for (i=0;enums[i].name;i++) {
		lua_pushinteger(L, enums[i].value);
		lua_setfield(L, -2, enums[i].name);
	}
	lua_pushstring(L, name);
	lua_pushcclosure(L, make_enum, 2);
	lua_setfield(L, -2, name);
}

static int
lSetColorEditOptions(lua_State *L) {
	ImGuiColorEditFlags flags = luaL_checkinteger(L, 1);
	ImGui::SetColorEditOptions(flags);
	return 0;
}

// key, press, state
static int
lkeyState(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	int press = lua_toboolean(L, 2);
	int state = luaL_checkinteger(L, 3);

	ImGuiIO& io = ImGui::GetIO();

	io.KeyCtrl = state & 0x01;
	io.KeyShift = state & 0x04;
	io.KeyAlt = state & 0x02;
	io.KeySuper = state & 0x08;

	if (key >=0 && key < 512) {
		io.KeysDown[key] = press;
	}
	return 0;
}

extern "C" LUAMOD_API int
luaopen_bgfx_imgui(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "create", lcreate },
		{ "destroy", ldestroy },
		{ "begin_frame", lbeginFrame },
		{ "end_frame", lendFrame },
		{ "key_state", lkeyState },
		{ "SetColorEditOptions", lSetColorEditOptions },
		{ NULL, NULL },
	};

	luaL_newlib(L, l);

	luaL_Reg widgets[] = {
		{ "Button", wButton },
		{ "SmallButton", wSmallButton },
		{ "InvisibleButton", wInvisibleButton },
		{ "ArrowButton", wArrowButton },
		{ "Checkbox", wCheckbox },
		{ "RadioButton", wRadioButton },
		{ "ProgressBar", wProgressBar },
		{ "Bullet", wBullet },
		{ "DragFloat", wDragFloat },
		{ "DragInt", wDragInt },
		{ "SliderFloat", wSliderFloat },
		{ "SliderInt", wSliderInt },
		{ "SliderAngle", wSliderAngle },
		{ "VSliderFloat", wVSliderFloat },
		{ "VSliderInt", wVSliderInt },
		{ "ColorEdit", wColorEdit },
		{ "ColorPicker", wColorPicker },
		{ "ColorButton", wColorButton },
		{ NULL, NULL },
	};
	luaL_newlib(L, widgets);
	lua_setfield(L, -2, "widget");

	lua_newtable(L);
	static struct enum_pair eColorEditFlags[] = {
		ENUM(ImGuiColorEditFlags, NoAlpha),
		ENUM(ImGuiColorEditFlags, NoPicker),
		ENUM(ImGuiColorEditFlags, NoOptions),
		ENUM(ImGuiColorEditFlags, NoSmallPreview),
		ENUM(ImGuiColorEditFlags, NoInputs),
		ENUM(ImGuiColorEditFlags, NoTooltip),
		ENUM(ImGuiColorEditFlags, NoLabel),
		ENUM(ImGuiColorEditFlags, NoSidePreview),
		ENUM(ImGuiColorEditFlags, NoDragDrop),
		ENUM(ImGuiColorEditFlags, AlphaBar),
		ENUM(ImGuiColorEditFlags, AlphaPreview),
		ENUM(ImGuiColorEditFlags, AlphaPreviewHalf),
		ENUM(ImGuiColorEditFlags, HDR),
		ENUM(ImGuiColorEditFlags, DisplayRGB),
		ENUM(ImGuiColorEditFlags, DisplayHSV),
		ENUM(ImGuiColorEditFlags, DisplayHex),
		ENUM(ImGuiColorEditFlags, Uint8),
		ENUM(ImGuiColorEditFlags, Float),
		ENUM(ImGuiColorEditFlags, PickerHueBar),
		ENUM(ImGuiColorEditFlags, PickerHueWheel),
		ENUM(ImGuiColorEditFlags, InputRGB),
		ENUM(ImGuiColorEditFlags, InputHSV),
		{ NULL, 0 },
	};
	enum_gen(L, "ColorEditFlags", eColorEditFlags);
	lua_setfield(L, -2, "enum");

	return 1;
}

