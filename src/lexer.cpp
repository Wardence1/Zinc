#include "lexer.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

typedef enum {
    SPACE, // " "
    INT_KEYWORD, // "int"
    VOID_KEYWORD, // "void"
    RETURN_KEYWORD, // "return"
    PLUS_OPERATOR, // "+"
    MINUS_OPERATOR, // "-"
    EQUALS_OPERATOR, // "="
    INTEGER_LITERAL, // contant integer
    FLOATING_LITERAL, // constant fraction  //@todo: make the language support this
    IDENTIFIER, // variable/function names
    PUNCTUATION, // ";" or "\n"
    BEG_CURLY, // "{"
    END_CURLY, // "}"
    BEG_PAREN, // "("
    END_PAREN, // ")"
} TokenType;

typedef enum {
    NEW_SM, // What a State Machine starts as
    SPACE_SM, // " ", "\t"
    NUMBER_SM, // All real numbers
    OPERATOR_SM, // "+", "-", "="
    PUNCTUATION_SM, // "\n", ";"
    GROUPING_SM, // "{", "}", "(", ')'
    UNDEFINED_SM, // In the middle of a word, a token will never be set to this type
} State_Machine;

typedef struct {
    TokenType type;
    std::string lexeme;
} Token;

string keywords[] = {"int", "void", "return"};
vector<Token> tokens;

// Check to see if a word is a keyword
bool isKeyword(string word) {

    for (string w : keywords) {
        if (w == word) {
            return true;
        }
    }

    return false;
}

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

// Checks to see if a string is full of numbers, this includes '-' and '.'
bool isNumber(string str) {

    for (char c : str) {
        if ((c > '9' || c < '0') && c != '.' && c != '-') return false;
    }
    return true;
}

TokenType getOperatorType(string op) {

    if (op.size() > 1) {
        fprintf(stderr, "\"%s\" is not a valid operator\n", op.c_str());
        exit(1);
    }

    TokenType type;

    switch (op[0]) {
        case '+':
            type = PLUS_OPERATOR;
            break;
        case '-':
            type = MINUS_OPERATOR;
            break;
        case '=':
            type = EQUALS_OPERATOR;
            break;
        default:
            fprintf(stderr, "\"%s\" is not a valid operator\n", op.c_str());
            exit(1);
    }

    return type;
}

TokenType getGrouperType(string str) {

    if (str.size() > 1) {
        fprintf(stderr, "\"%s\" is not a valid grouper\n", str.c_str());
        exit(1);
    }

    TokenType type;

    switch (str[0]) {
        case '{':
            type = BEG_CURLY;
            break;
        case '}':
            type = END_CURLY;
            break;
        case '(':
            type = BEG_PAREN;
            break;
        case ')':
            type = END_PAREN;
            break;
        default:
            fprintf(stderr, "\"%s\" is not a valid grouper\n", str.c_str());
            exit(1);
    }

    return type;
}

TokenType getNumberType(string str) {

    if (!isNumber(str)) {
        fprintf(stderr, "\"%s\" is not a number\n", str.c_str());
        exit(1);
    }

    TokenType type;

    for (char c : str) {
        if (c == '.') return FLOATING_LITERAL;
    }

    return INTEGER_LITERAL;
}

// Sets the identifier of a keyword in tokens
void setKeyword(vector<Token>& tokens, string lexeme) {

    TokenType type;

    if (lexeme == "int") type = INT_KEYWORD;
    else if (lexeme == "void") type = VOID_KEYWORD;
    else if (lexeme == "return") type = RETURN_KEYWORD;
    else {fprintf(stderr, "\"%s\", isn't a valid keyword.\n", lexeme.c_str()); exit(1);}

    tokens.push_back(Token{type, lexeme});
}

// Sets the type of a token
void setType(vector<Token>& tokens, string lexeme, State_Machine prevState) {

    TokenType type;

    switch (prevState) {
        case PUNCTUATION_SM:
            type = PUNCTUATION;
            break;
        case SPACE_SM:
            type = SPACE;
            break;
        case OPERATOR_SM:
            type = getOperatorType(lexeme);
            break;
        case NUMBER_SM:
            type = getNumberType(lexeme);
            break;
        case GROUPING_SM:
            type = getGrouperType(lexeme);
            break;
    }

    tokens.push_back(Token{type, lexeme});
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
    State_Machine state, prevState = NEW_SM, NEW_SM;

    // Go through file contents
    char ch;
    for (int i = 0; i < content.size(); i++) {
        
        ch = content[i];

        /* Tokenize Everything */
        if (isDigit(ch)) { // number
            if (state != NUMBER_SM) {
                switched = true;
                state = NUMBER_SM;
            }
        } else if (isOperator(ch)) { // operator
            if (state != OPERATOR_SM) {
                switched = true;
                state = OPERATOR_SM;
            }
        } else if (ch == ' ' || ch == '\t') { // space
            if (state != SPACE_SM) {
                switched = true;
                state = SPACE_SM;
            }
        } else if (ch == ';' || ch == '\n') { // punctuation
            if (state != PUNCTUATION_SM) {
                switched = true;
                state = PUNCTUATION_SM;
            }
        } else if (ch == '{' || ch == '}' || ch == '(' || ch == ')') { // grouping
            switched = true;
            state = GROUPING_SM;
        } else { // undefined, this means your in the middle of an unidentified word
            if (state != UNDEFINED_SM) {
                switched = true;
                state = UNDEFINED_SM;
            }
        }
        
        // Save token if the state switched
        if (switched) {

            // Figure out what the unidentified string is
            if (prevState == UNDEFINED_SM) {
                if (isKeyword(lexeme)) {
                    setKeyword(tokens, lexeme);
                } else {
                    tokens.push_back(Token{IDENTIFIER, lexeme});
                }
            } else {
                setType(tokens, lexeme, prevState);
            }

            switched = false;
            lexeme = "";
        }

        lexeme += ch;
        prevState = state;
    }

    /* DEBUG */
    for (auto token : tokens) {
        switch (token.type) {
            case 0:
                cout << token.lexeme + " : SPACE\n";
                break;
            case 1:
                cout << token.lexeme + " : U32\n";
                break;
            case 2:
                cout << token.lexeme + " : VOID\n";
                break;
            case 3:
                cout << token.lexeme + " : RETURN\n";
                break;
            case 4:
                cout << token.lexeme + " : PLUS\n";
                break;
            case 5:
                cout << token.lexeme + " : MINUS\n";
                break;
            case 6:
                cout << token.lexeme + " : EQUALS\n";
                break;
            case 7:
                cout << token.lexeme + " : INTEGER\n";
                break;
            case 8:
                cout << token.lexeme + " : FLOATING\n";
                break;
            case 9:
                cout << token.lexeme + " : IDENTIFER\n";
                break;
            case 10:
                cout << token.lexeme + " : PUNCTUATION\n";
                break;
            case 11:
                cout << token.lexeme + " : BEGINNING CURLY\n";
                break;
            case 12:
                cout << token.lexeme + " : END CURLY\n";
                break;
            case 13:
                cout << token.lexeme + " : BEGINNING PAREN\n";
                break;
            case 14:
                cout << token.lexeme + " : END PAREN\n";
                break;
            default:
                fprintf(stderr, "\"%d\" is not a valid type\n", token.type);
                exit(1);
        }
    }
}