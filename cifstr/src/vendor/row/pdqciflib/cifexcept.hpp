

#ifndef ROW_CIFEXCEPT_HPP
#define ROW_CIFEXCEPT_HPP

#include <stdexcept>
#include <string>
#include <memory>


namespace row::cif {

    namespace internal
    {
        class concreteException {
        private:
            std::string m_msg;
        public:
            explicit concreteException(const char* msg) : m_msg(msg) {}
            [[nodiscard]] const char* what() const noexcept { return m_msg.c_str(); }
        };

    } 


    class no_such_tag_error : public std::out_of_range {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        no_such_tag_error(const char* msg)
            : std::out_of_range(msg),
            m_impl(std::make_shared<internal::concreteException>(msg))  {}
        no_such_tag_error(const std::string& msg)
            : no_such_tag_error(msg.c_str()) {}

        [[nodiscard]] const char* what() const noexcept override {
            return m_impl->what();
        }
    };

    class tag_already_exists_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        tag_already_exists_error(const char* msg)
            : std::runtime_error(msg),
            m_impl(std::make_shared<internal::concreteException>(msg)) {}
        tag_already_exists_error(const std::string& msg)
            : std::runtime_error(msg.c_str()),
            m_impl(std::make_shared<internal::concreteException>(msg.c_str())) {}

        [[nodiscard]] const char* what() const noexcept override {
            return m_impl->what();
        }
    };

    class loop_length_mismatch_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        loop_length_mismatch_error(const char* msg)
            : std::runtime_error(msg),
            m_impl(std::make_shared<internal::concreteException>(msg)) {}
        loop_length_mismatch_error(const std::string& msg)
            : std::runtime_error(msg.c_str()),
            m_impl(std::make_shared<internal::concreteException>(msg.c_str())) {}

        [[nodiscard]] const char* what() const noexcept override {
            return m_impl->what();
        }
    };

    class tag_value_mismatch_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        tag_value_mismatch_error(const char* msg)
            : std::runtime_error(msg),
            m_impl(std::make_shared<internal::concreteException>(msg)) {}
        tag_value_mismatch_error(const std::string& msg)
            : std::runtime_error(msg.c_str()),
            m_impl(std::make_shared<internal::concreteException>(msg.c_str())) {}

        [[nodiscard]] const char* what() const noexcept override {
            return m_impl->what();
        }
    };

    class illegal_tag_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        illegal_tag_error(const char* msg)
            : std::runtime_error(msg),
            m_impl(std::make_shared<internal::concreteException>(msg)) {}
        illegal_tag_error(const std::string& msg)
            : illegal_tag_error(msg.c_str()) {}

        [[nodiscard]] const char* what() const noexcept override {
            return m_impl->what();
        }
    };

}

#endif
