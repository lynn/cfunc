(begin
  (define factorial (lambda (n)
    (if (<= n 0) 1 (* n (factorial (- n 1))))))
  (factorial 7)
)
