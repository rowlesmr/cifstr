#include <iostream>
#include <string>
#include <array>
#include <string_view>

export module logger;


namespace row {

    export class Logger
    {
    public:
        enum Verbosity { NONE, SOME, ALL, EVERYTHING };
        Verbosity verbosity{ ALL };

    private:
        static constexpr std::array<std::string_view, 4> level_names {"NONE", "SOME", "ALL", "EVERYTHING"};

    public:
        Logger(Verbosity lev)
            :verbosity{ lev }
        {
            if (verbosity == EVERYTHING) {
                std::cout << "LOGGER set to: " << level_names[verbosity] << '\n';
            }
        }
        Logger() = default;

        ~Logger()
        {
            if (verbosity == EVERYTHING) {
                std::cout << " LOGGER destroyed" << std::endl;
            }
        }

        void log(Verbosity lev, const std::string_view message) const {
            if (lev <= verbosity) {
                std::cout << message << '\n';
            }
        }
    };

}