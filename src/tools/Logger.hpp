#ifndef WOLD_LOGGER_HPP
#define WOLD_LOGGER_HPP

#include <cstddef>

class Logger {
private:
public:
    Logger();
    ~Logger();

    void Log(const char* text); // In stdout
    void Log(const char* file_name, size_t line, size_t column, const char* text); // In stdout
    //void Log(const char* format, ...); // In stdout

    void LogDebug(const char* text); // In stderr
    void LogDebug(const char* file_name, size_t line, size_t column, const char* text); // In stderr
    void PrintDebug(const char* format, ...); // In stderr

    void LogError(const char* text); // In stderr
    void LogError(const char* file_name, size_t line, size_t column, const char* text); // In stderr
    void PrintError(const char* format, ...); // In stderr
    void PrintError(const char* file_name, size_t line, size_t column, const char* format, ...); // In stderr

    void LogWarning(const char* text); // In stdout
    void LogWarning(const char* file_name, size_t line, size_t column, const char* text); // In stdout
    void PrintWarning(const char* format, ...); // In stdout
    void PrintWarning(const char* file_name, size_t line, size_t column, const char* format, ...); // In stdout
};

extern Logger wDebug;
extern Logger wLogger;


#endif // WOLD_LOGGER_HPP