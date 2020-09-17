I dug this weird little unfinished C implementation of a Lisp out of my Dropbox (dated 2015). It seems to be based on that one [Peter Norvig article](https://norvig.com/lispy.html). Writing C is tricky.

Looking back, it has a fun way of parsing code and making an S-expression tree in one pass. The output is nice and colorful, too. Sadly, it just keeps allocating stuff on a heap until it runs out and dies.

It's 2020 as I write this, so maybe if I let it sit on GitHub for five more years, by 2025 I'll have figured out how to implement garbage collection.

# Usage

```
make
./cfunc "(begin (define fac (lambda (x) (if (= x 0) 1 (* x (fac (+ -1 x)))))) (fac 7))"
```
