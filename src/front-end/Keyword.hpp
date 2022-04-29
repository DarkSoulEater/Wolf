#ifndef WOLF_KEYWORD_HPP
#define WOLF_KEYWORD_HPP

#define DeclKeyword(NAME) w##NAME,
enum class wKeyword {
    #include "Keyword.decl"
};

#endif // WOLF_KEYWORD_HPP