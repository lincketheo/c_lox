
# Scanner 
- Converts characters to tokens:
```
var a = 5;
5 + 5
```

To 

```
VAR 
IDENTIFIER 
EQUALS
NUMBER
SEMICOLON
NUMBER 
PLUS 
NUMBER
```

# Parser 
- Converts tokens to expressions 

```
5 + 5 * (9 + 0)
```

To 

```
BINARY(LITERAL, PLUS, BINARY(LITERAL, TIMES, GROUPING(LITERAL, PLUS, LITERAL)))
```

# Interpreter 
- Simplifies Expressions 

```
5 + 5 * (9 + 0)
```

To 

```
50
```
