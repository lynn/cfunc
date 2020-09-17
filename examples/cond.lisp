(begin
  (define sgn (lambda (x)
    (cond ((> x 0) 1)
          ((< x 0) -1)
          (#t 0))
  ))
  (list (sgn 33) (sgn -50) (sgn 0.0))
)
