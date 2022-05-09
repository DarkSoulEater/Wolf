#include <assert.h>
#include <iostream>
#include <Windows.h>
#include "Logger.hpp"

Logger wDebug;
Logger wLogger;

Logger::Logger() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hConsole, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
}

Logger::~Logger() {
    
}

#define NONE "\u001B[0m"
#define RED     "\u001B[91m"
#define MAGNETA "\u001B[95m"
#define BLUE    "\u001b[94m"

// -------------------- Log ------------------------
void Logger::Log(const char* text) {
    std::cout << BLUE"Log: "NONE << text << std::endl; 
}

void Logger::Log(const char* file_name, size_t line, size_t column, const char* text) {
    std::cout << file_name << ':' << line << ':' << column << ": " << BLUE"Log: "NONE << text << std::endl; 
}

//void Logger::Print(const char* format, ...) {}

// -------------------- Debug -----------------------
void Logger::LogDebug(const char* text) {
    std::cerr << MAGNETA"Debug: "NONE << text << std::endl;
}

void Logger::LogDebug(const char* file_name, size_t line, size_t column, const char* text) {
    std::cerr << file_name << ':' << line << ':' << column << ": " << MAGNETA"Debug: "NONE << text << std::endl;
}

void Logger::PrintDebug(const char* format, ...) {
    assert(format);
    va_list message;
    va_start(message, format);
    std::cerr << MAGNETA"Debug: "NONE;
    vfprintf(stderr, format, message);
    std::cerr << std::endl;
}

// -------------------- Error -----------------------
void Logger::LogError(const char* text) {
    std::cerr << RED"Error: "NONE << text << std::endl;
}

void Logger::LogError(const char* file_name, size_t line, size_t column, const char* text) {
    std::cerr << file_name << ':' << line << ':' << column << ": " << RED"Error: "NONE << text << std::endl;
}

void Logger::PrintError(const char* format, ...) {
    assert(format);
    va_list message;
    va_start(message, format);
    std::cerr << RED"Error: "NONE;
    vfprintf(stderr, format, message);
    std::cerr << std::endl;
}

void Logger::PrintError(const char* file_name, size_t line, size_t column, const char* format, ...) {
    assert(format);
    va_list message;
    va_start(message, format);
    std::cerr << file_name << ':' << line << ':' << column << ": " << RED"Error: "NONE;
    vfprintf(stderr, format, message);
    std::cerr << std::endl;
}


// -------------------- Warning -----------------------
void Logger::LogWarning(const char* text) {
    std::cout << MAGNETA"Warning: "NONE << text << std::endl;
}

void Logger::LogWarning(const char* file_name, size_t line, size_t column, const char* text) {
    std::cout << file_name << ':' << line << ':' << column << ": " << MAGNETA"Warning: "NONE << text << std::endl;
}

void Logger::PrintWarning(const char* format, ...) {
    assert(format);
    va_list message;
    va_start(message, format);
    std::cout << MAGNETA"Warning: "NONE;
    vfprintf(stderr, format, message);
    std::cout << std::endl;
}

void Logger::PrintWarning(const char* file_name, size_t line, size_t column, const char* format, ...) {
    assert(format);
    va_list message;
    va_start(message, format);
    std::cout << file_name << ':' << line << ':' << column << ": " << MAGNETA"Warning: "NONE;
    vfprintf(stderr, format, message);
    std::cout << std::endl;
}
