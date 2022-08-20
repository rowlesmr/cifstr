#include "cifparse.hpp"



void row::cif::Status::loop()
{
	is_loop = !is_loop;
}

void row::cif::Status::finished_printing()
{
	is_quote = false;
	is_printed = false;
}

void row::cif::Status::just_printed()
{
	is_printed = true;
}

void row::cif::Status::get_ready_to_print()
{
	is_quote = true;
	is_printed = false;
}

void row::cif::Status::reset()
{
	is_loop = false;
	is_quote = false;
	is_printed = false;
}

void row::cif::Buffer::clear()
{
	tag.clear();
	tags.clear();
	values.clear();
	loopNum = 0;
	maxLoop = 0;
	totalValues = 0;
	tagNum = 0;
}

void row::cif::Buffer::appendValue(std::string val)
{
	values[loopNum].push_back(std::move(val));
	loopNum = ++loopNum % maxLoop;
	++totalValues;
}

void row::cif::Buffer::appendTag(std::string in_tag)
{
	tags.push_back(std::move(in_tag));
	++tagNum;
}

void row::cif::Buffer::initialiseValues()
{
	if (values.size() == 0) {
		maxLoop = tags.size();
		values = std::vector<Datavalue>(maxLoop);
	}
}

row::cif::Cif row::cif::read_file(const std::string& filename, bool overwrite /*= false*/) noexcept(false)
{
	pegtl::file_input in(filename);
	return read_input(in, overwrite);
}

row::cif::Cif row::cif::read_string(const std::string& cifstring, bool overwrite /*= false*/, const std::string& source /*= "string"*/) noexcept(false)
{
	pegtl::string_input in(cifstring, source);
	return read_input(in, overwrite);
}
