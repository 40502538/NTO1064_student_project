#  Boolean Expression & Truth Table Simulator (C++ OOP)

## Overview
This project is a command-line Boolean Expression Simulator written in C++ using Object-Oriented Programming (OOP).

It allows users to:
- Input Boolean expressions (e.g. (A AND B) OR (NOT C))
- Detect and explain operators
- Generate truth tables
- Save and load results

---

##  How to Compile & Run

### Compile
g++ -std=c++17 -o simulator main.cpp

### Run
./simulator

---

## How to Use

### Enter Expression
Example:
(A AND B) OR (NOT C)

### Supported Operators
- AND
- OR
- NOT
- XOR
- NAND
- NOR

---

### Save Output
After generating a table:
Save to file? (Y/N)

---

### Load File
LOAD filename.txt

---

### Exit
QUIT

---

## Class Structure

### BooleanOperator (Base Class)
- evaluate()
- getName()
- getExplanation()

### Derived Classes
- AND_Operator
- OR_Operator
- NOT_Operator
- XOR_Operator
- NAND_Operator
- NOR_Operator

### BooleanExpression
- Parses and evaluates expressions

### TruthTable
- Generates and prints tables

### Factory Function
- makeOperator()

---

## Design Decisions
- OOP structure with polymorphism
- Separation of concerns
- Recursive evaluation
- Use of STL containers

---

## Example

Input:
A AND B

Output:
| A | B | A AND B |
|---|---|----------|
| 0 | 0 |    0     |
| 0 | 1 |    0     |
| 1 | 0 |    0     |
| 1 | 1 |    1     |

---

## Limitations
- Only variables A, B, C supported
- Max 3 operators
- Limited precedence handling

---

## Future Improvements
- More variables
- Better parsing
- GUI version
- CSV export

---


