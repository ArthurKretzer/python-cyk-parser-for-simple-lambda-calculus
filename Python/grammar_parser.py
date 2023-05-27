import re
import pandas as pd

# Define the grammar rules. Must be chomsk normalized.
grammar = {
    "S": [["A", "B"], ["E", "F"], [r"(?!lambda)[a-zA-Z]+(-[a-zA-Z]+)*"]],
    "A": [["C", "S"]],
    "B": [["S", "D"]],
    "C": [["("]],
    "D": [[")"]],
    "E": [["C", "G"]],
    "F": [["H", "B"]],
    "G": [["lambda"]],
    "H": [["A", "D"]],
}


def is_terminal(rule: list[str]):
    return len(rule) == 1


def is_terminal_and_equal_to_token(rule: list[list[str]], token: str):
    try:
        match = re.match(rf"{rule[0]}", token)
    except:
        match = rule[0] == token
    return is_terminal(rule) and match


def split_input_string(input_str: str) -> list[str]:
    return re.findall(r"[\\(\\)]|lambda|[a-zA-Z]+(?:-[a-zA-Z]+)?", input_str)


# CYK parsing function
def parse_cyk(input_str: str) -> tuple[bool, list[list[list[str]]], int]:
    input_splitted = split_input_string(input_str)
    input_length = len(input_splitted)

    # Create a table for memoization
    table = [[[] for _ in range(input_length)] for _ in range(input_length)]

    pd.DataFrame(table).to_csv("table_raw.csv")

    # Initialize the table
    for i in range(input_length):
        token = input_splitted[i]
        # Iterate over the rules
        for non_terminal, rule in grammar.items():
            for rhs in rule:
                # If a terminal is found
                if is_terminal_and_equal_to_token(rhs, token):
                    table[i][i] = [non_terminal]

    pd.DataFrame(table).to_csv("table_initialized.csv")

    # Fill the table
    for length in range(2, input_length + 1):
        for i in range(input_length - length + 1):
            j = i + length - 1
            for k in range(i, j):
                # Iterate over the rules
                for non_terminal, rules in grammar.items():
                    for rule in rules:
                        if len(rule) == 2:
                            left_symbol_rule = rule[0]
                            right_symbol_rule = rule[1]
                            # Now searches for matches in table
                            for left_symbol in table[i][k]:
                                for right_symbol in table[k + 1][j]:
                                    if (left_symbol_rule == left_symbol) and (
                                        right_symbol_rule == right_symbol
                                    ):
                                        table[i][j].append(non_terminal)

    pd.DataFrame(table).to_csv("table_finished.csv")

    # Check if the input string is accepted
    check = "S" in table[0][input_length - 1]
    if check:
        return check, table, input_length
    else:
        return check, [], input_length


class Node:
    def __init__(self, value: str, terminal_value: str = None):
        self.value = value
        self.terminal_value = terminal_value
        self.left = None
        self.right = None
        self.possible_symbols = [
            [item] for sublist in grammar[value[0]] for item in sublist
        ]

    def add_right(self, node):
        self.right = node

    def add_left(self, node):
        self.left = node


def search_left(
    initial_x_index: int, line: list[list[str]], possible_symbols: list[str]
) -> tuple[str, int]:
    for x_index in range(initial_x_index - 1, -1, -1):
        if line[x_index] in possible_symbols:
            return line[x_index][0], x_index


def search_right(
    initial_y_index: int, line: list[list[str]], possible_symbols: list[str]
) -> tuple[str, int]:
    for y_index in range(initial_y_index + 1, len(line)):
        if line[y_index] in possible_symbols:
            return line[y_index][0], y_index


def search_nodes(
    table: list[list[list[str]]],
    x_index: int,
    y_index: int,
    possible_symbols: list[str],
) -> tuple[str, list[int], str, list[int]]:
    left_value, left_index = search_left(x_index, table[y_index], possible_symbols)
    left_point = [left_index, y_index]
    right_value, right_index = search_right(
        y_index, [line[x_index] for line in table], possible_symbols
    )
    right_point = [x_index, right_index]

    return left_value, left_point, right_value, right_point


def build_tree(table: list[list[list[str]]], input_length: int, input_str: str) -> Node:
    input_splitted = split_input_string(input_str)
    initial_node = Node(table[0][input_length - 1])
    initial_point = [input_length - 1, 0]

    if initial_point[0] == initial_point[1]:
        initial_node.terminal_value = input_splitted[0]
        return initial_node

    queue = [(initial_node, initial_point)]

    while queue:
        node, point = queue.pop(0)
        x_index = point[0]
        y_index = point[1]
        left_symbol, left_point, right_symbol, right_point = search_nodes(
            table, x_index, y_index, node.possible_symbols
        )

        if (left_point[0] == left_point[1]) & (left_symbol == "S"):
            left_node = Node(left_symbol, terminal_value=input_splitted[left_point[0]])
        else:
            left_node = Node(left_symbol)

        if (right_point[0] == right_point[1]) & (right_symbol == "S"):
            right_node = Node(
                right_symbol, terminal_value=input_splitted[right_point[0]]
            )
        else:
            right_node = Node(right_symbol)

        node.add_left(left_node)
        node.add_right(right_node)

        if left_point[0] != left_point[1]:
            queue.append((left_node, left_point))

        if right_point[0] != right_point[1]:
            queue.append((right_node, right_point))

    return initial_node


def get_independent_variables(
    lambda_variables: list[str], possible_independent_variables: list[str]
) -> list[str]:
    independent_variables = []
    for variable in possible_independent_variables:
        if variable not in lambda_variables:
            independent_variables.append(variable)
            # print(f"Variável '{variable}' é independente neste contexto.")

    return independent_variables


def breadth_search_for_variables(root: Node) -> list[str]:
    variables = []

    if root is None:
        return variables

    if root.value == "F":
        independent_variables = breadth_search_considering_lambda_variables(root)
        return independent_variables

    independent_variables = breadth_search_for_variables(root.left)
    variables += independent_variables
    independent_variables = breadth_search_for_variables(root.right)
    variables += independent_variables

    if root.terminal_value is not None:
        variables.append(root.terminal_value)
        # print(f"Variável: {root.terminal_value}")

    return variables


def breadth_search_considering_lambda_variables(root: Node) -> list:
    variables = []

    if root is None:
        return variables

    lambda_variables = breadth_search_for_variables(root.left)
    possible_independent_variables = breadth_search_for_variables(root.right)

    variables = get_independent_variables(
        lambda_variables, possible_independent_variables
    )

    return variables


def main():
    input_strs = [
        "x",
        "y",
        "(lambda (x) (x y))",
        "(lambda (y) (x y))",
        "((lambda(x)x)(x y))",
        "(lambda (y) (lambda (z) (x (y z))))",
        "marmota",
        "lambda(x)x",
        "(lambda(x)x)",
    ]
    for input_str in input_strs:
        # Parse the input string
        is_accepted, table, input_length = parse_cyk(input_str)
        if table:
            tree = build_tree(table, input_length, input_str)
            independent_variables = breadth_search_for_variables(tree)
            print(
                f"Case #{input_strs.index(input_str)}: {' '.join(independent_variables)}"
            )
        # Print the result
        # print(f"Input string \033[94m{input_str}\033[0m is", "\033[92m\033[1maccepted\033[0m" if is_accepted else "\033[1m\033[91mrejected\033[0m")


if __name__ == "__main__":
    main()
