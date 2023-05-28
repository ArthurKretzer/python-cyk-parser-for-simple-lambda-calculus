# Python CYK Parser for simplified Lambda Calculus Grammar
This is a repo for a challenge HDU - 5987 solution using Python with Cocke-Younger-Kasami (CYK) parser and breadth first search (BFS) algorithms.

Note that this code was rejected by [the virtual judge](https://vjudge.net/problem/HDU-5987), but it respected the expected output.

## Python

You can run this code with Python 3.11 by executing grammar_parser.py.

```
python3 -m pip install -r requirements.txt
python3 grammar_parser.py
```

If you want to visualize the results, you can use it with pandas. CSV files are generated with the parsing tables from CYK algorithm. Otherwise lines 42, 54 and 75 should be commented.

## C++

To run the c++ code you need to have standard c++ dependencies intalled. I used WSL Ubuntu 22.04 to run it.
Then you should execute the Makefile with
```bash 
make run
```
This will compile and execute the code with the input provided on sample_input.txt.