#include <iterator>
#include <iostream>
#include <string>
#include <regex>
#include <list>
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

// look at the token
string peek(map<size_t, pair<string, string>>::iterator &it)
{
    return it->second.first;
}

// get the current token and increment the iterator
string get(map<size_t, pair<string, string>>::iterator &it)
{
    return it++->second.first;
}

// expression
int exp(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it, map<string, int> res);

// number
int number(map<size_t, pair<string, string>>::iterator &it)
{
    int result = stoi(get(it));
    return result;
}

// factor
int factor(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it, map<string, int> res)
{
    if (it->second.second == "Literal")
        return number(it);
    else if (it->second.second == "Identifier")
    {
        map<string, int>::iterator p;
        p = res.find(it++->second.first);
        if (p == res.end())
            throw  runtime_error("Error");
        else
            return p->second;
    }
    else if (peek(it) == "(")
    {
        get(it); // '('
        int result = exp(matches, it, res);
        get(it); // ')'
        return result;
    }
    else if (peek(it) == "-")
    {
        get(it);
        return -factor(matches, it, res);
    }
    else if (peek(it) == "+")
    {
        get(it);
        return factor(matches, it, res);
    }
    return 0;
}

// term
int term(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it, map<string, int> res)
{
    int result = factor(matches, it, res);
    while (it != matches.end() && peek(it) == "*")
        if (get(it) == "*")
            result *= factor(matches, it, res);
    return result;
}

// expression
int exp(map<size_t, pair<string, string>> &matches, map<size_t, pair<string, string>>::iterator &it, map<string, int> res)
{
    int result = term(matches, it, res);
    while (it != matches.end() && (peek(it) == "+" || peek(it) == "-"))
        if (get(it) == "+")
            result += term(matches, it, res);
        else
            result -= term(matches, it, res);
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

    // store variables and the outputs
    map<string, int> res;

    string var;

    while (it != matches.end()) {
        if (it->second.second == "Identifier") {
            var = it->second.first;
            it++;
            if (it->second.second == "Equal") {
                it++;
                int temp = exp(matches, it, res);
                cout << var << " = " << temp << endl;
                res[var] = temp;
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