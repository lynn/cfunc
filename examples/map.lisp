(begin
  (define range (lambda (lo hi)
    (if (> lo hi) #nil (cons lo (range (+ lo 1) hi)))))
  (define map (lambda (f x)
    (if (nil? x) #nil (cons (f (car x)) (map f (cdr x))))))
  (define square (lambda (x) (* x x)))
  (map square (range 1 10))
)
