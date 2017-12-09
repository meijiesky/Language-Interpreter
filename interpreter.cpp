#include <iterator>
#include <iostream>
#include <string>
#include <regex>
#include <list>
#include <algorithm>
#include <utility>
#include <map>
#include<stdio.h>


using namespace std;


map<string, string> patterns{
    { "\\*|\\+|\\-",                 "Operator" },
    { "[a-zA-Z_][a-zA-Z_0-9]*",      "Identifier" },
    { "0|[1-9][0-9]*",               "Literal" },
    { "\\(",                         "Lp" },
    { "\\)",                         "Rp" },
    { "\\;",                         "EOS" },
    { "\\=",                         "Equal" },
    { "[^a-zA-Z_0-9\\+\\-\\*\\;\\=]","Other" }
};


// Lexer: get tokens
map<size_t, pair<string, string>> lexer(string s) {
    // storage for results
    map<size_t, pair<string, string>> matches;

    for (auto pat = patterns.begin(); pat != patterns.end(); ++pat)
    {
        regex r(pat->first);
        auto words_begin = sregex_iterator(s.begin(), s.end(), r);
        auto words_end = sregex_iterator();

        for (auto it = words_begin; it != words_end; ++it)
            // pair is used to store the token and its corresponsing type
            if (it->str() != " ")
                matches[it->position()] = make_pair(it->str(), pat->second);
    }
    return matches;
}

string peek(map<size_t, pair<string, string>>::iterator &it)
{
    return it->second.first;
}

string get(map<size_t, pair<string, string>>::iterator &it)
{
    return it++->second.first;
}

int exp(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it);

int number(map<size_t, pair<string, string>>::iterator &it)
{
    int result = stoi(get(it));
    return result;
}

int factor(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it)
{
    if (it->second.second == "Literal")
        return number(it);
    else if (peek(it) == "(")
    {
        get(it); // '('
        int result = exp(matches, it);
        get(it); // ')'
        return result;
    }
    else if (peek(it) == "-")
    {
        get(it);
        return -factor(matches, it);
    }
    else if (peek(it) == "+")
    {
        get(it);
        return factor(matches, it);
    }
    return 0; // error
}

int term(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it)
{
    int result = factor(matches, it);
    while (it != matches.end() && peek(it) == "*")
        if (get(it) == "*")
            result *= factor(matches, it);
    return result;
}

int exp(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it)
{
    int result = term(matches, it);
    while (it!= matches.end() && (peek(it) == "+" || peek(it) == "-"))
        if (get(it) == "+")
            result += term(matches, it);
        else
            result -= term(matches, it);
    return result;
}


int main()
{
    string str;
    string s = " ";

    // read in multiple lines and exit once there is no inout
    while (s.length() != 0)
    {
        cout << "Enter text line: " << endl;
        getline(cin, s);
        str = str + " " + s;
    }


    map<size_t, pair<string, string>> matches = lexer(str);

    map<size_t, pair<string, string>>::iterator it = matches.begin();
    int result = exp(matches, it);

    string var;

    while (it != matches.end()) {
        if (it->second.second == "Identifier") {
            var = it->second.first;
            it++;
            if (it->second.second == "Equal") {
                it++;
                cout << var << " = " << exp(matches, it) << endl;
                if (it->second.second == "EOS") {
                    it++;
                }
                else
                    throw runtime_error("Error");
            }
            else
                throw runtime_error("Error");
        }
        else
            throw runtime_error("Error");
    }
    return 0;
}