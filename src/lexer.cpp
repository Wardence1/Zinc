#include "lexer.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

typedef enum {
    NEW,
    SPACE,
    KEYWORD,
    OPERATOR,
    NUMBER,
    IDENTIFIER,
    PUNCTUATION,
    UNDEFINED, // In the middle of a word, a token will never be set to this type
} TokenType;

typedef struct {
    TokenType type;
    std::string lexeme;
} Token;

string keywords[] = {"int", "void", "return"};
vector<Token> tokens;

// Check to see if a word is a keyword
bool isKeyword(string word) {

    for (auto w : keywords) {
        if (w == word) {
            return true;
        }
    }

    return false;
}

// @todo: support for other symbols, *, /, -(as in negative)
// Checks to see if a character is a digit
bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}
// Checks to see if a character is an operator
bool isOperator(char ch) {
    return ch == '=' || ch == '+' || ch == '-';
}
// Checks to see if a character is a letter
bool isLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

void lex(ifstream& iFile) {

    // Put file contents into a string
    ostringstream oss;
    oss << iFile.rdbuf();
    string content = oss.str();
    content += ';'; // Account for EOF

    // Setup
    string lexeme;
    bool switched = false; // If the state switches this sets to true
    TokenType state, prevState = NEW, NEW;

    // Go through file contents
    char ch;
    for (int i = 0; i < content.size(); i++) {
        
        ch = content[i];

        /* Tokenize Everything */
        if (isDigit(ch)) { // number
            if (state != NUMBER) {
                switched = true;
                state = NUMBER;
            }
        } else if (isOperator(ch)) { // operator
            if (state != OPERATOR) {
                switched = true;
                state = OPERATOR;
            }
        } else if (ch == ' ') { // space
            if (state != SPACE) {
                switched = true;
                state = SPACE;
            }
        } else if (ch == ';' || ch == '\n') { // punctuation
            if (state != PUNCTUATION) {
                switched = true;
                state = PUNCTUATION;
            }
        } else { // undefined, this means your in the middle of an unidentified word
            if (state != UNDEFINED) {
                switched = true;
                state = UNDEFINED;
            }
        }
        
        // Save token if the state switched
        if (switched) {

            // Figure out what the unidentified string is
            if (prevState == UNDEFINED) {
                if (isKeyword(lexeme)) {
                    tokens.push_back(Token{KEYWORD, lexeme});
                } else {
                    tokens.push_back(Token{IDENTIFIER, lexeme});
                }
            } else {
                tokens.push_back(Token{state, lexeme});
            }

            switched = false;
            lexeme = "";
        }

        lexeme += ch;
        prevState = state;
    }

    for (auto token : tokens) {
        cout << token.lexeme << "\n";
    }
}