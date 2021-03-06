#include "string_view.hpp"

#include <string>
#include <ostream>

#include <cstring>

string_view::string_view(char const* str)
    : m_data { str }, m_size { strlen(str) } {}

string_view::string_view(std::string const& str)
    : m_data { str.data() }, m_size { str.size() } {}

std::ostream& operator << (std::ostream& o, string_view const& sv) {
	for(char c : sv)
		o << c;
	return o;
}
