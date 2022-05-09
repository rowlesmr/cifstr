#include <iostream>
#include <string>
#include <array>

export module logger;


namespace row {

    export class Logger
    {
    public:
        enum Level { DEBUG, INFO, WARNING, ERROR, CRITICAL };

    private:
        Level level{ DEBUG };
        static constexpr std::array level_names{ "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL" };

    public:
        Logger(Level lev)
            :level{ lev }
        {
            if (level >= CRITICAL) {
                std::cout << "[" << level_names[level] << "]: " << "LOGGER set to: " << level_names[level] << '\n';
            }
        }
        Logger()
            : Logger(Level::DEBUG) {}

        ~Logger()
        {
            if (level >= CRITICAL) {
                std::cout << "[" << level_names[level] << "]: " << " LOGGER destroyed" << std::endl;
            }
        }

        void log(Level lev, std::string message)
        {
            if (lev >= level) {
                std::cout << "[" << level_names[lev] << "]: " << message << '\n';
            }
        }
    };


}