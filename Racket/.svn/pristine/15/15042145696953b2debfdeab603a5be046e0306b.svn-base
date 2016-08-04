;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-beginner-reader.ss" "lang")((modname hw1) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
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
flag-Switzerland