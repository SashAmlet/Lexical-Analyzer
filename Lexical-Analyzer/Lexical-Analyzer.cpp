#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <regex>
#include <map>
using namespace std;

string ReadFile(string path) {
    string sourceCode;

    ifstream inputFile(path);
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            sourceCode += line + "\n";
        }
        inputFile.close();
    }
    else {
        cerr << "Failed to open file." << endl;
        return "";
    }
    return sourceCode;
}


string prepareTextForLA(const string& inputText)     {
    // Removing comments (single and multi-line)
    string textWithoutComments = regex_replace(inputText, regex("//.*|/\\*.*?\\*/"), "");

    // Removing extra spaces and line breaks
    string textWithoutSpacesAndNewlines = regex_replace(textWithoutComments, regex("\\s+"), " ");

    // Removing leading and trailing spaces
    textWithoutSpacesAndNewlines = regex_replace(textWithoutSpacesAndNewlines, regex("^\\s+|\\s+$"), "");

    return textWithoutSpacesAndNewlines;
}

// Function for coloring text.
void colorizeText(const string& text, const string& color) {
    // Using ANSI Escape Code to Set Text Color
    cout << "\x1B[" << color << "m" << text << "\x1B[0m"; // Reset color after token
}

// Function for distinguishing and coloring token classes
void highlightLexemes(const string& code) {
    // We create a map, where the key is a regular expression, the value is color
    map<string, string> regexToColor = {
        { "#include <.*?>", "4" },
        { "\\b(int|float|double|char)\\b", "32" },
        { "\\b\\d+(\\.\\d+)?\\b", "31" },
        { "(\".*?\")|('.*?')", "42" },
        { "\\b(if|else|while|return|cout|cin|endl)\\b", "36" },
        { "[+\\-*/=<>]", "35" },
        { "[;{}()\\[\\]]", "33" },
        { "\\b[a-zA-Z]+\\b", "34" }
    };

    string combinedRegex;
    for (const auto& entry : regexToColor) {
        if (!combinedRegex.empty()) {
            combinedRegex += "|";
        }
        combinedRegex += "(" + entry.first + ")";
    }

    regex lexemeRegex(combinedRegex);
    smatch matches;

    size_t position = 0;
    while (regex_search(code.begin() + position, code.end(), matches, lexemeRegex)) {
        if (matches.position() > 0) {
            // Print characters before token
            cout << code.substr(position, matches.position());
        }

        if (matches.str() != "") {
            // Color the lexeme according to the map
            string matchedText = matches.str();
            for (const auto& entry : regexToColor) {
                if (regex_match(matchedText, regex(entry.first))) {
                    colorizeText(matchedText, entry.second);
                    break;
                }
            }
        }

        position += matches.position() + matches.length();
    }

    // Print the remaining text, if any
    if (position < code.length()) {
        cout << code.substr(position);
    }
}

int main()
{
    string path = "Program.txt";

    string sourceCode = ReadFile(path), preparedCode = prepareTextForLA(sourceCode);
    //cout << sourceCode << endl << preparedCode << endl; 
    highlightLexemes(preparedCode);
}