#ifndef WOLF_GLOBAL_HPP
#define WOLF_GLOBAL_HPP

// Variable

extern const char* InputFileName;

extern const char* StartRule;

// Flags

extern bool FlagWithoutBranches;

extern bool FlagSkipUselessRule;

// Debug
#define WOLF_DB_LOG_PARCE_ARG

#define WOLF_DB_LOG_RULE_FIRST_TERMINAL

#define WOLF_DB_PRINT_SEMANTIC_TOKEN

//#define WOLF_DB_LOG_KEYWORD

#define WOLF_CHECK_LL1

#endif // WOLF_GLOBAL_HPP