;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-advanced-reader.ss" "lang")((modname hw5) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #t #t none #f ())))
;Homework 5 - Nolan Winkler

(require racket/list)
(require 2htdp/image)
(define-struct pair (a b))

;; a Huff-tree is either
;; - empty
;; - a (make-Huff path weight char tl tr), 
;; - where path is a string, weight is a num, char is a char 
;; and tl, tr are Huff-trees
(define-struct Huff (path weight char tl tr))


;;one huge compress function just to make it easy to call
;;and other stuff from lab4 that is necessary to hw5
(define (compress msg)
  (local
    {(define (str->aht msg)
       (local
         {(define (slopmaker msg)
            (local
              {(define (lopmaker msg)
                 (local
                   {(define ohlist (string->list msg))
                    (define (numlistmaker clist)
                      (cond
                        [(empty? clist) empty]
                        [else
                         (local
                           {(define charimon (first clist))
                            (define firstnum 
                              (length (filter (λ (e) (char=? charimon e)) 
                                              ohlist)))}
                           (cons firstnum (numlistmaker (rest clist))))]))}
                   (remove-duplicates
                    (build-list (length ohlist) 
                                (λ (n) (make-pair 
                                        (list-ref (numlistmaker ohlist) n)
                                        (list-ref ohlist n)))))))}
              (sort (lopmaker msg) (λ (p1 p2) (< (pair-a p1) (pair-a p2))))))
          (define (treemaker list)
            (map (λ (p) (make-Huff "" (pair-a p) (pair-b p) empty empty)) list))
          (define (tree-alg tlist)
            (local
              {(define (extract tlist)
                 (local
                   {(define sortedlist 
                      (sort tlist (λ (t1 t2) 
                                    (< (Huff-weight t1) (Huff-weight t2)))))}
                   (take sortedlist 2)))
               (define (tmerge 2tlist)
                 (local
                   {(define t1 (list-ref 2tlist 0))
                    (define t2 (list-ref 2tlist 1))}
                   (make-Huff ""
                              (+ (Huff-weight t1)
                                 (Huff-weight t2))
                              #\NUL
                              t1
                              t2)))
               (define (reinsort t tlist)
                 (sort (cons t tlist) (λ (t1 t2) 
                                        (< (Huff-weight t1) (Huff-weight t2)))))
               (define (pset 1t)
                 (cond
                   [(empty? (Huff-tl 1t)) 1t]
                   [else 
                    (local
                      {(define 1lt (Huff-tl 1t))
                       (define 1rt (Huff-tr 1t))
                       (define temp (Huff-path 1t))}
                      (begin
                        (set-Huff-path! 1lt (string-append temp "0"))
                        (set-Huff-path! 1rt (string-append temp "1"))
                        (make-Huff "" (Huff-weight 1t) (Huff-char 1t)
                                   (pset 1lt)
                                   (pset 1rt))))]))}
              (cond
                [(= 1 (length tlist)) (pset (first tlist))]
                [else (tree-alg (reinsort 
                                 (tmerge (extract tlist)) (drop tlist 2)))])))}
         (tree-alg (treemaker (slopmaker msg)))))
     (define aht (str->aht msg))
     (define (encode msg)
       (local
         {(define (char->code char tree)
            (cond
              [(char=? char (Huff-char tree)) (Huff-path tree)]
              [(empty? (Huff-tl tree)) ""]
              [else (string-append
                     (char->code char (Huff-tl tree))
                     (char->code char (Huff-tr tree)))]))}
         (foldr string-append "" (map (λ (char) (char->code char aht))
                                      (string->list msg)))))}
    (make-pair (encode msg) aht)))

(define (show-tree t)
  (cond
    [(empty? t) (square 30 "solid" "white")]
    [else
     (local
       {(define node (overlay (above 
                               (text (Huff-path t) 10 "black")
                               (overlay/align/offset 
                                "right" "center"
                                (text (make-string 1 (Huff-char t)) 
                                      10 "black")
                                10 0
                                (text (number->string (Huff-weight t))
                                      10 "black")))
                              (circle 20 "outline" "black")))
        (define l-img (show-tree (Huff-tl t)))
        (define r-img (show-tree (Huff-tr t)))
        (define l-w (image-width l-img))
        (define r-w (image-width r-img))
        (define w (/ (+ (image-width l-img) (image-width r-img)) 2))}
       (above node
              (line 0 10 "black")
              (line w 0 "black")
              (cond
                [(> l-w r-w)
                 (beside/align
                  "top"
                  (above (line 0 10 "black") l-img)
                  (above (line 0 10 "black") r-img)
                  (line (/ (- l-w r-w) 2) 0 "white"))]
                [else
                 (beside/align
                  "top"
                  (line (/ (- r-w l-w) 2) 0 "white")
                  (above (line 0 10 "black") l-img)
                  (above (line 0 10 "black") r-img))])))]))

(define (helpergraderf msg)
  (local
    {(define myfout (compress msg))
     (define temp (pair-b myfout))}
    (begin (set-pair-b! myfout (show-tree temp))
           myfout)))

;;actual beginning of HW5 code

;;decompress: string huff -> string
;;Given a bitstring and an augmented Huff-tree, returns the
;;string of ASCII characters corresponding to the message.
(define (decompress code tree)
  (local
    {(define origlist (string->list code))
     (define charimon empty)
     (define msgsofar "")
     ;searcher: string tree -> string
     ;;returns the character corresponding to a bitstring in a tree
     (define (searcher string tree)
       (cond
         [(and (not (string=? string (Huff-path tree)))
               (empty? (Huff-tl tree))) ""]
         [(string=? "" string) string]
         [else (if (string=? string (Huff-path tree))
                   (list->string (list (Huff-char tree)))
                   (string-append (searcher string (Huff-tl tree))
                                  (searcher string (Huff-tr tree))))]))
     ;;dcfun: (void) -> string
     ;;Performs searches until it finds everything and prints the message
     (define (dcfun)
       (cond
         [(and (empty? origlist) (empty? charimon)) msgsofar]
         [else (begin
                 (if (string=? "" (searcher (list->string charimon) tree))
                     (begin
                       (set! charimon (append  charimon 
                                               (list (first origlist))))
                       (set! origlist (rest origlist)))
                     (begin
                       (set! msgsofar (string-append 
                                       msgsofar 
                                       (searcher (list->string charimon) tree)))
                       (set! charimon empty)))                 
                 (dcfun))]))}
    (dcfun)))

(check-expect (decompress (pair-a (compress "ab"))
                          (pair-b (compress "ab"))) "ab")
(check-expect (decompress (pair-a (compress "Sally Sells Seashells")) 
                          (pair-b (compress "Sally Sells Seashells")))
              "Sally Sells Seashells")

;;decompress helper test          
(define (searcherf string tree)
  (cond
    [(and (not (string=? string (Huff-path tree)))
          (empty? (Huff-tl tree))) ""]
    [(string=? "" string) string]
    [else (if (string=? string (Huff-path tree))
              (list->string (list (Huff-char tree)))
              (string-append (searcherf string (Huff-tl tree))
                             (searcherf string (Huff-tr tree))))]))

(check-expect (searcherf "011" (pair-b (compress "Sally Sells Seashells")))
              "S")
(check-expect (searcherf "000" (pair-b (compress "Sally Sells Seashells")))
              "")

;;measure: string -> num
;;Returns the ratio of the size of the compressed message to
;;the size of the uncomppressed message
(define (measure msg)
  (local
    ;;stringsize: string -> num
    ;;Returns 8 times the number of characters in a string, aka its size
    {(define (stringsize msg)
       (* 8 (length (string->list msg))))
     ;;treesize: tree -> num
     ;;Returns the size of a tree according to HW5 formula
     (define (treesize tree)
       (cond
         [(empty? (Huff-tl tree))
          (+ (stringsize (Huff-path tree))
             8
             16)]
         [else
          (+ 16
             (treesize (Huff-tl tree))
             (treesize (Huff-tr tree)))]))}
    (/ (+ 
        (treesize (pair-b (compress msg))) (stringsize (pair-a (compress msg)))) 
       (stringsize msg))))

(check-expect (measure "ab") 6)
(check-within (measure "SEASHELLS") 6.1 .1)
(check-expect (measure "aaaa") .75)

;measure helper tests:
(define (stringsize msg)
  (* 8 (length (string->list msg))))

(check-expect (stringsize "Hello") 40)
(check-expect (stringsize "") 0)

(define (treesize tree)
  (cond
    [(empty? (Huff-tl tree))
     (+ (stringsize (Huff-path tree))
        8
        16)]
    [else
     (+ 16
        (treesize (Huff-tl tree))
        (treesize (Huff-tr tree)))]))

(check-expect (treesize (pair-b (compress "Hello")))
              216)
(check-expect (treesize (pair-b (compress "ab")))
              80)