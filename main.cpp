#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <fstream>

enum TokenType {
    // enum of all lexeme types
    NUMBER,
    FLOAT,
    HEX, // Hexadecimal numbers (16)
    STRING,
    CHAR,
    PREPROCESSOR,
    COMMENT,
    RESERVED,
    OPERATOR,
    DELIMITER,
    IDENTIFIER,
    UNKNOWN,
    TYPE
};

struct Token {
    // save lexeme and its type
    std::string lexeme;
    TokenType type;
};


const std::unordered_map<std::string, TokenType> dataTypes = {
        {"int", TYPE},{"float", TYPE}, {"double", TYPE},{"char", TYPE},{"bool", TYPE},
        {"string", TYPE}
};

const std::unordered_map<std::string, TokenType> reservedWords = {
        {"alignas", RESERVED}, {"alignof", RESERVED}, {"andb", RESERVED},
        {"and_eqb", RESERVED}, {"asma", RESERVED}, {"auto", RESERVED},
        {"bitandb", RESERVED}, {"bitorb", RESERVED}, {"bool", RESERVED},
        {"break", RESERVED}, {"case", RESERVED}, {"catch", RESERVED},
        {"char", RESERVED}, {"char8_tc", RESERVED}, {"char16_t", RESERVED},
        {"char32_t", RESERVED}, {"class", RESERVED}, {"complb", RESERVED},
        {"conceptc", RESERVED}, {"const", RESERVED}, {"const_cast", RESERVED},
        {"constevalc", RESERVED}, {"constexpr", RESERVED}, {"constinitc", RESERVED},
        {"continue", RESERVED}, {"co_awaitc", RESERVED}, {"co_returnc", RESERVED},
        {"co_yieldc", RESERVED}, {"decltype", RESERVED}, {"default", RESERVED},
        {"delete", RESERVED}, {"do", RESERVED}, {"double", RESERVED},
        {"dynamic_cast", RESERVED}, {"else", RESERVED}, {"enum", RESERVED},
        {"explicit", RESERVED}, {"exportc", RESERVED}, {"extern", RESERVED},
        {"false", RESERVED}, {"float", RESERVED}, {"for", RESERVED},
        {"friend", RESERVED}, {"goto", RESERVED}, {"if", RESERVED},
        {"inline", RESERVED}, {"int", RESERVED}, {"long", RESERVED},
        {"mutable", RESERVED}, {"namespace", RESERVED}, {"new", RESERVED},
        {"noexcept", RESERVED}, {"notb", RESERVED}, {"not_eqb", RESERVED},
        {"nullptr", RESERVED}, {"operator", RESERVED}, {"orb", RESERVED},
        {"or_eqb", RESERVED}, {"private", RESERVED}, {"protected", RESERVED},
        {"public", RESERVED}, {"register", RESERVED}, {"reinterpret_cast", RESERVED},
        {"requiresc", RESERVED}, {"return", RESERVED}, {"short", RESERVED},
        {"signed", RESERVED}, {"sizeof", RESERVED}, {"static", RESERVED},
        {"static_assert", RESERVED}, {"static_cast", RESERVED}, {"struct", RESERVED},
        {"switch", RESERVED}, {"template", RESERVED}, {"this", RESERVED},
        {"thread_local", RESERVED}, {"throw", RESERVED}, {"true", RESERVED},
        {"try", RESERVED}, {"typedef", RESERVED}, {"typeid", RESERVED},
        {"typename", RESERVED}, {"union", RESERVED}, {"unsigned", RESERVED},
        {"using", RESERVED}, {"virtual", RESERVED}, {"void", RESERVED},
        {"volatile", RESERVED}, {"wchar_t", RESERVED}, {"while", RESERVED},
        {"xorb", RESERVED}, {"xor_eqb", RESERVED}
};

const std::unordered_map<char, TokenType> operators = {
        {'+', OPERATOR}, {'-', OPERATOR}, {'*', OPERATOR},
        {'/', OPERATOR}, {'%', OPERATOR}, {'=', OPERATOR},
        {'<', OPERATOR}, {'>', OPERATOR}, {'!', OPERATOR},
};

const std::unordered_map<char, TokenType> delimiters = {
        {'(', DELIMITER}, {')', DELIMITER}, {'{', DELIMITER},
        {'}', DELIMITER}, {';', DELIMITER}, {',', DELIMITER}, {':', OPERATOR}
};

void addToken(std::vector<Token>& tokens, const std::string& lexeme, TokenType type) {
    // function adds a new token to the token vector

    tokens.push_back({lexeme, type});
}

void tokenize(const std::string& code, std::vector<Token>& tokens) {
    // function define if the word is lexeme

    size_t i = 0;
    while (i < code.length()) {
        if (isspace(code[i])) {
            i++;
            continue;
        }

        if (code[i] == '0' && (i + 1 < code.length() && (code[i + 1] == 'x' || code[i + 1] == 'X'))) {
            size_t start = i;
            i += 2;
            while (i < code.length() && isxdigit(code[i])) i++;
            addToken(tokens, code.substr(start, i - start), HEX); // Hexadecimal numbers (16)
            continue;
        }

        if (isdigit(code[i])) {
            size_t start = i;
            while (i < code.length() && isdigit(code[i])) i++;
            if (i < code.length() && code[i] == '.') {
                i++;
                while (i < code.length() && isdigit(code[i])) i++;
                addToken(tokens, code.substr(start, i - start), FLOAT);
            } else {
                addToken(tokens, code.substr(start, i - start), NUMBER);
            }
            continue;
        }

        if (code[i] == '"') {
            size_t start = i++;
            while (i < code.length() && code[i] != '"') i++;
            i++;
            addToken(tokens, code.substr(start, i - start), STRING);
            continue;
        }

        if (code[i] == '\'') {
            size_t start = i++;
            while (i < code.length() && code[i] != '\'') i++;
            i++;
            addToken(tokens, code.substr(start, i - start), CHAR);
            continue;
        }

        if (code[i] == '#') {
            size_t start = i++;
            while (i < code.length() && !isspace(code[i])) i++;
            addToken(tokens, code.substr(start, i - start), PREPROCESSOR);
            continue;
        }

        if (code[i] == '/' && (i + 1 < code.length() && (code[i + 1] == '/' || code[i + 1] == '*'))) {
            size_t start = i;
            if (code[i + 1] == '/') {
                i += 2;
                while (i < code.length() && code[i] != '\n') i++;
                addToken(tokens, code.substr(start, i - start), COMMENT);
            } else {
                i += 2;
                while (i + 1 < code.length() && !(code[i] == '*' && code[i + 1] == '/')) i++;
                i += 2;
                addToken(tokens, code.substr(start, i - start), COMMENT);
            }
            continue;
        }

        if (isalpha(code[i]) || code[i] == '_') {
            size_t start = i++;
            while (i < code.length() && (isalnum(code[i]) || code[i] == '_')) i++;
            std::string lexeme = code.substr(start, i - start);

            const Token& PrevToken = tokens[tokens.size() - 1];

            if (reservedWords.count(lexeme)) {
                addToken(tokens, lexeme, RESERVED);
            } else if (PrevToken.type == RESERVED) {
                addToken(tokens, lexeme, IDENTIFIER);
            }
            continue;
        }

        if (operators.count(code[i])) {
            addToken(tokens, std::string(1, code[i]), OPERATOR);
            i++;
            continue;
        }

        if (delimiters.count(code[i])) {
            addToken(tokens, std::string(1, code[i]), DELIMITER);
            i++;
            continue;
        }

        addToken(tokens, std::string(1, code[i]), UNKNOWN);
        i++;
    }
}

void printTokens(const std::vector<Token>& tokens) {
    // function print all lexemes which were found

    for (const auto& token : tokens) {
        std::cout << "<" << token.lexeme << ", ";
        switch (token.type) {
            case NUMBER: std::cout << "NUMBER"; break;
            case FLOAT: std::cout << "FLOAT"; break;
            case HEX: std::cout << "HEX"; break;
            case STRING: std::cout << "STRING"; break;
            case CHAR: std::cout << "CHAR"; break;
            case PREPROCESSOR: std::cout << "PREPROCESSOR"; break;
            case COMMENT: std::cout << "COMMENT"; break;
            case RESERVED: std::cout << "RESERVED"; break;
            case OPERATOR: std::cout << "OPERATOR"; break;
            case DELIMITER: std::cout << "DELIMITER"; break;
            case IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case UNKNOWN: std::cout << "UNKNOWN"; break;
        }
        std::cout << ">" << std::endl;
    }
}

int main() {
    // test1
    std::string code = R"(
    #include <iostream>
    int main() {
        // This is a comment
        int x = 0x12;
        float y = 3.14;
        std::cout << "Hello, World!" << std::endl;
        return 0;
    }
    )";

    // File name to read from
    std::string filename = "cpp2.txt";

    // Open the file
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return 1;
    }

    // String to store the text
    std::string text;
    char buffer[101];  // Buffer to hold 100 symbols plus null-terminator

    // Read 100 symbols at a time until the end of the file
    while (file.read(buffer, 100) || file.gcount() > 0) {
        buffer[file.gcount()] = '\0';  // Null-terminate the buffer

        text += buffer;
    }

    // Close the file
    file.close();


    std::vector<Token> tokens;
    tokenize(text, tokens);

    printTokens(tokens);

    return 0;
}
