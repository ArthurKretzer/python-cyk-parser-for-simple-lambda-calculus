#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

using cyk_table = vector<vector<vector<string>>>;
using cyk_result = std::tuple<cyk_table, bool>;
using grammar_type = std::unordered_map<std::string, std::vector<std::vector<std::string>>>;

std::vector<std::string> terminals = {"C", "D", "G", "variable"};
std::vector<std::string> non_terminals = {"S", "A", "B", "E", "F", "H"};

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
                    possible_symbols.push_back(rule[0]);
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
        if (std::find(possibleSymbols.begin(), possibleSymbols.end(), line[xIndex][0]) != possibleSymbols.end())
        {
            return std::make_tuple(line[xIndex][0], xIndex);
        }
    }
    return std::make_tuple("", -1);
}

// C++ version of the search_right function
std::tuple<std::string, int> searchRight(
    int initialYIndex, const std::vector<std::vector<std::string>> &line, const std::vector<std::string> &possibleSymbols)
{
    for (int yIndex = initialYIndex + 1; yIndex < line.size(); ++yIndex)
    {
        if (std::find(possibleSymbols.begin(), possibleSymbols.end(), line[yIndex][0]) != possibleSymbols.end())
        {
            return std::make_tuple(line[yIndex][0], yIndex);
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
    std::tie(rightValue, rightIndex) = searchRight(yIndex, table[xIndex], possibleSymbols);
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
            std::cout << "Variável '" << variable << "' é independente neste contexto." << std::endl;
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
        std::cout << "Variável: " << root->terminal_value << std::endl;
    }

    return variables;
}