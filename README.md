# Expression Processing System Simulator

This program simulates a system that processes algebraic expressions submitted by the user, following a control string that dictates reading and evaluating operations. The system supports variables, arithmetic expressions, functions, and postponed evaluation when required variable values are missing.

---

## 📌 Control String

The control string is provided as a command-line argument and contains the following commands:

- **R** – `read`: reads an expression from standard input and pushes it into a queue  
- **C** – `calc`: attempts to evaluate the next expression in the queue

---

## 📌 Expression Format

Each expression must follow the format:


### ✔ Variables
- Represented by a single Latin letter (case-insensitive)
- Each variable holds exactly one value — the most recently computed one

### ✔ Expressions

Expressions may contain:

- integer numbers  
- floating-point numbers (with a dot as decimal separator)  
- unary operators: `+`, `-`  
- binary operators: `+`, `-`, `*`, `/`, `^`  
- parentheses `(...)`  
- variables  
- functions (case-insensitive)  
- arbitrary whitespace  

---

## 📌 Operator Precedence & Associativity

From highest to lowest:

1. Unary `+` and `-`
2. Exponentiation `^` (right-associative)
3. Multiplication and division `*`, `/`
4. Addition and subtraction `+`, `-` (left-associative)

---

## 📌 Supported Functions

Functions can have **any number of arguments**, including zero.  
Arguments are separated with `;`.

| Function | Description |
|----------|-------------|
| `MIN(a1; …; an)` | Minimum value |
| `MAX(a1; …; an)` | Maximum value |
| `AVG(a1; …; an)` | Arithmetic mean |
| `SUM(a1; …; an)` | Sum |
| `PRD(a1; …; an)` | Product |

Function names are **case-insensitive**.

---

## 📌 Working with Variables

If an expression contains a variable **without a value yet**:

- The evaluation is **postponed**
- The expression is returned to the end of the queue
- **Partially completed calculations must be preserved**

Example:  
`3 + 5*7 + (3*5) * x`  
→ postponed but simplified to `38 + 15*x`.

After a successful evaluation:

- The resulting value is assigned to the corresponding variable
- Previous values are overwritten

---

## 📌 Output Messages

For every operation:

- When reading:  
  `Expression (k) was read.`

- When evaluation succeeds:  
  `Expression (k) was calculated. The value of <var> is now <value>.`

- When postponed:  
  `Expression (k) was postponed.`

---

## 📌 Example Execution

Control string:

RRRRCCCCRCC

User input:
x = 2 * SUM(x; 4.5+2y; 1.5)
y = 5+6 * -2
z = SUM(MIN(1; 2; 3); SUM(1))
x = 56
y = 2 * x

Expected output:
Expression (1) was read.
Expression (2) was read.
Expression (3) was read.
Expression (4) was read.
Expression (1) was postponed.
Expression (2) was calculated. The value of y is now -7.
Expression (3) was calculated. The value of z is now 2.
Expression (4) was calculated. The value of x is now 30.
Expression (5) was read.
Expression (1) was calculated. The value of x is now 44.
Expression (5) was calculated. The value of y is now 88.

## 📌 Error Handling

The program must report errors **without terminating**, such as:

- invalid characters in the control string  
- `calc` command when the queue is empty  
- syntax errors in expressions  
- semantic errors (division by zero, invalid operations, etc.)

Invalid commands or expressions are skipped, and processing continues.

---

## 📌 End of Control String

If there are unevaluated expressions left in the queue:

The program should ask the user whether to:

- try evaluating them (if possible), or  
- discard them and terminate  

