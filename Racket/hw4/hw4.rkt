;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-intermediate-lambda-reader.ss" "lang")((modname hw4) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
;Homework 4 - Nolan Winkler
(require racket/list)
(require 2htdp/image)

;Sorting Test Lists
(define salist '("a" "b" "c" "d" "f"))
(define salist2 '("e" "g" "l"))
(define ualist '("b" "c" "f" "a" "d"))
(define snlist '(1 5 7 12 13 17 24 75))
(define snlist2 '(3 16 18 19 25 50 74 76 199))
(define unlist '(5 7 1 12 24 13 17 75))
(define uilist (build-list 5 (lambda (x) (square (random 30) "solid" "black"))))
(define tilist (list (square 5 "solid" "red") (square 17 "outline" "blue")))

;Part 1 - Sorting

;;isort: (listof a) (a a -> bool) -> (listof a)
;Returns a list that is the result of sorting l by sortfun
;in the "insertion sort" fashion
(define (isort l sortfun)
  (local
    ;;polyinsert: a (listof a) (a a -> bool) -> (listof a)
    ;inserts an element a in a sorted list l in its proper position
    ;according to sortfun
    {(define (polyinsert a l sortfun)
       (cond
         [(empty? l) (cons a empty)]
         [(sortfun (first l) a) 
     (cons (first l) (polyinsert a (rest l) sortfun))]
    [else (cons a l)]))}
    (foldr (λ (a l) (polyinsert a l sortfun)) empty l)))

(check-expect (isort unlist <) snlist)
(check-expect (isort unlist >) (list 75 24 17 13 12 7 5 1))
(check-expect (isort ualist string<?) salist)
"eyeball test isort for images"
(isort (build-list 5 (lambda (x) (square (random 50) "solid" "black"))) 
       (lambda (x y) (<= (image-height x) (image-height y))))

;;qsort: (listof a) (a a -> bool) -> (listof a)
;Returns a list that is the result of sorting l by sortfun
;in the "quick sort" fashion
(define (qsort l sortfun)
  (cond
    [(empty? l) l]
    [else (append (qsort (filter (λ (e) (sortfun e (first l))) l) sortfun)
                  (cons (first l) empty)
                  (qsort (filter (λ (e) (sortfun (first l) e)) l) sortfun))]))

(check-expect (qsort unlist <) snlist)
(check-expect (qsort unlist >) (list 75 24 17 13 12 7 5 1))
(check-expect (qsort ualist string<?) salist)
"eyeball test qsort for images"
(qsort (build-list 5 (lambda (x) (square (random 50) "solid" "black"))) 
       (lambda (x y) (< (image-height x) (image-height y))))

;;msort: (listof a) (a a -> bool) -> (listof a)
;Returns a list that is the result of sorting l by sortfun
;in the "merge sort" fashion
(define (msort l sortfun)
  (local
    ;;polymerge: (listof a) (listof a) (a a -> bool) -> (listof a)
    ;Returns a list that is sorted by sortfun and contains all of the elements
    ;of sorted lists l1 and l2
    {(define (polymerge l1 l2 sortfun)
       (cond
         [(empty? l1) l2]
         [(empty? l2) l1]
         [(sortfun (first l2) (first l1)) 
          (cons (first l2) (polymerge (rest l2) l1 sortfun))]
         [else 
          (cons (first l1) (polymerge (rest l1) l2 sortfun))]))}
    (cond
      [(empty? (rest l)) l]
      [else (polymerge (msort (take l (floor (/ (length l) 2))) sortfun)
                       (msort (drop l (floor (/ (length l) 2))) sortfun)
                       sortfun)])))

(check-expect (msort unlist <) snlist)
(check-expect (msort unlist >) (list 75 24 17 13 12 7 5 1))
(check-expect (msort ualist string<?) salist)
"eyeball test msort for images"
(msort (build-list 5 (lambda (x) (square (random 50) "solid" "black"))) 
       (lambda (x y) (< (image-height x) (image-height y))))

;Gratuitous Helper Sorting Function Tests

(define (polyinsert a l sortfun)
  (cond
    [(empty? l) (cons a empty)]
    [(sortfun (first l) a) 
     (cons (first l) (polyinsert a (rest l) sortfun))]
    [else (cons a l)]))

(check-expect (polyinsert "e" salist string<?); string in middle example
              (list "a" "b" "c" "d" "e" "f"))
(check-expect (polyinsert -1 snlist <); num at beg example
              (list -1 1 5 7 12 13 17 24 75))
"eyeball test polyinsert for images - img at end example"
(polyinsert (circle 23 "solid" "green") 
            tilist
            (λ (i1 i2) (< (image-width i1) (image-width i2))))

(define (polymerge l1 l2 sortfun)
  (cond
    [(empty? l1) l2]
    [(empty? l2) l1]
    [(sortfun (first l2) (first l1)) 
     (cons (first l2) (polymerge (rest l2) l1 sortfun))]
    [else 
     (cons (first l1) (polymerge (rest l1) l2 sortfun))]))

(check-expect (polymerge snlist snlist2 <)
              (list 1 3 5 7 12 13 16 17 18 19 24 25 50 74 75 76 199))
(check-expect (polymerge salist salist2 string<?)
              (list "a" "b" "c" "d" "e" "f" "g" "l"))
"eyeball test for polymerge for images"
(polymerge 
 (qsort (build-list 5 (lambda (x) (square (random 50) "solid" "black"))) 
        (lambda (x y) (< (image-height x) (image-height y))))
 (qsort (build-list 5 (lambda (x) (square (random 50) "solid" "black"))) 
        (lambda (x y) (< (image-height x) (image-height y))))
 (lambda (x y) (<= (image-height x) (image-height y))))


;Part 2 - Expression Trees

;; a bool-tree is either
;; - a boolean (true or false),
;; - a (make-Not t) for t bool-tree,
;; - a (make-And tL tR) for tL, tR bool-trees, or
;; - a (make-Or tL tR) for tL, tR bool-trees
(define-struct Not (t))
(define-struct And (tL tR))
(define-struct Or (tL tR)) 

;num-levels: bool-tree -> num
;Returns the number of levels in the boolean tree, that is the
;number of nodes on the path from the root node to its most distant leaf node
(define (num-levels bt)
  (cond
    [(boolean? bt) 1]
    [(Not? bt) (+ 1 (num-levels (Not-t bt)))]
    [(And? bt) (+ 1 
                  (if  (> (num-levels (And-tL bt)) (num-levels (And-tR bt)))
                       (num-levels (And-tL bt)) (num-levels (And-tR bt))))]
    
    [(Or? bt) (+ 1 
                 (if  (> (num-levels (Or-tL bt)) (num-levels (Or-tR bt)))
                      (num-levels (Or-tL bt)) (num-levels (Or-tR bt))))]))

(check-expect 5 (num-levels 
                 (make-Not (make-Not (make-Or (make-And true false) false)))))
(check-expect 1 (num-levels true))
(check-expect 6 (num-levels 
                 (make-Not (make-And (make-Or (make-And (make-Not true)
                                                        false)
                                              true) 
                                     true)))) 

;fringe: bool-tree -> (listof bool)
;Returns, in order from L to R, the list of booleans on the edges of bt
(define (fringe bt)
  (cond
    [(boolean? bt) (cons bt empty)]
    [(Not? bt) (fringe (Not-t bt))]
    [(And? bt) (append (fringe (And-tL bt))
                       (fringe (And-tR bt)))]
    [(Or? bt) (append (fringe (Or-tL bt))
                      (fringe (Or-tR bt)))]))

(check-expect (fringe 
               (make-Not (make-Not (make-Or (make-And true false) false))))
              (list true false false))
(check-expect (list true true false false true false)
              (fringe 
               (make-Not (make-And true 
                                   (make-Or (make-And (make-Or true false)
                                                      false) 
                                            (make-And true false))))))

(check-expect (list false false true true false true)
              (fringe
               (make-Or (make-And (make-Or false false) true) 
                        (make-Or (make-And true false) true))))
              
;unparse: bool-tree -> string
;Returns the Racket expression that if one were to enter, it would evaluate the tree
(define (unparse bt)
  (cond
    [(boolean? bt) (if (identity bt) "true" "false")]
    [(Not? bt) (string-append "(not" " " (unparse (Not-t bt)) ")")]
    [(And? bt) (string-append "(and"
                              " "
                              (unparse (And-tL bt))
                              " "
                              (unparse (And-tR bt))
                              ")")]
    [(Or? bt) (string-append "(or"
                             " " 
                             (unparse (Or-tL bt))
                             " " 
                             (unparse (Or-tR bt))
                             ")"
                             )]))

(check-expect "false" (unparse false)); 1-level example
(check-expect "(or true false)" (unparse (make-Or true false))); 2-level example
(check-expect "(or (and true false) true)"; 3-level example
              (unparse (make-Or (make-And true false) true)))
(check-expect ;()-checking example 1
 "(not (not (or (and true false) false)))"
 (unparse (make-Not (make-Not (make-Or (make-And true false) false)))))
(check-expect; ()-checking example 2 
 "(not (and true (or (and (or true false) false) (and true false))))"
 (unparse (make-Not (make-And true 
                              (make-Or (make-And 
                                        (make-Or true false) 
                                        false) 
                                       (make-And true false))))))
(check-expect; ()-checking example 3
 "(or (and (or false false) true) (or (and true false) true))"
 (unparse (make-Or (make-And 
                    (make-Or false false) 
                    true) 
                   (make-Or 
                    (make-And true false) 
                    true))))