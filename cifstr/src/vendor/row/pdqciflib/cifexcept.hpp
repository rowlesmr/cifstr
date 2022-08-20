

#ifndef ROW_CIFEXCEPT_HPP
#define ROW_CIFEXCEPT_HPP

#include <stdexcept>
#include <string>
#include <memory>


namespace row::cif {

    namespace internal {
        class concreteException {
        private:
            std::string m_msg;
        public:
            explicit concreteException(const char* msg);
            [[nodiscard]] const char* what() const noexcept;
        };

    } 


    class no_such_tag_error : public std::out_of_range {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        no_such_tag_error(const char* msg);
        no_such_tag_error(const std::string& msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

    class tag_already_exists_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        tag_already_exists_error(const char* msg);
        tag_already_exists_error(const std::string& msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

    class loop_length_mismatch_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        loop_length_mismatch_error(const char* msg);
        loop_length_mismatch_error(const std::string& msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

    class tag_value_mismatch_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        tag_value_mismatch_error(const char* msg);
        tag_value_mismatch_error(const std::string& msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

    class illegal_tag_error : public std::runtime_error {
    private:
        std::shared_ptr<internal::concreteException> m_impl;

    public:
        illegal_tag_error(const char* msg);
        illegal_tag_error(const std::string& msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

}

#endif
