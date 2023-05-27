#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <set>
#include <tuple>
#include <bits/stdc++.h>
using namespace std;

using cyk_result = std::tuple<std::map<int, std::map<int, std::set<std::string>>>, bool>;
using cyk_table = map<int, map<int, set<string>>>;

// Non-terminals symbols
vector<string> terminals, non_terminals;

// Rules of the grammar
unordered_map<string, vector<vector<string>>> R;

std::vector<std::string> splitInputString(const std::string &inputStr)
{
    std::regex regexPattern("[\\(\\)]|lambda|[a-zA-Z]+(?:-[a-zA-Z]+)?");
    std::vector<std::string> splitStrings;

    std::sregex_iterator iter(inputStr.begin(), inputStr.end(), regexPattern);
    std::sregex_iterator end;

    for (; iter != end; ++iter)
    {
        splitStrings.push_back(iter->str());
    }

    return splitStrings;
}

bool isTerminal(const std::vector<std::string, std::allocator<std::string>> &rule)
{
    return rule.size() == 1;
}

bool isTerminalAndEqualToToken(const std::vector<std::string, std::allocator<std::string>> &rule, const std::string &token)
{
    bool match = false;
    if (isTerminal(rule))
    {
        try
        {
            std::regex regexPattern(rule[0]);
            match = std::regex_match(token, regexPattern);
        }
        catch (...)
        {
            match = rule[0] == token;
        }
    }
    return match;
}

void printStringVector(vector<string> str)
{
    std::cout << "Input string "
              << "\033[94m";
    for (const auto &str : str)
    {
        std::cout << str << " ";
    }
    std::cout << "\033[0m is ";
}

// function to perform the CYK Algorithm
cyk_result cykParse(vector<string> input_str)
{
    int input_str_size = (int)input_str.size();

    // Initialize the table
    cyk_table T;

    // Filling in the table
    for (int j = 0; j < input_str_size; j++)
    {

        // Iterate over the rules
        for (auto rules : R)
        {
            string lhs = rules.first;
            vector<vector<string>> rule = rules.second;

            for (auto rhs : rule)
            {

                // If a terminal is found
                if (isTerminalAndEqualToToken(rhs, input_str[j]))
                    T[j][j].insert(lhs);
            }
        }
        for (int i = j; i >= 0; i--)
        {

            // Iterate over the range from i to j
            for (int k = i; k <= j; k++)
            {

                // Iterate over the rules
                for (auto rules : R)
                {
                    string lhs = rules.first;
                    vector<vector<string>> rule = rules.second;

                    for (auto rhs : rule)
                    {
                        // If a terminal is found
                        if (rhs.size() == 2 && T[i][k].find(rhs[0]) != T[i][k].end() && T[k + 1][j].find(rhs[1]) != T[k + 1][j].end())
                            T[i][j].insert(lhs);
                    }
                }
            }
        }
    }

    // If word can be formed by rules
    // of given grammar
    printStringVector(input_str);
    if (T[0][input_str_size - 1].size() != 0)
    {
        std::cout
            << "\033[1;32mAccepted!\033[0m"
            << std::endl;

        return std::make_tuple(T, true);
    }
    else
    {
        std::cout << "\033[1;31mRejected!\033[0m"
                  << std::endl;
        return std::make_tuple(T, false);
    }
}

int main()
{
    int quantity;
    std::cin >> quantity;
    std::cin.ignore(); // Ignore the newline character after reading the quantity

    std::vector<std::string> inputStrs;
    inputStrs.reserve(quantity);

    std::string input;
    for (int i = 0; i < quantity; ++i)
    {
        std::getline(std::cin, input);
        inputStrs.push_back(input);
    }

    // terminal symbols
    terminals = {"C", "D", "G",
                 "variable"};

    // non terminal symbols
    non_terminals = {"S", "A", "B", "E",
                     "F", "H"};

    // Rules
    R["S"] = {{"A", "B"}, {"E", "F"}, {"(?!lambda)[a-zA-Z]+(-[a-zA-Z]+)*"}};
    R["A"] = {{"C", "S"}};
    R["B"] = {{"S", "D"}};
    R["C"] = {{"("}};
    R["D"] = {{")"}};
    R["E"] = {{"C", "G"}};
    R["F"] = {{"H", "B"}};
    R["G"] = {{"lambda"}};
    R["H"] = {{"A", "D"}};

    for (const std::string &inputStr : inputStrs)
    {
        std::vector<std::string> splitStrings = splitInputString(inputStr);
        // Function Call
        cyk_result result = cykParse(splitStrings);

        cyk_table table = std::get<0>(result);
        bool accepted = std::get<1>(result);
        if (accepted)
        {
            // Iterate over the outer map
            for (const auto &entry : table)
            {
                int key1 = entry.first;
                const std::map<int, std::set<std::string>> &innerMap = entry.second;

                // Iterate over the inner map
                for (const auto &innerEntry : innerMap)
                {
                    int key2 = innerEntry.first;
                    const std::set<std::string> &values = innerEntry.second;

                    // Iterate over the set of strings
                    for (const std::string &value : values)
                    {
                        // Process the value
                        std::cout << "Key1: " << key1 << ", Key2: " << key2 << ", Value: " << value << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}
