# Parts of a Language

# Frontend
## Scanning (Lexing)
- Takes linear system of characters and chunks them into a series of tokens

## Parsing
- GRAMMAR - ability to compose larger expressions out of smaller ones

## Static Analysis
- "a + b", but what do "a" and "b" refer to?
- binding / resolution:
    - For each identifier, find where that name is defined and wire the two together
    - Scope: region of code where a name can be used to refer to a certain declaration
    - Type checking 

Where do you store all this information?
- Attributes of the syntax tree
    - extra fields in the nodes that aren't initialized during parsing but get filled later
- Lookup table off the side
    - symbol table

## Intermediate Representations
- C, Pascal, Fortran -> IR -> x86, ARM

## Optimization
```
pennyArea = 3.14159 * (0.75 / 2) * (0.75 / 2);
```
```
pennyArea = 0.4417860938;
```

## Code Generation
- Convert to machine code

# Back end
## Virtual machine
- pcode / byte code
## Runtime
- Garbage collection etc

# Alternatives
- Single pass compilers
    - Context un aware / no global state 
- Tree walk interpreters
    - Run it right after parsing it into an AST
    - Not common 
- Transpiler
    - Source to source compilers
- Just in Time Compilation 
    - Hot spot JVM CLR (Microsoft) and most JAvascript interpreters
    - Compile to native code for the architecture  

# Compiler vs Interpreter 
- Compiling:
    - implementation technique
    - translating source language to some other lower level form
    - doesn't execute it 
- Interpreter 
    - executes it immediately

# Representing Code 
## Context Free Grammars
## Formal Grammar 
- Takes a set of atomic pieces (alphabet) and defines an infinite set of strings in the grammar 


## Lexical 
- Alphabet = Characters 
- String = Lexeme / token 
- Implemented by the = Scanner 

## Syntactic Grammar 
- Alphabet = Token
- String = expression 
- Implemented by the = Parser

- Terminal - letter from the grammar's alphabet ("if" / "1234" ...)
- Non terminal - reference to another rule in the grammar

Backus Naur Form 
Example: Breakfast 
```
breakfast -> protein "with" breakfast "on the side";
breakfast -> protein;
breakfast -> bread;

protein -> crispiness "crispy" "bacon";
protein -> "sausage";
protein -> cooked "eggs";

crispiness -> "really";
crispiness -> "really" crispiness;

cooked -> "scrambled";
cooked -> "poached";
cooked -> "fried";

bread -> "toast";
bread -> "biscuits";
bread -> "English Muffin";
```

Lox:
```
expression -> literal | unary | binary | grouping;
literal -> NUMBER | STRING | "true" | "false" | "nil";
grouping -> "(" expression ")";
unary -> ("-" | "!") expression;
binary -> expression operator expression;
operator -> "==" | "!=" | "<" | "<=" | ">" | ">=" | "+" | "-" | "*" | "/";
```

# Ambiguity
- different choices of productions can lead to the same string

Example: `6/3-1`
    - solution: precedence
    - Associativity - left / right associative
        - Left (e.g. minus):
            - `3 - 1 - 1 = (3 - 1) - 1`
        - Right (e.g. assignment):
            - `a = b = c == a = (b = c)`
    - Lox
        - Equality (==, !=) Left
        - Comparison (>, >=, <, <=) Left
        - Term (-, +) Left
        - Factor (/, *) Left
        - Unary (!, -) Right

          
precedence:
expression -> ...
equality -> ...
comparison -> ...
term -> ...
factor -> ...
unary -> ...
primary -> ...

Rules match precedence level or higher:
    - unary matches !negated or 1234
    - term matches 1 + 2 or 3 * 4 / 5

```
expression -> equality
equality -> comparison ( ( "!=" | "==") comparison)*;
comparison -> term (( ">" | ">=" | "<" | "<=" ) term)*;
term -> factor ( ( "+" | "-" ) factor )*;
factor -> unary ( ( "/" | "*" ) unary )*;
unary -> ( "!" | "-" ) unary | primary;
primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")";
```
https://www.google.com/search?client=firefox-b-1-e&q=proving+that+a+grammar+is+unambiguous

# Parsing 
- LL(k) LR(1) LALR Earley, Shunting yard algorithm, packrat parsing
- Recursive descent:
    - Top Down
    
- Terminal - Code to match and consume the token 
- Non terminal - Call to that rule's function 
- | if / switch
- * or + while or for loop 
- ? if statement


# Statements
```
program -> declaration* EOF;
declaration -> varDecl | statement;
varDecl -> "var" IDENTIFIER ( "=" expression )? ";";
statement -> exprStmt | printStmt;
exprStmt -> expression ";";
printStmt -> "print" expression ";";
primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER;
```


























