(begin
  (define map (lambda (f x)
    (if (nil? x) #nil (cons (f (car x)) (map f (cdr x))))))
  (define square (lambda (x) (* x x)))
  (map square (list 1 2 3 4 5))
)
