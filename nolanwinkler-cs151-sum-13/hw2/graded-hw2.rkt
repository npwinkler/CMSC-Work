;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-intermediate-lambda-reader.ss" "lang")((modname graded-hw2) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
; Homework 2 - Nolan Winkler

(require 2htdp/image)
(require racket/list)

#| The new design recipe consists of a contract, a purpose, 
the code itself, and check-expect-style tests for all functions
which do not produce images. |#

;Problem 1

; Contract - larger: num num -> num
; Purpose - Returns the largest number input of two numbers, n1 and n2.
(define (larger n1 n2)
  (if (> n1 n2) n1 n2))

(check-expect (larger 3 4) 4); n2>n1 example
(check-expect (larger 12 12) 12); n2=n1 example
(check-expect (larger 10 5) 10); n1>n2 example

;Problem 2

;Contract - width: img img -> img
;Purpose - Returns the wider image of i1 or i2, or i1 if images have equal width.
(define (wider i1 i2)
  (if (> (image-width i2) (image-width i1)) i2 i1))

"red circle wider than red square width example"
(wider (circle 10 "solid" "red") (square 19 "solid" "red"))
"red circle same width as blue circle width example"
(wider (circle 10 "solid" "red") (circle 10 "solid" "blue")) 
"red circle less wide than red square width example"
(wider (circle 10 "solid" "red") (square 21 "solid" "red"))
;#grader: You should use check expect for these tests. Eyeball tests
;are used for functions that define images that are hard to redefine
;for the sake of a test.

;Problem 3

;Contract - more-pixels?: img img -> bool
;Purpose - Returns true if i1 is larger in area than i2, returns false otherwise.
(define (more-pixels? i1 i2)
  (local
    {;Contract- image-area: img -> num 
     ;Purpose- Computes the pixel area of i
     (define (image-area i)
       (* (image-width i) (image-height i)))}
    (> (image-area i1) (image-area i2))))

(check-expect (more-pixels? (circle 10 "solid" "red") (square 21 "solid" "red")) false); i2-area<i1-area example
(check-expect (more-pixels? (circle 10 "solid" "red") (circle 10 "solid" "red")) false); i2-area=i1-area example
(check-expect (more-pixels? (circle 10 "solid" "red") (square 9 "solid" "red")) true); i2-area>i1-area example

;Problem 4

;Contract - leap?: num -> bool
;Purpose - Returns true if year is a leap year, else returns false
(define (leap? year)
  (or (= (modulo year 400) 0) 
      (and (= (modulo year 4) 0) 
           (not (= (modulo year 100) 0)))))

(check-expect (leap? 1600) true); divisible by 400 example
(check-expect (leap? 1300) false); divisible by 100 but not 400 example
(check-expect (leap? 1996) true); divislbe by 4 but not 100 example
(check-expect (leap? 2001) false); not divisible by 4 example

;Problem 5

;Contract - next-odd: num -> num
;Purpose - If n is odd, returns n. If n is even, returns n+1.
(define (next-odd n)
  (cond
    [(even? n) (+ 1 n)]
    [else n]))

(check-expect (next-odd 5) 5); positive odd example
(check-expect (next-odd 212) 213); positive even example
(check-expect (next-odd 0) 1); 0 example
(check-expect (next-odd -5) -5); negative odd example
(check-expect (next-odd -4) -3); negative even example
;#grader: Your function does not take into account non-integer inputs
;Ceiling would be helpful in solving this problem.
;Problem 6

;Contract- color=?: color color -> bool
;Purpose- Returns true if two colors' RGBs are exactly the same, returns false otherwise.
(define (color=? c1 c2)
  (and (= (color-red c1) (color-red c2)) 
       (= (color-green c1) (color-green c2)) 
       (= (color-blue c1) (color-blue c2))))

(check-expect (color=? (make-color 100 222 014) (make-color 100 222 014)) true); RGB same example
(check-expect (color=? (make-color 100 222 014) (make-color 101 222 014)) false); R different example
(check-expect (color=? (make-color 100 222 014) (make-color 100 212 014)) false); G different example
(check-expect (color=? (make-color 100 222 014) (make-color 100 222 114)) false); B different example
(check-expect (color=? (make-color 100 222 014) (make-color 101 212 114)) false); all different example

;Problem 7

;Contract- luminance: color -> num
;Purpose - Returns the luminance (0.2126r + 0.7152g + 0.0722b) of inputcolor
(define (luminance inputcolor)
  (+ (* .2126 (color-red inputcolor))
     (* .7152 (color-green inputcolor))
     (* .0722 (color-blue inputcolor))))

(check-expect (luminance (make-color 100 222 014)) 181.0452); color example
(check-expect (luminance (make-color 0 0 0)) 0); white example
(check-expect (luminance (make-color 255 255 255)) 255); black example

;Problem 8

;Contract- more-luminous?: color color -> bool
;Purpose - Returns true if c1 is more luminous than c2, false otherwise
(define (more-luminous? c1 c2)
  (> (luminance c1) (luminance c2)))

(check-expect (more-luminous? (make-color 100 222 014) (make-color 110 222 014)) false); c2 more luminous case
(check-expect (more-luminous? (make-color 100 222 014) (make-color 100 222 014)) false); equal luminous case
(check-expect (more-luminous? (make-color 100 232 014) (make-color 100 222 014)) true); c1 more luminous case

;Problem 9

;; a russian-doll is either 
;; - (make-solid), or 
;; - (make-hollow d), where d is a russian-doll 
(define-struct solid ()) 
(define-struct hollow (d))

;Contract- num-pieces: russian-doll -> num
;Purpose- Computes the number of separate pieces 
;(2 * each hollow doll + solid doll) in rd
(define (num-pieces rd)
  (cond
    [(solid? rd) 1]
    [(hollow? rd) (+ 2 (num-pieces (hollow-d rd)))]))

(check-expect (num-pieces (make-hollow (make-hollow (make-solid)))) 5); hollows and a solid example
(check-expect (num-pieces (make-solid)) 1); just a solid example
   
;Problem 10

;Contract- num-hollows: russian-doll -> num
;Purpose- Computes the number of hollow dolls in a russian doll, rd
(define (num-hollows rd)
  (/ (- (num-pieces rd) 1) 2))

(check-expect (num-hollows (make-hollow (make-hollow (make-solid)))) 2); hollows and a solid example
(check-expect (num-hollows (make-solid)) 0); just a solid example

;Problem 11

;; a date is a 
;; (make-date m d y) where 
;; - m is a number in [1, 12], 
;; - d is a number in [1, 31], and 
;; - y is a number in [1900, 2099] 
(define-struct date (month day year))

;Contract- day-of-week: date -> string
;Purpose- Returns the day of the week thedate fell on, according to this formula:
;w = (remainder n 7)
;n = (y-1900) + j + d + floor(y/4)
(define (day-of-week thedate)
  (local
    { ;Contract- j-calc: date -> num 
     ;Purpose- Returns month adjustor j from thedate
     (define (j-calc thedate)
       (cond
         [(or (= (date-month thedate) 3) (= (date-month thedate) 7) (= (date-month thedate) 11)) 4]
         [(or (= (date-month thedate) 9) (= (date-month thedate) 12)) 6]
         [(= (date-month thedate) 5) 2]
         [(= (date-month thedate) 6) 5]
         [(= (date-month thedate) 8) 3]
         [(= (date-month thedate) 10) 1]
         [(and (= (date-month thedate) 1) (not (leap? (date-year thedate)))) 1]
         [(and (= (date-month thedate) 2) (leap? (date-year thedate))) 3]
         [(and (= (date-month thedate) 2) (not (leap? (date-year thedate)))) 4]
         [else 0]))
     ;Contract- w-calc: date -> num 
     ;Purpose- Returns w, the numerical day of the year, from thedate
     (define (w-calc thedate)
       (+ (date-year thedate) -1900 (date-day thedate) (floor (/ (date-year thedate) 4))
          (j-calc thedate)))
     ;Contract- n-calc: num -> num
     ;Purpose - Returns the numerical day of the week from the numerical day of the year, w
     (define (n-calc w)
       (remainder w 7))
     ;Contract- string-calc: num -> string
     ;Purpose- Returns the word corresponding to the numerical day of the week, n
     (define (string-calc n)
       (cond
         [(= n 0) "Sunday"]
         [(= n 1) "Monday"]
         [(= n 2) "Tuesday"]
         [(= n 3) "Wednesday"]
         [(= n 4) "Thursday"]
         [(= n 5) "Friday"]
         [(= n 6) "Saturday"]))}
    (string-calc (n-calc (w-calc thedate)))))

(check-expect (day-of-week (make-date 3 25 1994)) "Friday");regular year example1
(check-expect (day-of-week (make-date 9 12 1910)) "Monday");regular year example2
(check-expect (day-of-week (make-date 2 13 1900)) "Tuesday");leap year example1
(check-expect (day-of-week (make-date 4 30 2000)) "Sunday");leap year example2
;#grader: The month adjustment for July is 0, not 4

;Problem 12

;Contract- average: (listof num) -> num
;Purpose- "computes the arithmetic mean of a list of numbers."
(define (average numlist)
  (local
    {;Contract- list-sum: (listof num) -> num
     ;Purpose- Computes the sum of the elements of l
     (define (list-sum l)
       (cond
         [(empty? l) 0]
         [else (+ (first l) 
                  (list-sum (rest l)))]))}
  (cond
    [(empty? numlist) "Error: empty list"]
    [else (/ (list-sum numlist) (length numlist))])))
;#grader: You raise an error with the function (error)
(check-expect (average '()) "Error: empty list"); empty list example
(check-expect (average (list 45 28 0 68 32 18 32 95 63 54 45.3 48)) 44.025); (listof num) example

;Problem 13

;Contract- num-zeros: (listof num) -> num
;Purpose- Returns the number of 0s in numlist
(define (num-zeros numlist)
  (cond
    [(empty? numlist) 0]
    [(= 0 (first numlist)) (+ 1 (num-zeros (rest numlist)))]
    [else (num-zeros (rest numlist))]))

(check-expect (num-zeros '()) 0);empty list example
(check-expect (num-zeros (list 45 28 0 68 32 18 32 95 63 54 45.3 48)) 1); >0 0's example
(check-expect (num-zeros (list 45 28 68 32 18 32 95 63 54 45.3 48)) 0); no 0's example

;Problem 14

;Contract- negations: (listof bool) -> (listof bool)
;Purpose - Returns a list with falses where inputlist had trues and trues for falses
(define (negations inputlist)
  (cond
    [(empty? inputlist) inputlist]
    [else (append (list (not (first inputlist))) (negations (rest inputlist)))]))

(check-expect (negations (list false false false true false true true false))
              (list true true true false true false false true)); listof bool example
(check-expect (negations empty) empty); empty example
;#You should just use cons, so (cons (not (first inputlist))

;Problem 15

;Contract- circles: (listof color) -> (listof img)
;Purpose- Returns a list of solid circles of radius 12 with each circle's color
;corresponding to each color in collist
(define (circles collist)
  (map (lambda (color)
         (circle 12 "solid" color))
       collist))

"circles example - list of green, red, blue, white, black circles"
(circles (list 
           (make-color 0 255 0) 
           (make-color 255 0 0) 
           (make-color 0 255 255) 
           (make-color 255 255 255) 
           (make-color 0 0 0)))
;#grader: use check-expect. Explanation in problem 2.

;Problem 16

;Contract- includes?: string (listof string) -> bool
;Purpose- Returns true if searchfor is contained in searchthru, false if it is not 
 (define (includes? searchfor searchthru)
   (cond
     [(empty? searchthru) false]
   [else (or (equal? searchfor (first searchthru)) 
             (includes? searchfor (rest searchthru)))]))
 
 (check-expect (includes? "a" (cons "a" (cons "b" empty))) true); given true example
 (check-expect (includes? "c" (cons "a" (cons "b" empty))) false); given false example
 (check-expect (includes? "abba" (cons "a" (cons "abba" empty))) true); longer true example
 (check-expect (includes? "abba" (cons "a" (cons "abbas" empty))) false); longer false example

;Problem 17

;Contract- longer-than: num (listof string) -> (listof string)
;Purpose- Returns a list of only the strings in inputlist that were longer than xchars characters
 (define (longer-than xchars inputlist)
   (cond
     [(empty? inputlist) empty]
     [(< xchars (string-length (first inputlist))) 
      (cons (first inputlist)
            (longer-than xchars (rest inputlist)))]
     [else (longer-than xchars (rest inputlist))]))
 
 (check-expect (longer-than 1 (cons "a" (cons "bb" empty))) 
               (cons "bb" empty)); given filtering example
 (check-expect (longer-than 0 (cons "a" (cons "bb" empty))) 
               (cons "a" (cons "bb" empty))); given no filtering out example
 (check-expect (longer-than 2 (cons "a" (cons "bb" empty))) 
               empty); filter out everything example
 (check-expect (longer-than 3 (cons "sevens" (cons "abba" (cons "a" (cons "bb" empty))))) 
               (cons "sevens" (cons "abba" empty))); longer filter example
 
 ;; === grading ===

;; === Grader's tests ===

;; ===Problem 1 ===
(check-expect (larger 10 100) 100)
(check-expect (larger 10 5) 10)
(check-expect (larger 11 11) 11)

;; ===Problem 2 ===
(check-expect (wider (square 10 "solid" "blue")
                     (square 10 "solid" "red")) (square 10 "solid" "blue"))
(check-expect (wider (square 10 "solid" "blue")
                     (square 20 "solid" "red")) (square 20 "solid" "red"))
(check-expect (wider (square 20 "solid" "blue")
                     (square 10 "solid" "red")) (square 20 "solid" "blue"))

;; ===Problem 3 ===
(check-expect
 (more-pixels? (square 10 "solid" "blue")
               (square 20 "solid" "red")) false)
(check-expect
 (more-pixels? (rectangle 10 5 "solid" "blue")
               (rectangle 4 12 "solid" "red")) true)
(check-expect
 (more-pixels? (square 10 "solid" "red")
               (square 10 "solid" "blue")) false)

;;===Problem 4===
(check-expect (leap? 800) true)
(check-expect (leap? 1000) false)
(check-expect (leap? 1004) true)
(check-expect (leap? 1341) false)

;;===Problem 5===
;(check-expect (next-odd 3.1415) 5)
(check-expect (next-odd 43) 43)
(check-expect (next-odd -4) -3)
;(check-expect (next-odd -0.4) 1)

;;===Problem 6===
(check-expect (color=? (make-color 1 2 3) (make-color 1 2 3)) true)
(check-expect (color=? (make-color 5 2 3) (make-color 1 2 3)) false)
(check-expect (color=? (make-color 1 5 3) (make-color 1 2 3)) false)
(check-expect (color=? (make-color 1 2 5) (make-color 1 2 3)) false)

;;===Problem 7===
(check-expect (luminance (make-color 255 255 255)) 255)
(check-expect (luminance (make-color 1 0 0)) 0.2126)
(check-expect (luminance (make-color 0 10 0)) 7.152)
(check-expect (luminance (make-color 0 0 100)) 7.22)
(check-expect (luminance (make-color 1 10 100)) 14.5846)

;;===Problem 8===
(check-expect (more-luminous? (make-color 0 10 0) (make-color 0 0 100)) false)
(check-expect (more-luminous? (make-color 5 5 5) (make-color 1 1 1)) true)

;;===Problem 9===
(check-expect (num-pieces (make-solid)) 1)
(check-expect (num-pieces (make-hollow (make-solid))) 3)

;;===Problem 10===
(check-expect (num-hollows (make-solid)) 0)
(check-expect (num-hollows (make-hollow (make-solid))) 1)
(check-expect (num-hollows (make-hollow 
                            (make-hollow
                             (make-hollow (make-solid))))) 3)

;;===Problem 11===
(check-expect (day-of-week (make-date 1 8 1905)) "Sunday")
(check-expect (day-of-week (make-date 2 5 1912)) "Monday")
(check-expect (day-of-week (make-date 3 3 1988)) "Thursday")
(check-expect (day-of-week (make-date 4 6 1932)) "Wednesday")
(check-expect (day-of-week (make-date 5 25 1933)) "Thursday")
(check-expect (day-of-week (make-date 6 13 1995)) "Tuesday")
(check-expect (day-of-week (make-date 7 5 2012)) "Thursday")
(check-expect (day-of-week (make-date 8 26 2051)) "Saturday")
(check-expect (day-of-week (make-date 9 1 2071)) "Tuesday")
(check-expect (day-of-week (make-date 10 31 2072)) "Monday")
(check-expect (day-of-week (make-date 11 15 2080)) "Friday")
(check-expect (day-of-week (make-date 12 12 2092)) "Friday")

;;===Problem 12===
;(check-error (average empty))
(check-expect (average (list 1 2)) 1.5)
(check-expect (average (list 1 2 3 4 5)) 3)

;;===Problem 13===
(check-expect (num-zeros empty) 0)
(check-expect (num-zeros (list 0 0 5)) 2)
(check-expect (num-zeros (list 1 2 3 4)) 0)

;;===Problem 14===
(check-expect (negations empty) empty)
(check-expect (negations (list true false)) (list false true))
(check-expect (negations (list false false true false))
              (list true true false true))

;;===Problem 15===
(check-expect (circles empty) empty)
(check-expect (circles (list (make-color 255 0 0)
                             (make-color 0 255 0)))
              (list (circle 12 "solid" (make-color 255 0 0))
                    (circle 12 "solid" (make-color 0 255 0))))

;;===Problem 16===
(check-expect (includes? "Hello" empty) false)
(check-expect (includes? "Hello" (list "Hello" "World")) true)
(check-expect (includes? "Hi" (list "Hello" "World")) false)

;;===Problem 17===
(check-expect (longer-than 0 empty) empty)
(check-expect (longer-than 3 (list "The"
                                   "quick"
                                   "brown"
                                   "fox"))
              (list "quick" "brown"))


;; problem 1          2/2
;; problem 2          2/2
;; problem 3          2/2
;; problem 4          2/2
;; problem 5          2/3
;; problem 6          3/3
;; problem 7          3/3
;; problem 8          3/3
;; problem 9          5/5
;; problem 10         5/5
;; problem 11         7/8
;; problem 12         6/7
;; problem 13         7/7
;; problem 14         6/7
;; problem 15         7/7
;; problem 16         7/7
;; problem 17         7/7

;; style              8/10 ;lines were over 80 chars
                            ;and you used eyeball tests instead of check-expect
;; svn used correctly 10/10

;; _total-score_     94/100

;; grader: Adam Wyeth