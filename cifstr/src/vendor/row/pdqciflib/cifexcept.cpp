
#include "cifexcept.hpp"


const char* row::cif::internal::concreteException::what() const noexcept
{
	return m_msg.c_str();
}

row::cif::internal::concreteException::concreteException(const char* msg) : m_msg(msg)
{

}

const char* row::cif::no_such_tag_error::what() const noexcept
{
	return m_impl->what();
}

row::cif::no_such_tag_error::no_such_tag_error(const std::string& msg) : no_such_tag_error(msg.c_str())
{

}

row::cif::no_such_tag_error::no_such_tag_error(const char* msg) : std::out_of_range(msg),
m_impl(std::make_shared<internal::concreteException>(msg))
{

}

const char* row::cif::tag_already_exists_error::what() const noexcept
{
	return m_impl->what();
}

row::cif::tag_already_exists_error::tag_already_exists_error(const std::string& msg) : std::runtime_error(msg.c_str()),
m_impl(std::make_shared<internal::concreteException>(msg.c_str()))
{

}

row::cif::tag_already_exists_error::tag_already_exists_error(const char* msg) : std::runtime_error(msg),
m_impl(std::make_shared<internal::concreteException>(msg))
{

}

const char* row::cif::loop_length_mismatch_error::what() const noexcept
{
	return m_impl->what();
}

row::cif::loop_length_mismatch_error::loop_length_mismatch_error(const std::string& msg) : std::runtime_error(msg.c_str()),
m_impl(std::make_shared<internal::concreteException>(msg.c_str()))
{

}

row::cif::loop_length_mismatch_error::loop_length_mismatch_error(const char* msg) : std::runtime_error(msg),
m_impl(std::make_shared<internal::concreteException>(msg))
{

}

const char* row::cif::tag_value_mismatch_error::what() const noexcept
{
	return m_impl->what();
}

row::cif::tag_value_mismatch_error::tag_value_mismatch_error(const std::string& msg) : std::runtime_error(msg.c_str()),
m_impl(std::make_shared<internal::concreteException>(msg.c_str()))
{

}

row::cif::tag_value_mismatch_error::tag_value_mismatch_error(const char* msg) : std::runtime_error(msg),
m_impl(std::make_shared<internal::concreteException>(msg))
{

}

const char* row::cif::illegal_tag_error::what() const noexcept
{
	return m_impl->what();
}

row::cif::illegal_tag_error::illegal_tag_error(const std::string& msg) : illegal_tag_error(msg.c_str())
{

}

row::cif::illegal_tag_error::illegal_tag_error(const char* msg) : std::runtime_error(msg),
m_impl(std::make_shared<internal::concreteException>(msg))
{

}


