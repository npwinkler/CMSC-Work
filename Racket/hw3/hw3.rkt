;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-intermediate-lambda-reader.ss" "lang")((modname hw3) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
;Homework 3 - Nolan Winkler
(require 2htdp/image)

;Problem 1

;;Contract- nonempties: (listof lists) -> (listof lists)
;;Purpose- Returns a list of lists in inputl which are not empty
(define (nonempties inputl)
  (filter (compose not empty?) inputl))

(check-expect (nonempties (list (list 1 2) empty (list 3) empty))
              (list (list 1 2) (list 3))); given example
(check-expect (nonempties empty) empty); empty example
(check-expect (nonempties (list (list empty))) (list (list empty))); other example

;Problem 2

;;Contract - singletons: (listof a) -> (listof (listsof a))
;Purpose - Returns a list of lists of only one element, the elements of inputl
(define (singletons inputl)
  (map (λ (e) (cons e empty)) inputl))

(check-expect (singletons (list 1 2 3)) 
              (list (list 1) (list 2) (list 3))); given example
(check-expect (singletons (list 1 empty 2)) 
              (list (list 1) (list empty) (list 2))); another example
(check-expect (singletons empty) empty); empty example

;Problem 3

;;Contract - greens-only: (listof colors) -> (listof num)
;Purpose - Returns the green values of a list of colors, il
(define (greens-only il)
  (map (λ (c) (color-green c)) il))

(check-expect (greens-only (list (make-color 1 2 3) (make-color 4 5 6)))
              (list 2 5));given example
(check-expect (greens-only empty) empty);empty example
(check-expect (greens-only (list (make-color 0 0 0) (make-color 255 255 255)))
              (list 0 255));black/white example

;Problem 4

;;Contract - psqrs: (listof num) -> (listof num)
;Purpose - Returns a list of the elements in il that are perfect squares
(define (psqrs il)
  (filter (compose integer? sqrt) il))

(check-expect (psqrs (list 8 9 16 49 50)) (list 9 16 49));given example
(check-expect (psqrs (list 0 -1 9 225 2)) (list 0 9 225));example with 0
(check-expect (psqrs empty) empty);empty example

;Problem 5

;;N.B: min is already defined by Dr. Racket, so the function is called list-min
;;Contract - list-min: (listof num) -> num
;Purpose - Returns the minimum of the list of numbers, il
(define (list-min il)
  (cond
    [(empty? il) empty]
  [else (foldr min +inf.0 il)]))

(check-within (list-min (list 8 9 16 49 50)) 8 .00001); given example
(check-within (list-min (list 0 -1 1)) -1 .000001); positive and negative example
(check-expect (list-min empty) empty); empty example

;Problem 6

;; a (pair Î± Î2) is a (make-pair x y) 
;; where x is an Î± and y is a Î² 
(define-struct pair (a b))

;;Contract - partition : (Î± -> bool) (listof Î±) -> (pairof (listof Î±))
;Purpose - Divides il into two lists, one of the elements of il which were true when
;fun was applied to them and one of the elements for which fun evaluates to false.
;Returns a pair consisting of these two lists.
(define (partition fun il)
  (make-pair (filter fun il) (filter (compose not fun) il)))

(check-expect (partition even? '(1 2 3)) (make-pair '(2) '(1 3)));nume example
(check-expect (partition (λ (e) (string=? e "Wanted")) '("Wanted" "Not"))
              (make-pair '("Wanted") '("Not")));string example
(check-expect (partition identity (list true false false))
              (make-pair (list true) (list false false)));bool and mixed example

;Problem 7

;;Contract - selective-map : (Î± -> bool) (Î± -> Î2) (listof Î±) -> (listof Î2)
;Purpose - Returns the list where mapf is performed only on the elements of il
;for which filterf returned true
(define (selective-map filterf mapf il)
  (map mapf (filter filterf il)))

(check-expect (selective-map cons? length (list (list 1 2) empty (list 3)))
              (list 2 1));given example
(check-expect (selective-map identity not (list true true false)) 
              (list false false));boolean example
(check-expect (selective-map (λ (e) (> 3 (string-length e))) 
                             (λ (e) (string-append e "Map"))
                             (list "Hi" "1" "Won't Be Mapped" "NoDora")) 
              (list "HiMap" "1Map"));string example

;Problem 8

;;Contract - lls: num color -> img
;Purpose - produces a col left-leaning Sierpinski gasket of 
;left side and base length size 
(define (lls size color)
  (local
    {(define (llsut size color)
       (right-triangle size size "outline" color))
     (define (llsdt size color)
       (rotate 180 (llsut size color))) 
     (define (llstriforce size color)
       (above/align "left" (llsut (/ size 2) color)
                    (beside (llsut (/ size 2) color) 
                            (llsut (/ size 2) color))))}
    (cond
      [(<= size 2) (llstriforce size color)]
      [else (above/align "left" (lls (/ size 2) color)
                         (beside (lls (/ size 2) color) 
                                 (lls (/ size 2) color)))])))

"eyeball test lls 214 green:" (lls 214 "green")
"eyeball test lls 57 orange" (lls 57 "orange")
(check-expect (image-height (lls 412 "blue")) 412)
(check-expect (image-width (lls 412 "yellow")) 412)
                        
;Problem 9

;;Contract - twice: (a -> a) -> (a -> a)
;;Purpose - Returns a new function that is the same as applying f twice
(define (twice f)
  (compose f f))

(check-expect ((twice add1) 5) 7);given numerical example
(check-expect ((twice not) true) true);given boolean example

;Problem 10

;;Contract-  curry : (a a -> a) -> (a -> a)
;; given a binary function, f, return a new unary function which returns 
;; a unary function which will apply f to the two arguments 

(define curry (λ (fun)
                (λ (i1)
                  (λ (i2) (fun i1 i2)))))

(check-expect (((curry +) 5) 7) 12) 
(check-expect (((curry *) 6) 2) 12) 
(check-expect (((curry *) 6) 3) 18)


;; uncurry : (a -> a) -> (a a -> a)
;; given a curried function, return the original function 
;;given a unary function which returns a unary function which will apply f to the two arguments,
;;return a binary function f
(define uncurry (λ (fun)
                  (λ (i1 i2) 
                    ((fun i1) i2))))

(check-expect ((uncurry (curry +)) 5 7) (+ 5 7)); given example 1
(check-expect ((uncurry (curry *)) 12 3) 36); another binary example
(check-expect ((uncurry (curry +)) 12
                                   ((uncurry (curry /)) 8 4))
              (+ 12 (/ 8 4))); complex binary example