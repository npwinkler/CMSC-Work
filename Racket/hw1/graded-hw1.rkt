;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-beginner-reader.ss" "lang")((modname graded-hw1) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
; Homework 1 - Nolan Winkler
(require 2htdp/image)

; Problem 1
(define v
  (+ 1 (- (* 2 3)) (/ 4 5) (* 6 7) )) ;pre-fix notation
v
(check-expect v 37.8)

;Problem 2
;sec/cent=sec/min*min/hr*hr/day*day/yr*yr/cent=3600*24*365*100
(define sec-per-century
  (* 60 60 24 365 100))
sec-per-century
(check-expect sec-per-century 3153600000)

;Problem 3
(define area
  (* pi (sqr 15))); pi*r^2, r=15
area
(check-within area (* 15 15 pi) .001)

;Problem 4
#| "The ratio of the size of the cross to the height is 5:8, 
and to the length is 5:12" - Wikipedia. I reasoned the central white square
was 1/3rd of both the height and width of the cross. |#
(define flag-Switzerland
  (overlay
   (rectangle (* (/ 5 12) 120) (* (/ 5 8 3 ) 120) "solid" "white"); wide rect
   (rectangle (* (/ 5 12 3) 120) (* (/ 5 8) 120) "solid" "white"); tall rect
   (square 120 "solid" "red")
    ))
;; GRADER'S NOTE: There is a tiny tabbing issue with the two ))
;; on the last line of your definition. Note (by convention) it
;; would be expected to close those parentheses on the line above:
;; (square 120 "solid" "red")))
;; (points were deducted for the tabbing error only).
flag-Switzerland

;; === grading ===

(check-expect v (+ (- 1 (* 2 3)) (/ 4 5) (* 6 7)))
(check-expect sec-per-century (* 100 365 24 60 60))
(check-within area (* pi 15 15) 0.000001)
flag-Switzerland

;; GRADER'S NOTE: Very good job. The commenting was done almost to
;; an extreme here (better than the reverse situation!). Still,
;; think about how you could be more economical with commenting
;; (helps with clarity for a reader of your code rather than
;; bogging the reader down with unnecessary information).

;; problem 1          20/20
;; problem 2          20/20
;; problem 3          20/20
;; problem 4          20/20

;; style              9/10
;; svn used correctly 10/10

;; _total-score_     99/100

;; grader: MARC KEALHOFER
