# C\_LOX
My (Theo) implementation of lox in C. Following the first part (not the second C part) of this reference, so the C code is my translation of his Java code.

## Usage:
```
$ make 
$ ./clox 
>> 1 + 1
2.000000
>> 1 < 9
TRUE
>> "foo" + "bar"
foobar
>> "foo" + 1
Error: Cannot cast a String: "foo" to a number
>> true + true
2.000000
>> 1 == true
TRUE
>> 5 <= 0       
FALSE
>> end
$ make clean
```

```
$ ./clox main.lox
```
