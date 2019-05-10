#include "utils.h"

#include <sstream>

std::vector<std::string>
split_string(const std::string &ss, char delim) {
	std::istringstream iss(ss);
	std::vector<std::string> vv;
	std::string elem;
	while (std::getline(iss, elem, delim)) {
		vv.push_back(elem);
	}

	return vv;
}

const char* s_default_layout = "_30NIf";

std::string
GetDefaultVertexLayoutElem() {
	return s_default_layout;
}

std::string&
refine_layout(std::string &elem) {
	assert(!elem.empty());
	if (elem.size() < 6) {
		elem += s_default_layout + elem.size();
	}

	return elem;
}

std::vector<std::string>
split_layout_elems(const std::string &layout) {
	auto elems = split_string(layout, '|');
	for (auto &e : elems) {
		refine_layout(e);
	}

	return elems;
}

std::string
refine_layouts(std::string &layout) {
	auto elems = split_layout_elems(layout);
	
	std::string fulllayout;
	for (const auto &e : elems){
		if (!fulllayout.empty()) {
			fulllayout += "|";
		}
		fulllayout += e;
	}
	return fulllayout;
}

bgfx::Attrib::Enum 
GetAttribFromLayoutElem(const std::string &elem) {
	switch (elem[0])
	{
	case 'p':return bgfx::Attrib::Position;
	case 'n':return bgfx::Attrib::Normal;
	case 'T':return bgfx::Attrib::Tangent;
	case 'b':return bgfx::Attrib::Bitangent;
	case 'i':return bgfx::Attrib::Indices;
	case 'w':return bgfx::Attrib::Weight;
	case 't': {
		auto channel = elem[2] - '0';
		return bgfx::Attrib::Enum(bgfx::Attrib::TexCoord0 + channel);
	}
	case 'c': {
		auto channel = elem[2] - '0';
		return bgfx::Attrib::Enum(bgfx::Attrib::Color0 + channel);
	}
	default:
		printf("not support type, %d", elem[0]);
		return bgfx::Attrib::Count;
	}
}

bgfx::VertexDecl
GenVertexDeclFromVBLayout(const std::string &vblayout) {
	auto elems = split_layout_elems(vblayout);
	bgfx::VertexDecl decl;
	decl.begin();
	for (const auto &e : elems) {
		auto attrib = GetAttribFromLayoutElem(e);

		uint8_t num = e[1] - '0';
		auto get_type = [](const std::string &e) {
			switch (e[5]) {
			case 'f': return bgfx::AttribType::Float;
			case 'h': return bgfx::AttribType::Half;
			case 'u': return bgfx::AttribType::Uint8;
			case 'U': return bgfx::AttribType::Uint10;
			case 'i': return bgfx::AttribType::Int16;
			default:return bgfx::AttribType::Count;
			}
		};

		auto type = get_type(e);
		bool asInt = e[4] == 'i';
		bool normalize = e[3] == 'n';
		decl.add(attrib, num, type, normalize, asInt);
	}
	decl.end();
	return decl;
}

std::string
GenVBLayoutFromDecl(const bgfx::VertexDecl &decl) {
	std::string vblayout;
	for (uint32_t ii = bgfx::Attrib::Position; ii < bgfx::Attrib::Count; ++ii) {
		auto attrib = bgfx::Attrib::Enum(ii);
		if (decl.has(attrib)) {
			uint8_t num;
			bgfx::AttribType::Enum type;
			bool normalize, asInt;
			decl.decode(attrib, num, type, normalize, asInt);

			auto get_attrib_name = [](bgfx::Attrib::Enum a, uint8_t num) {
				auto numstr = std::to_string(num);
				const char* names[bgfx::Attrib::Count] = {
					"p0", "n0", "T0", "b0",
					"c0", "c0", "c0", "c0",
					"i0", "w0",
					"t0", "t1","t2","t3",
					"t4", "t5","t6","t7",
				};

				assert(bgfx::Attrib::Count > a);

				const char* name = names[a];
				return name[0] + std::to_string(num) + name[1];
			};

			if (!vblayout.empty())
				vblayout += '|';
			vblayout += get_attrib_name(attrib, num);
			vblayout += normalize ? 'n' : 'N';
			vblayout += asInt ? 'i' : 'I';

			auto get_type_char = [](bgfx::AttribType::Enum type) {
				char cc[bgfx::AttribType::Count] = {
					'u', 'U', 'i', 'h', 'f'
				};
				assert(bgfx::AttribType::Count > type);
				return cc[type];
			};

			vblayout += get_type_char(type);
		}
	}

	return vblayout;
};


std::string GenStreamNameFromDecl(const bgfx::VertexDecl &decl) {
	const char* short_names[] = {
		"p", "n", "T", "b",
		"c0", "c1", "c2", "c3",
		"i", "w",
		"t0", "t1", "t2", "t3",
		"t4", "t5", "t6", "t7",
	};
	std::string streamName;
	for (uint32_t ii = bgfx::Attrib::Position; ii < bgfx::Attrib::Count; ++ii) {
		auto attrib = bgfx::Attrib::Enum(ii);
		if (decl.has(attrib)) {
			auto name = short_names[ii];
			streamName += name;
		}
	}

	return streamName;
}


std::string GenStreamNameFromElem(const std::string &elem){
	std::string ss;
	ss += elem[0];

	if (elem[0] == 't' || elem[2] == 'c') {
		ss += elem[2];
	}

	return ss;
}

size_t
GetVertexElemSizeInBytes(const std::string &elem) {
	auto decl = GenVertexDeclFromVBLayout(elem);

	return decl.getStride();
}


size_t
CalcVertexSize(const std::string &layout) {
	bgfx::VertexDecl decl = GenVertexDeclFromVBLayout(layout);
	return decl.getStride();
};