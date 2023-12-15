#include <fstream>
#include <iostream>
#include <regex>
#include <stack>
#include <string>
#include <vector>

using namespace std;

// Constants
const string INPUT_FILE_EXTENSION = ".htmlf";
const string OUTPUT_FILE_NAME = "index.html";

// Enum for tags
enum class HtmlTag { H1, P, Div, EndDiv };

struct Type {
    HtmlTag tag;
    string data;

    Type(HtmlTag t, const string& d) : tag(t), data(d) {}
};

// Function prototypes
void init(const string& file, ifstream& ifile, ofstream& ofile);
vector<Type> tokenize(ifstream& ifile);
void generate(const vector<Type>& tokens, ofstream& ofile);

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input-file>" << endl;
        return 1;
    }

    string file = argv[1];
    string sub = file.substr(file.find_last_of("."));
    if (file.substr(file.size() - INPUT_FILE_EXTENSION.size()) !=
        INPUT_FILE_EXTENSION) {
        cerr << "Input file must be a " << INPUT_FILE_EXTENSION << " file!"
             << endl;
        return 1;
    }

    ifstream ifile;
    ofstream ofile;

    init(file, ifile, ofile);
    generate(tokenize(ifile), ofile);

    // File streams are closed automatically when they go out of scope

    return 0;
}

void init(const string& file, ifstream& ifile, ofstream& ofile) {
    ifile.open(file);
    if (!ifile.is_open()) {
        cerr << "Failed to open input file: " << file << endl;
        exit(1);
    }

    ofile.open(OUTPUT_FILE_NAME);
    if (!ofile.is_open()) {
        cerr << "Failed to create/open output file: " << OUTPUT_FILE_NAME
             << endl;
        exit(1);
    }

    ofile << R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
          <meta charset="UTF-8">
          <meta name="viewport" content="width=device-width, initial-scale=1.0">
          <title>Document</title>
        </head>
        <body>
    )" << endl;
}

vector<Type> tokenize(ifstream& ifile) {
    vector<Type> tokens;
    string line;
    stack<HtmlTag> tagStack;

    regex pattern_data("\"(.*?)\"");
    regex pattern_tag("^(.*?)\\(");

    while (getline(ifile, line)) {
        string tag = "";
        string data = "";
        smatch matches;
        if (regex_search(line, matches, pattern_tag)) {
            if (matches.size() > 1) {
                tag = matches[1].str();
                tag.erase(remove_if(tag.begin(), tag.end(), [](char & i){return i == ' ';}), tag.end());
            }
        }
        if (regex_search(line, matches, pattern_data)) {
            if (matches.size() > 1) {
                data = matches[1].str();
            }
        }

        if (tag == "h1" || tag == "p" || tag == "div") {
            if (tag == "h1") {
                tokens.emplace_back(HtmlTag::H1, data);
            } else if (tag == "p") {
                tokens.emplace_back(HtmlTag::P, data);
            } else if (tag == "div") {
                tokens.emplace_back(HtmlTag::Div, data);
                tagStack.push(HtmlTag::Div);
            }
        } else if (regex_match(string(line), regex(".*\\}.*"))) {
            if (!tagStack.empty() && tagStack.top() == HtmlTag::Div) {
                tokens.emplace_back(HtmlTag::EndDiv, data);
                tagStack.pop();
            }
        }
    }

    return tokens;
}

void generate(const vector<Type>& tokens, ofstream& ofile) {
    for (const auto& token : tokens) {
        switch (token.tag) {
            case HtmlTag::H1:
                ofile << "<h1>" << token.data << "</h1>" << endl;
                break;
            case HtmlTag::P:
                ofile << "<p>" << token.data << "</p>" << endl;
                break;
            case HtmlTag::Div:
                ofile << "<div>" << token.data << endl;
                break;
            case HtmlTag::EndDiv:
                ofile << "</div>" << endl;
                break;
        }
    }

    ofile << R"(
        </body>
        </html>
    )" << endl;
}
