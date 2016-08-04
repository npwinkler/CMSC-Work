;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-intermediate-reader.ss" "lang")((modname graded-lab1) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ())))
; Lab 1 - Nolan Winkler
(require 2htdp/image)

;Problem 1 - should be volume of a sphere/shell, not area, given the equation
;volume-of-shell: num num -> num (contract)
#| compute the volume of a shell whose radius is outer
and whose spherical hole has a radius of inner |#
(define (volume-of-shell outer inner) ; definition
  (local
    {(define (volume-of-sphere r)
       (* (/ 4 3) pi (expt r 3)))}
    (- (volume-of-sphere outer)
       (volume-of-sphere inner))))

(check-within (volume-of-shell 10 7) 2752.035164544659 .001); test1
(check-within (volume-of-shell 5 1) 519.4099853935124 .001); test2

;Problem 2
;flag-Greece: num -> img (contract)
#| Create an image of the Greek flag with width size and corresponding height.
Dimensions were based off of
http://commons.wikimedia.org/wiki/File:Flag_of_Greece_(construction).svg |#
(define (flag-Greece size)
  (local
    {(define gfwidth size)
     (define gfheight (* (/ 2 3) gfwidth))
     (define bluestripe (rectangle gfwidth (/ gfheight 9) "solid" "RoyalBlue"))
     (define whitestripe (rectangle gfwidth (/ gfheight 9) "solid" "white"))
     (define field (above bluestripe
                          (above whitestripe
                                 (above bluestripe
                                        (above whitestripe
                                               (above bluestripe
                                                      (above whitestripe
                                                             (above bluestripe
                                                                    (above whitestripe
                                                                           bluestripe)))))))))
     ;#grader: lines should not be over 80 chars
     (define sside (* (/ 10 27) gfwidth))
     (define bluesquare (square sside "solid" "RoyalBlue"))
     (define vrwidth (* (/ 1 5) sside))
     (define vrheight sside)
     (define hrwidth sside)
     (define hrheight (* (/ 1 5) sside))
     (define whitecross (overlay (rectangle vrwidth vrheight "solid" "white")
                                 (rectangle hrwidth hrheight "solid" "white")))
     (define corner (overlay whitecross
                            bluesquare))}
    (overlay/align "left" "top" corner
                   field)))
(flag-Greece 250); example 1
(flag-Greece 512); example 2

;flag-Turkey: num -> img (contract)
#| Create an image of the Greek flag with width size and corresponding height.
Dimensions were roughly based off of
http://en.wikipedia.org/wiki/Flag_of_Turkey#Construction
but adjustments were made including cropping out the white hem
and estimating side length of the star|#
(define (flag-Turkey size)
  (local
    {(define tfwidth size)
     (define tfheight (* (/ 2 3) tfwidth))
     (define wcradius (* (/ 1 4) tfheight))
     (define rcradius (* (/ 1 5) tfheight))
     (define wsside (* .125 tfheight))
     (define redcirc (circle rcradius "solid" (make-color 227 10 23)))
     (define whitecirc (circle wcradius "solid" "white"))
     (define redrect (rectangle tfwidth tfheight "solid" (make-color 227 10 23)))
     (define crescent (underlay/offset whitecirc
                                       (* (/ 1 25) tfwidth) 0
                                       redcirc))
     (define whitestar (rotate 20 
                               (star wsside "solid" "white")))}
    (underlay/align/offset "left" "middle" redrect
                           (* (/ 1 7) tfwidth) 0
                           (underlay/offset crescent
                                            (* (/ 1 5) tfwidth) 0
                                            whitestar))))
(flag-Turkey 250); example1
(flag-Turkey 372); example2

;; === grading ===

;; Grader's tests

;; ===Problem 1===
(check-within (volume-of-shell 10 0)
              (* (/ 4 3) pi 1000)
              0.001)
(check-within (volume-of-shell 8 3)
              (* (/ 4 3) pi 485)
              0.001)


;; ===Problem 2===
(check-expect (image-width (flag-Greece 100)) 100)
(check-expect (image-width (flag-Turkey 100)) 100)

"(flag-Greece 200)"
(flag-Greece 200)
"(flag-Greece 100)"
(flag-Greece 100)
"(flag-Turkey 200)"
(flag-Turkey 200)
"(flag-Turkey 100)"
(flag-Turkey 100)

;; area-of-shell          20/20
;; flag-Greece            20/20
;; flag-Turkey            20/20

;; contracts & purposes   10/10
;; tests                  10/10

;; style                 9/10 ;;Make sure your lines aren't too long
;; svn used correctly    10/10

;; _total-score_        99/100

;;Great Job!
;; grader: Adam Wyeth

