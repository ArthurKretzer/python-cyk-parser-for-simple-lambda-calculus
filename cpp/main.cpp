#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <set>
#include <tuple>
#include <map>
using namespace std;

using cyk_table = vector<vector<vector<string>>>;
using cyk_result = std::tuple<cyk_table, bool>;
using grammar_type = std::map<string, vector<vector<string>>>;

// Non-terminals symbols
vector<string> terminals = {"C", "D", "G",
                            "variable"};
vector<string> non_terminals = {"S", "A", "B", "E",
                                "F", "H"};

// Rules of the grammar
grammar_type grammar = {
    {"S", {{"A", "B"}, {"E", "F"}, {"(?!lambda)[a-zA-Z]+(-[a-zA-Z]+)*"}}},
    {"A", {{"C", "S"}}},
    {"B", {{"S", "D"}}},
    {"C", {{"("}}},
    {"D", {{")"}}},
    {"E", {{"C", "G"}}},
    {"F", {{"H", "B"}}},
    {"G", {{"lambda"}}},
    {"H", {{"A", "D"}}}};

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

bool isTerminal(const std::vector<std::string> &rule_right_side)
{
    return rule_right_side.size() == 1;
}

bool isTerminalAndEqualToToken(const std::vector<std::string> &rule_right_side, const std::string &token)
{
    bool match = false;
    if (isTerminal(rule_right_side))
    {
        try
        {
            std::regex regexPattern(rule_right_side[0]);
            match = std::regex_match(token, regexPattern);
        }
        catch (...)
        {
            match = rule_right_side[0] == token;
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
    cyk_table solution_table(input_str_size + 1, std::vector<std::vector<std::string>>(input_str_size + 1));

    // Filling in the table
    for (int j = 0; j < input_str_size; j++)
    {

        // Iterate over the rules
        for (auto rules : grammar)
        {
            string rule_left_side = rules.first;
            vector<vector<string>> rule_right_side = rules.second;

            for (auto rule : rule_right_side)
            {

                // If a terminal is found
                if (isTerminalAndEqualToToken(rule, input_str[j]))
                    solution_table[j][j].push_back(rule_left_side);
            }
        }

        for (int i = j; i >= 0; i--)
        {

            // Iterate over the range from i to j
            for (int k = i; k <= j; k++)
            {

                // Iterate over the rules
                for (auto rules : grammar)
                {
                    string rule_left_side = rules.first;
                    vector<vector<string>> rule_right_side = rules.second;

                    for (std::vector<std::string> rule : rule_right_side)
                    {
                        // If a terminal is found
                        if (rule.size() == 2)
                        {
                            string left_symbol_rule = rule[0];
                            string right_symbol_rule = rule[1];
                            // Now searches for matches in table
                            for (string left_symbol : solution_table[i][k])
                            {
                                for (string right_symbol : solution_table[k + 1][j])
                                {
                                    if ((left_symbol_rule == left_symbol) && (right_symbol_rule == right_symbol))
                                    {
                                        solution_table[i][j].push_back(rule_left_side);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // If word can be formed by rules
    // of given grammar
    // printStringVector(input_str);
    if (solution_table[0][input_str_size - 1].size() != 0)
    {
        // std::cout
        //     << "\033[1;32mAccepted!\033[0m"
        //     << std::endl;

        return std::make_tuple(solution_table, true);
    }
    else
    {
        // std::cout << "\033[1;31mRejected!\033[0m"
        //           << std::endl;
        return std::make_tuple(solution_table, false);
    }
}

// C++ version of the Node class
class Node
{
public:
    std::string value;
    std::string terminal_value;
    Node *left;
    Node *right;
    std::vector<std::string> possible_symbols;

    Node(const std::string &value, const std::string &terminal_value = "")
        : value(value), terminal_value(terminal_value), left(nullptr), right(nullptr)
    {
        getPossibleSymbols(value);
    }

    void addLeft(Node *node)
    {
        left = node;
    }

    void addRight(Node *node)
    {
        right = node;
    }

private:
    void getPossibleSymbols(const std::string &symbol)
    {

        auto it = grammar.find(symbol);
        if (it != grammar.end())
        {
            const std::vector<std::vector<std::string>> &symbolRules = it->second;

            for (const std::vector<std::string> &rule : symbolRules)
            {
                if (!rule.empty())
                {
                    for (const std::string &ruleSymbol : rule)
                    {
                        possible_symbols.push_back(ruleSymbol);
                    }
                }
            }
        }
    }
};

// C++ version of the search_left function
std::tuple<std::string, int> searchLeft(
    int initialXIndex, const std::vector<std::vector<std::string>> &line, const std::vector<std::string> &possibleSymbols)
{
    for (int xIndex = initialXIndex - 1; xIndex >= 0; --xIndex)
    {
        if (!line[xIndex].empty() && (std::find(possibleSymbols.begin(), possibleSymbols.end(), line[xIndex][0]) != possibleSymbols.end()))
        {
            return std::make_tuple(line[xIndex][0], xIndex);
        }
    }
    return std::make_tuple("", -1);
}

std::tuple<std::string, int> searchRight(
    int initialYIndex, const std::vector<std::vector<std::string>> &line, const std::vector<std::string> &possibleSymbols)
{
    for (std::size_t yIndex = initialYIndex + 1; yIndex < line.size(); ++yIndex)
    {
        if (!line[yIndex].empty() && (std::find(possibleSymbols.begin(), possibleSymbols.end(), line[yIndex][0]) != possibleSymbols.end()))
        {
            return std::make_tuple(line[yIndex][0], static_cast<int>(yIndex));
        }
    }
    return std::make_tuple("", -1);
}

// C++ version of the search_nodes function
std::tuple<std::string, std::vector<int>, std::string, std::vector<int>> searchNodes(
    const cyk_table &table,
    int xIndex,
    int yIndex,
    const std::vector<std::string> &possibleSymbols)
{
    const std::vector<std::vector<std::string>> &xLine = table[yIndex];

    std::string leftValue;
    int leftIndex;
    std::tie(leftValue, leftIndex) = searchLeft(xIndex, xLine, possibleSymbols);
    std::vector<int> leftPoint = {leftIndex, yIndex};

    std::string rightValue;
    int rightIndex;

    std::vector<std::vector<std::string>> yLine;

    for (const auto &line : table)
    {
        yLine.push_back(line[xIndex]);
    }

    std::tie(rightValue, rightIndex) = searchRight(yIndex, yLine, possibleSymbols);
    std::vector<int> rightPoint = {xIndex, rightIndex};

    return std::make_tuple(leftValue, leftPoint, rightValue, rightPoint);
}

// C++ version of the build_tree function
Node *buildTree(const cyk_table &table, const std::vector<std::string> &inputSplitted)
{
    int inputLength = inputSplitted.size();
    Node *initialNode = new Node(table[0][inputLength - 1][0]);

    std::vector<int> initialPoint = {inputLength - 1, 0};
    if (initialPoint[0] == initialPoint[1])
    {
        initialNode->terminal_value = inputSplitted[0];
        return initialNode;
    }

    std::vector<std::pair<Node *, std::vector<int>>> queue = {std::make_pair(initialNode, initialPoint)};

    while (!queue.empty())
    {
        Node *node = queue.front().first;
        std::vector<int> point = queue.front().second;
        queue.erase(queue.begin());

        int xIndex = point[0];
        int yIndex = point[1];
        std::string leftSymbol;
        std::vector<int> leftPoint;
        std::string rightSymbol;
        std::vector<int> rightPoint;
        std::tie(leftSymbol, leftPoint, rightSymbol, rightPoint) = searchNodes(table, xIndex, yIndex, node->possible_symbols);

        Node *leftNode;
        if (leftPoint[0] == leftPoint[1] && leftSymbol == "S")
        {
            leftNode = new Node(leftSymbol, inputSplitted[leftPoint[0]]);
        }
        else
        {
            leftNode = new Node(leftSymbol);
        }

        Node *rightNode;
        if (rightPoint[0] == rightPoint[1] && rightSymbol == "S")
        {
            rightNode = new Node(rightSymbol, inputSplitted[rightPoint[0]]);
        }
        else
        {
            rightNode = new Node(rightSymbol);
        }

        node->addLeft(leftNode);
        node->addRight(rightNode);

        if (leftPoint[0] != leftPoint[1])
        {
            queue.push_back(std::make_pair(leftNode, leftPoint));
        }

        if (rightPoint[0] != rightPoint[1])
        {
            queue.push_back(std::make_pair(rightNode, rightPoint));
        }
    }

    return initialNode;
}

std::vector<std::string> breadthSearchForVariables(Node *root);

std::vector<std::string> getIndependentVariables(
    const std::vector<std::string> &lambdaVariables, const std::vector<std::string> &possibleIndependentVariables)
{
    std::vector<std::string> independentVariables;
    for (const std::string &variable : possibleIndependentVariables)
    {
        if (std::find(lambdaVariables.begin(), lambdaVariables.end(), variable) == lambdaVariables.end())
        {
            independentVariables.push_back(variable);
        }
    }
    return independentVariables;
}

std::vector<std::string> breadthSearchConsideringLambdaVariables(Node *root)
{
    std::vector<std::string> variables;

    if (root == nullptr)
        return variables;

    std::vector<std::string> lambdaVariables = breadthSearchForVariables(root->left);
    std::vector<std::string> possibleIndependentVariables = breadthSearchForVariables(root->right);

    variables = getIndependentVariables(lambdaVariables, possibleIndependentVariables);

    return variables;
}

std::vector<std::string> breadthSearchForVariables(Node *root)
{
    std::vector<std::string> variables;

    if (root == nullptr)
        return variables;

    if (root->value == "F")
    {
        std::vector<std::string> independentVariables = breadthSearchConsideringLambdaVariables(root);
        return independentVariables;
    }

    std::vector<std::string> independentVariables = breadthSearchForVariables(root->left);
    variables.insert(variables.end(), independentVariables.begin(), independentVariables.end());

    independentVariables = breadthSearchForVariables(root->right);
    variables.insert(variables.end(), independentVariables.begin(), independentVariables.end());

    if (!root->terminal_value.empty())
    {
        variables.push_back(root->terminal_value);
    }

    return variables;
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

    int _case = 1;
    for (const std::string &inputStr : inputStrs)
    {
        std::vector<std::string> splitStrings = splitInputString(inputStr);
        // Function Call
        cyk_result result = cykParse(splitStrings);

        cyk_table table = std::get<0>(result);
        bool accepted = std::get<1>(result);
        if (accepted)
        {
            Node *start = buildTree(table, splitStrings);
            vector<string> independent_variables = breadthSearchForVariables(start);
            cout << "Case #" << _case << ":";
            for (const string &variable : independent_variables)
            {
                cout << " " << variable;
            }
            cout << endl;
        }
        _case++;
    }

    return 0;
}