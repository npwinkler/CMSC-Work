;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-intermediate-lambda-reader.ss" "lang")((modname lab2) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
;Lab2 - Nolan Winkler

;This lab was not able to be succesfully completed. So, I demonstrate
;various functions that have correct properties of a 
;correct-image-producing function that when combined could represent a solution.

(require 2htdp/image)

(define (design s)
  (local
    {(define r (rectangle (* s 3/5) (min 2 (/ s 10)) "solid" "darkblue"))
     (define block (above r (rectangle (* s 3/5) (/ s 5) "solid" "blue") r))}
    (overlay
     (square s "outline" "gray")
     (overlay/xy block
                 (/ s -3.5)
                 (/ s -6) 
                 (square s "solid" "dodgerblue")))))

;Contract- recursive-tiles: num -> img
;Produces the correct recursive tiling design for edge length s in a reasonable time
;i.e. # and relative size of squares in each layer is correct, 
;demonstrating proper use of recursion
;errors: spacing of the outermost layer is always off for n>8,
;image-width and image-height are 2n-8 rather than n.
(define (recursive-tiles size)
  (local
    {(define (trdesign s)
       (design s))
     (define (tldesign s)
       (flip-horizontal (trdesign s)))
     (define (bldesign s)
       (flip-vertical (tldesign s)))
     (define (brdesign s)
       (flip-horizontal (bldesign s)))
     (define tl2
       (tldesign 2))
     (define tr2
       (trdesign 2))
     (define bl2
       (bldesign 2))
     (define br2
       (brdesign 2))
;tlvert: num-> img 
;produces recursive vertical column of top-left designs, biggest design with side n
;trvert, blvert, brvert do the same for their respective designs. Each succesive design
;vertically halves the side size n by 2 until n=4 to get the double border.
     (define (tlvert n)
       (cond
         [(= n 4) (above tl2 tl2)]
         [else (above (beside (tlvert (/ n 2)) (tlvert (/ n 2)))
                      (tldesign n))]))
     (define (trvert n)
       (cond
         [(= n 4) (above tr2 tr2)]
         [else (above (beside (trvert (/ n 2)) (trvert (/ n 2)))
                      (trdesign n))]))
     (define (blvert n)
       (cond
         [(= n 4) (above bl2 bl2)]
         [else (above (bldesign n)
                      (beside (blvert (/ n 2)) (blvert (/ n 2))))]))
     (define (brvert n)
       (cond
         [(= n 4) (above br2 br2)]
         [else (above (brdesign n)
                      (beside (brvert (/ n 2)) (brvert (/ n 2))))]))
;tlhor: num-> img 
;produces recursive horizontal column of top-left designs, biggest design with side n
;trhor, blhor, brhor do the same for their respective designs. Each succesive design
;horizontally halves the side size n by 2 until n=4 to get the double border.
     (define (tlhor n)
       (cond
         [(= n 4) (beside tl2 tl2)]
         [else (beside (above (tlhor (/ n 2)) (tlhor (/ n 2)))
                       (tldesign n))]))
     (define (trhor n)
       (cond
         [(= n 4) (beside tr2 tr2)]
         [else (beside (trdesign n)
                       (above (trhor (/ n 2)) (trhor (/ n 2))))]))
     (define (blhor n)
       (cond
         [(= n 4) (beside bl2 bl2)]
         [else (beside (above (blhor (/ n 2)) (blhor (/ n 2)))
                       (bldesign n))]))
     (define (brhor n)
       (cond
         [(= n 4) (beside br2 br2)]
         [else (beside (brdesign n)
                       (above (brhor (/ n 2)) (brhor (/ n 2))))]))
;trq: num -> img
;creates quadrant of side length n using the horizontal and vertical columns of that design
     (define (trq n)
       (cond
         [(= n 2) tr2]
         [else (overlay/align "right" "top" (trq (/ n 2))
                              (overlay/align "left" "bottom" (trhor n) (trvert n)))]))
     (define (tlq n)
       (cond
         [(= n 2) tl2]
         [else (overlay/align "left" "top" (tlq (/ n 2))
                              (overlay/align "right" "bottom" (tlhor n) (tlvert n)))]))
     (define (brq n)
       (cond
         [(= n 2) br2]  
         [else (overlay/align "right" "bottom" (brq (/ n 2))
                              (overlay/align "left" "top" (brhor n) (brvert n)))]))
     (define (blq n)
       (cond
         [(= n 2) bl2]
         [else (overlay/align "left" "bottom" (blq (/ n 2))
                              (overlay/align "right" "top" (blhor n) (blvert n)))]))}
    (above (beside (tlq (/ size 2)) (trq (/ size 2)))
           (beside (blq (/ size 2)) (brq (/ size 2))))))

;Contract- recursive-tiles2: num -> img
;Produces a complete square with no weird spacing using recursion that has correct
;individual tiles in each quadrant, demonstrating proper use of recursion
;errors: pattern is off in not having the double outer border (look at size=8 wrong case)
;image-width and image-height are 2n-4 rather than n.
(define (recursive-tiles2 size)
  (local
    {(define (trdesign s)
  (design s))
(define (tldesign s)
  (flip-horizontal (trdesign s)))
(define (bldesign s)
  (flip-vertical (tldesign s)))
(define (brdesign s)
  (flip-horizontal (bldesign s)))
(define tl2
  (tldesign 2))
(define tr2
  (trdesign 2))
(define bl2
  (bldesign 2))
(define br2
  (brdesign 2))
     (define (tlvert n)
  (cond
    [(= n 2) (tldesign 2)]
    [else (above (beside (tlvert (/ n 2)) (tlvert (/ n 2)))
                 (tldesign n))]))
(define (trvert n)
  (cond
    [(= n 2) (trdesign 2)]
    [else (above (beside (trvert (/ n 2)) (trvert (/ n 2)))
                 (trdesign n))]))
(define (blvert n)
 (cond
  [(= n 2) (bldesign 2)]
    [else (above (bldesign n)
                 (beside (blvert (/ n 2)) (blvert (/ n 2))))]))
(define (brvert n)
 (cond
  [(= n 2) (brdesign 2)]
    [else (above (brdesign n)
                 (beside (brvert (/ n 2)) (brvert (/ n 2))))]))
(define (tlhor n)
    (cond
    [(= n 2) (tldesign n)]
    [else (beside (above (tlhor (/ n 2)) (tlhor (/ n 2)))
                  (tldesign n))]))
(define (trhor n)
  (cond
    [(= n 2) (trdesign n)]
    [else (beside (trdesign n)
                  (above (trhor (/ n 2)) (trhor (/ n 2))))]))
(define (blhor n)
  (cond
    [(= n 2) (bldesign n)]
    [else (beside (above (blhor (/ n 2)) (blhor (/ n 2)))
                  (bldesign n))]))
(define (brhor n)
  (cond
    [(= n 2) (brdesign n)]
    [else (beside (brdesign n)
          (above (brhor (/ n 2)) (brhor (/ n 2))))]))
     (define (trq n)
  (cond
    [(= n 2) tr2]
    [else (overlay/align "right" "top" (trq (/ n 2))
                         (overlay/align "left" "bottom" (trhor n) (trvert n)))]))
(define (tlq n)
  (cond
    [(= n 2) tl2]
    [else (overlay/align "left" "top" (tlq (/ n 2))
                        (overlay/align "right" "bottom" (tlhor n) (tlvert n)))]))
(define (brq n)
(cond
    [(= n 2) br2]  
    [else (overlay/align "right" "bottom" (brq (/ n 2))
                         (overlay/align "left" "top" (brhor n) (brvert n)))]))
(define (blq n)
  (cond
    [(= n 2) bl2]
    [else (overlay/align "left" "bottom" (blq (/ n 2))
                         (overlay/align "right" "top" (blhor n) (blvert n)))]))}
  (above (beside (tlq (/ size 2)) (trq (/ size 2)))
         (beside (blq (/ size 2)) (brq (/ size 2))))))

;recursive-tiles-3: num -> img
;Produces an image of the correct size using the design pieces with quadrant specificity
;errors: no attempt at recursion, pattern is obviously off
(define (recursive-tiles3 size)
  (local
    {(define (trdesign s)
       (design s))
     (define (tldesign s)
       (flip-horizontal (trdesign s)))
     (define (bldesign s)
       (flip-vertical (tldesign s)))
     (define (brdesign s)
       (flip-horizontal (bldesign s)))}
  (above (beside (tldesign (/ size 2)) (trdesign (/ size 2)))
         (beside (bldesign (/ size 2)) (brdesign (/ size 2))))))