;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-advanced-reader.ss" "lang")((modname lab4) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #t #t none #f ())))
;Lab 4 - Nolan Winkler

;;If I am interpreting your last grading comment to mean that I don't need to
;;create one function you can copy and paste outside without composes, calls
;;to outside functions, etc. here is my lab. Else start at START HERE if you
;;need to be able to have one function that makes no outside calls that does it

(require racket/list)
(require 2htdp/image)
(define-struct pair (a b))

;; a Huff-tree is either
;; - empty
;; - a (make-Huff path weight char tl tr), 
;; - where path is a string, weight is a num, char is a char 
;; and tl, tr are Huff-trees
(define-struct Huff (path weight char tl tr))

;;lopmaker: string -> (listof (pairof num char))
;;Creates a list of a pair for each unique character in the
;;string that has itself and its frequency of occurrence
(define (lopmaker msg)
  (local
    {(define ohlist (string->list msg))
     ;;numlistmaker: (listof char) -> (listof num)
     ;;Returns a list of the frequency each char in a list
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
                         (list-ref ohlist n)))))))

(check-expect (lopmaker "Sally Sells Seashells")
              (list
               (make-pair 3 #\S)
               (make-pair 2 #\a)
               (make-pair 6 #\l)
               (make-pair 1 #\y)               
               (make-pair 2 #\space)
               (make-pair 3 #\e)              
               (make-pair 3 #\s)
               (make-pair 1 #\h)))
(check-expect (lopmaker "aaaaaaaaaaAAa")
              (list
               (make-pair 11 #\a)
               (make-pair 2 #\A)))

;;lop helper test
(define testlist (string->list "Hello"))
(define (numlistmaker clist)
  (cond
    [(empty? clist) empty]
    [else
     (local
       {(define charimon (first clist))
        (define firstnum 
          (length (filter (λ (e) (char=? charimon e)) 
                          testlist)))}
       (cons firstnum (numlistmaker (rest clist))))]))
(check-expect (numlistmaker testlist) (list 1 1 2 2 1))

;;treemaker: (listof (pairof num char)) -> (listof Huff-tree)
;Returns the list of singleton Huff-trees corresponding to
;a list of weight/character pairs, sorted by ascending weight
(define (treemaker plist)
  (sort
   (map (λ (p) (make-Huff "" (pair-a p) (pair-b p) empty empty)) plist)
   (λ (t1 t2) (< (Huff-weight t1) (Huff-weight t2)))))

(check-expect (treemaker (lopmaker "Sally Sells Seashells"))
              (list
               (make-Huff "" 1 #\y empty empty)
               (make-Huff "" 1 #\h empty empty)
               (make-Huff "" 2 #\a empty empty)
               (make-Huff "" 2 #\space empty empty)
               (make-Huff "" 3 #\S empty empty)
               (make-Huff "" 3 #\e empty empty)
               (make-Huff "" 3 #\s empty empty)
               (make-Huff "" 6 #\l empty empty)))
(check-expect (treemaker (lopmaker "Hello World"))
              (list
               (make-Huff "" 1 #\H empty empty)
               (make-Huff "" 1 #\e empty empty)
               (make-Huff "" 1 #\space empty empty)
               (make-Huff "" 1 #\W empty empty)
               (make-Huff "" 1 #\r empty empty)
               (make-Huff "" 1 #\d empty empty)
               (make-Huff "" 2 #\o empty empty)
               (make-Huff "" 3 #\l empty empty)))

;;pset: Huff-tree -> Huff-tree
;;creates an augmented Huffman tree from a Huffman tree
;;by giving leaves their proper paths
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
                    (pset 1rt))))]))

(check-expect (pset (make-Huff "" 10 #\NUL
                               (make-Huff "" 4 #\NUL  
                                          (make-Huff "" 2 #\a empty empty) 
                                          (make-Huff "" 2 #\space empty empty))
                               (make-Huff "" 6 #\l empty empty)))
              (make-Huff "" 10 #\NUL
                         (make-Huff "" 4 #\NUL
                                    (make-Huff "00" 2 #\a empty empty)
                                    (make-Huff "01" 2 #\space empty empty))
                         (make-Huff "1" 6 #\l empty empty)))

;;tree-alg: (listof Huff-tree) -> Huff-tree
;;Returns 1 augmented Huff-tree from a list of singleton Huff-trees
(define (tree-alg tlist)
  (local
    ;;extract: (listof Huff-tree) -> (listof Huff-tree)
    ;;Gives a list of the 2 Huff-trees with smallest weight in tlist
    {(define (extract tlist)
       (local
         {(define sortedlist 
            (sort tlist (λ (t1 t2) 
                          (< (Huff-weight t1) (Huff-weight t2)))))}
         (take sortedlist 2)))
     ;;tmerge: (listof Huff-tree) -> Huff-tree 
     ;;Takes a list of 2 Huff-trees and combines them into 1 Huff-tree
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
     ;;reinsort: Huff-tree (listof Huff-tree) -> (listof Huff-tree)
     ;;Returns a list of t and everything in tlist, sorted by weight
     (define (reinsort t tlist)
       (sort (cons t tlist) (λ (t1 t2) 
                              (< (Huff-weight t1) (Huff-weight t2)))))}
    (cond
      [(= 1 (length tlist)) (pset (first tlist))]
      [else (tree-alg (reinsort 
                       (tmerge (extract tlist)) (drop tlist 2)))])))

(check-expect (tree-alg (list (make-Huff "" 1 #\y empty empty)
                              (make-Huff "" 1 #\h empty empty)
                              (make-Huff "" 2 #\a empty empty)
                              (make-Huff "" 2 #\space empty empty)
                              (make-Huff "" 3 #\S empty empty)
                              (make-Huff "" 3 #\e empty empty)
                              (make-Huff "" 3 #\s empty empty)
                              (make-Huff "" 6 #\l empty empty)))
              (make-Huff
               ""
               21
               #\nul
               (make-Huff
                ""
                9
                #\nul
                (make-Huff
                 ""
                 4
                 #\nul
                 (make-Huff "" 2 #\nul 
                            (make-Huff "0000" 1 #\y empty empty) 
                            (make-Huff "0001" 1 #\h empty empty))
                 (make-Huff "001" 2 #\a empty empty))
                (make-Huff "" 5 #\nul 
                           (make-Huff "010" 2 #\space empty empty) 
                           (make-Huff "011" 3 #\S empty empty)))
               (make-Huff
                ""
                12
                #\nul
                (make-Huff "" 6 #\nul 
                           (make-Huff "100" 3 #\e empty empty) 
                           (make-Huff "101" 3 #\s empty empty))
                (make-Huff "11" 6 #\l empty empty))))
(check-expect (tree-alg 
               (list
                (make-Huff "" 1 #\H empty empty)
                (make-Huff "" 1 #\e empty empty)
                (make-Huff "" 1 #\space empty empty)
                (make-Huff "" 1 #\W empty empty)
                (make-Huff "" 1 #\r empty empty)
                (make-Huff "" 1 #\d empty empty)
                (make-Huff "" 2 #\o empty empty)
                (make-Huff "" 3 #\l empty empty)))
              (make-Huff
               ""
               11
               #\nul
               (make-Huff
                ""
                4
                #\nul
                (make-Huff
                 ""
                 2
                 #\nul
                 (make-Huff "000" 1 #\r empty empty)
                 (make-Huff "001" 1 #\d empty empty))
                (make-Huff
                 ""
                 2
                 #\nul
                 (make-Huff "010" 1 #\space empty empty)
                 (make-Huff "011" 1 #\W empty empty)))
               (make-Huff
                ""
                7
                #\nul
                (make-Huff "10" 3 #\l empty empty)
                (make-Huff
                 ""
                 4
                 #\nul
                 (make-Huff
                  ""
                  2
                  #\nul
                  (make-Huff "1100" 1 #\H empty empty)
                  (make-Huff "1101" 1 #\e empty empty))
                 (make-Huff "111" 2 #\o empty empty)))))

;;tree-alg helper tests
(define (extract tlist)
  (local
    {(define sortedlist 
       (sort tlist (λ (t1 t2) 
                     (< (Huff-weight t1) (Huff-weight t2)))))}
    (take sortedlist 2)))
(check-expect (extract (list (make-Huff "" 1 #\y empty empty)
                             (make-Huff "" 4 #\NUL  
                                        (make-Huff "" 2 #\a empty empty) 
                                        (make-Huff "" 2 #\space empty empty))
                             (make-Huff "" 6 #\l empty empty)
                             (make-Huff "" 3 #\s empty empty)
                             (make-Huff "" 3 #\S empty empty)
                             (make-Huff "" 3 #\e empty empty)))
              (list (make-Huff "" 1 #\y empty empty)
                    (make-Huff "" 3 #\s empty empty)))

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
(check-expect (tmerge (list (make-Huff "" 4 #\NUL  
                                       (make-Huff "" 2 #\a empty empty) 
                                       (make-Huff "" 2 #\space empty empty))
                            (make-Huff "" 6 #\l empty empty)))
              (make-Huff "" 10 #\NUL
                         (make-Huff "" 4 #\NUL  
                                    (make-Huff "" 2 #\a empty empty) 
                                    (make-Huff "" 2 #\space empty empty))
                         (make-Huff "" 6 #\l empty empty)))

(define (reinsort t tlist)
  (sort (cons t tlist) (λ (t1 t2) 
                         (< (Huff-weight t1) (Huff-weight t2)))))
(check-expect (reinsort 
               (make-Huff "" 10 #\NUL
                          (make-Huff "" 4 #\NUL  
                                     (make-Huff "" 2 #\a empty empty) 
                                     (make-Huff "" 2 #\space empty empty))
                          (make-Huff "" 6 #\l empty empty))
               (list (make-Huff "" 1 #\y empty empty)
                     (make-Huff "" 15 #\p empty empty)
                     (make-Huff "" 2 #\s empty empty)
                     (make-Huff "" 7 #\S 
                                (make-Huff "" 3 #\q empty empty)
                                (make-Huff "" 4 #\u empty empty))
                     (make-Huff "" 3 #\e empty empty)))
              (list (make-Huff "" 1 #\y empty empty)
                    (make-Huff "" 2 #\s empty empty)
                    (make-Huff "" 3 #\e empty empty)
                    (make-Huff "" 7 #\S 
                               (make-Huff "" 3 #\q empty empty)
                               (make-Huff "" 4 #\u empty empty))
                    (make-Huff "" 10 #\NUL
                               (make-Huff "" 4 #\NUL  
                                          (make-Huff "" 2 #\a empty empty) 
                                          (make-Huff "" 2 #\space empty empty))
                               (make-Huff "" 6 #\l empty empty))
                    (make-Huff "" 15 #\p empty empty)))

;;str->aht: string -> Huff-tree
;;Returns the augmented Huff tree corresponding to msg
(define (str->aht msg)
  (tree-alg (treemaker (lopmaker msg))))

(check-expect (str->aht "Hello World")
              (make-Huff
               ""
               11
               #\nul
               (make-Huff
                ""
                4
                #\nul
                (make-Huff "" 2 #\nul 
                           (make-Huff "000" 1 #\r empty empty) 
                           (make-Huff "001" 1 #\d empty empty))
                (make-Huff "" 2 #\nul 
                           (make-Huff "010" 1 #\space empty empty) 
                           (make-Huff "011" 1 #\W empty empty)))
               (make-Huff
                ""
                7
                #\nul
                (make-Huff "10" 3 #\l empty empty)
                (make-Huff
                 ""
                 4
                 #\nul
                 (make-Huff "" 2 #\nul 
                            (make-Huff "1100" 1 #\H empty empty) 
                            (make-Huff "1101" 1 #\e empty empty))
                 (make-Huff "111" 2 #\o empty empty)))))

;;encode: string -> string
;;Takes a msg and an augmented Huff-tree and returns its bitstring
(define (encode msg tree)
  (local
    ;;char->code: char Huff-tree -> string
    ;;Returns the Huffcode bitstring of a character given its aug Huff-tree
    {(define (char->code char tree)
       (cond
         [(char=? char (Huff-char tree)) (Huff-path tree)]
         [(empty? (Huff-tl tree)) ""]
         [else (string-append
                (char->code char (Huff-tl tree))
                (char->code char (Huff-tr tree)))]))}
    (foldr string-append "" (map (λ (char) (char->code char tree))
                                 (string->list msg)))))

(check-expect (encode "Hello World" 
                      (tree-alg (treemaker (lopmaker "Hello World"))))
              "11001101101011101001111100010001")
(check-expect (encode "Sally Sells"
                      (tree-alg (treemaker (lopmaker "Sally Sells"))))
              "11111000011011010111101100100")

;;encode helper test
(define (char->code char tree)
  (cond
    [(char=? char (Huff-char tree)) (Huff-path tree)]
    [(empty? (Huff-tl tree)) ""]
    [else (string-append
           (char->code char (Huff-tl tree))
           (char->code char (Huff-tr tree)))]))
(check-expect (char->code #\a (str->aht "Sally Sells")) "1100")

;;compress: string -> (pairof string Huff-tree)
;;Returns a pair of the bitlist and augmented Huffman tree
;;that corresponds to the msg input
(define (compress msg)
  (local
    {(define aht (str->aht msg))}
    (make-pair (encode msg aht) aht)))

(check-expect 
 (compress "My name is Nolan")
 
 (make-pair
  "01000101000111101010101100100010010011110111111110110011"
  (make-Huff
   ""
   16
   #\nul
   (make-Huff
    ""
    7
    #\nul
    (make-Huff "00" 3 #\space empty empty)
    (make-Huff "" 4 #\nul 
               (make-Huff "" 2 #\nul 
                          (make-Huff "0100" 1 #\M empty empty) 
                          (make-Huff "0101" 1 #\y empty empty)) 
               (make-Huff "011" 2 #\n empty empty)))
   (make-Huff
    ""
    9
    #\nul
    (make-Huff
     ""
     4
     #\nul
     (make-Huff "" 2 #\nul 
                (make-Huff "1000" 1 #\i empty empty) 
                (make-Huff "1001" 1 #\s empty empty))
     (make-Huff "" 2 #\nul 
                (make-Huff "1010" 1 #\m empty empty) 
                (make-Huff "1011" 1 #\e empty empty)))
    (make-Huff
     ""
     5
     #\nul
     (make-Huff "110" 2 #\a empty empty)
     (make-Huff "" 3 #\nul 
                (make-Huff "1110" 1 #\l empty empty) 
                (make-Huff "" 2 #\nul 
                           (make-Huff "11110" 1 #\N empty empty) 
                           (make-Huff "11111" 1 #\o empty empty))))))))
(check-expect (compress "ab")
              (make-pair "01" (make-Huff "" 2 #\nul 
                                         (make-Huff "0" 1 #\a empty empty) 
                                         (make-Huff "1" 1 #\b empty empty))))

;;extra stuff (if you'd prefer to see the tree than having to interpret it)

;; show-tree : Huff-tree -> image
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

;;helpergraderf: string num -> (pairof string img)
;;takes a string and returns the bitlist and the image corresponding
;;to the augmented Huffman tree, with your scaling adjustor, factor
;;to account for how big your input string is
(define (helpergraderf msg factor)
  (local
    {(define myfout (compress msg))
     (define outtree (pair-b myfout))}
    (begin (set-pair-b! myfout (scale factor (show-tree outtree)))
           myfout)))


;;;START HERE IF I MISINTERPRETED YOU ACCORDING TO THE BEGINNING NOTE, ELSE
;;;IGNORE EVERYTHING PAST HERE
;;Lab 4 - Nolan Winkler
;
;(require racket/list)
;(require 2htdp/image)
;(define-struct pair (a b))
;
;;; a Huff-tree is either
;;; - empty
;;; - a (make-Huff path weight char tl tr), 
;;; - where path is a string, weight is a num, char is a char 
;;; and tl, tr are Huff-trees
;(define-struct Huff (path weight char tl tr))
;
;;;Compress Function
;
;;;compress: string -> (pairof string Huff-tree)
;;;Returns a pair of the bitlist and augmented Huffman tree
;;;that corresponds to the msg input
;(define (compress msg)
;  (local
;    ;;str->aht: string -> Huff-tree
;    ;;Returns the augmented Huff tree corresponding to msg
;    {(define (str->aht msg)
;       (local
;         ;;slopmaker: string -> (listof (pairof num char))
;         ;;Creates a list of a pair for each unique character in the string
;         ;;that has itself and its frequency of occurrence, sorted by freq
;         {(define (slopmaker msg)
;            (local
;              ;;lopmaker: string -> (listof (pairof num char))
;              ;;Creates a list of a pair for each unique character in the
;              ;;string that has itself and its frequency of occurrence
;              {(define (lopmaker msg)
;                 (local
;                   {(define ohlist (string->list msg))
;                    ;;numlistmaker: (listof char) -> (listof num)
;                    ;;Returns a list of the frequency each char in a list
;                    (define (numlistmaker clist)
;                      (cond
;                        [(empty? clist) empty]
;                        [else
;                         (local
;                           {(define charimon (first clist))
;                            (define firstnum 
;                              (length (filter (λ (e) (char=? charimon e)) 
;                                              ohlist)))}
;                           (cons firstnum (numlistmaker (rest clist))))]))}
;                   (remove-duplicates
;                    (build-list (length ohlist) 
;                                (λ (n) (make-pair 
;                                        (list-ref (numlistmaker ohlist) n)
;                                        (list-ref ohlist n)))))))}
;              (sort (lopmaker msg) (λ (p1 p2) (< (pair-a p1) (pair-a p2))))))
;          ;;treemaker: (listof (pairof num char)) -> (listof Huff-tree)
;          ;Returns the list of singleton Huff-trees corresponding to
;          ;a list of weight/character pairs
;          (define (treemaker list)
;            (map (λ (p) (make-Huff "" (pair-a p) (pair-b p) empty empty)) list))
;          ;;tree-alg: (listof Huff-tree) -> Huff-tree
;          ;;Returns 1 augmented Huff-tree from a list of singleton Huff-trees
;          (define (tree-alg tlist)
;            (local
;              ;;extract: (listof Huff-tree) -> (listof Huff-tree)
;              ;;Gives a list of the 2 Huff-trees with smallest weight in tlist
;              {(define (extract tlist)
;                 (local
;                   {(define sortedlist 
;                      (sort tlist (λ (t1 t2) 
;                                    (< (Huff-weight t1) (Huff-weight t2)))))}
;                   (take sortedlist 2)))
;               ;;tmerge: (listof Huff-tree) -> Huff-tree 
;               ;;Takes a list of 2 Huff-trees and combines them into 1 Huff-tree
;               (define (tmerge 2tlist)
;                 (local
;                   {(define t1 (list-ref 2tlist 0))
;                    (define t2 (list-ref 2tlist 1))}
;                   (make-Huff ""
;                              (+ (Huff-weight t1)
;                                 (Huff-weight t2))
;                              #\NUL
;                              t1
;                              t2)))
;               ;;reinsort: Huff-tree (listof Huff-tree) -> (listof Huff-tree)
;               ;;Returns a list of t and everything in tlist, sorted by weight
;               (define (reinsort t tlist)
;                 (sort (cons t tlist) (λ (t1 t2) 
;                                        (< (Huff-weight t1) (Huff-weight t2)))))
;               ;;pset: Huff-tree -> Huff-tree
;               ;;creates an augmented Huffman tree from a Huffman tree
;               ;;by giving leaves their proper paths
;               (define (pset 1t)
;                 (cond
;                   [(empty? (Huff-tl 1t)) 1t]
;                   [else 
;                    (local
;                      {(define 1lt (Huff-tl 1t))
;                       (define 1rt (Huff-tr 1t))
;                       (define temp (Huff-path 1t))}
;                      (begin
;                        (set-Huff-path! 1lt (string-append temp "0"))
;                        (set-Huff-path! 1rt (string-append temp "1"))
;                        (make-Huff "" (Huff-weight 1t) (Huff-char 1t)
;                                   (pset 1lt)
;                                   (pset 1rt))))]))}
;              (cond
;                [(= 1 (length tlist)) (pset (first tlist))]
;                [else (tree-alg (reinsort 
;                                 (tmerge (extract tlist)) (drop tlist 2)))])))}
;         (tree-alg (treemaker (slopmaker msg)))))
;     (define aht (str->aht msg))
;     ;;encode: string -> string
;     ;;Takes a msg and returns its bitstring
;     (define (encode msg)
;       (local
;         ;;char->code: char Huff-tree -> string
;         ;;Returns the Huffcode bitstring of a character given its aug Huff-tree
;         {(define (char->code char tree)
;            (cond
;              [(char=? char (Huff-char tree)) (Huff-path tree)]
;              [(empty? (Huff-tl tree)) ""]
;              [else (string-append
;                     (char->code char (Huff-tl tree))
;                     (char->code char (Huff-tr tree)))]))}
;         (foldr string-append "" (map (λ (char) (char->code char aht))
;                                      (string->list msg)))))}
;    (make-pair (encode msg) aht)))
;
;
;(check-expect 
; (compress "not used before aaaaA")
; (make-pair
;  "11100111111101101001000111100000101000111010010111110011110101010101011000"
;  (make-Huff
;   ""
;   21
;   #\nul
;   (make-Huff
;    ""
;    8
;    #\nul
;    (make-Huff
;     ""
;     4
;     #\nul
;     (make-Huff "" 2 #\nul 
;                (make-Huff "0000" 1 #\d empty empty) 
;                (make-Huff "0001" 1 #\b empty empty))
;     (make-Huff "" 2 #\nul 
;                (make-Huff "0010" 1 #\u empty empty) 
;                (make-Huff "0011" 1 #\s empty empty)))
;    (make-Huff "01" 4 #\a empty empty))
;   (make-Huff
;    ""
;    13
;    #\nul
;    (make-Huff
;     ""
;     6
;     #\nul
;     (make-Huff
;      ""
;      3
;      #\nul
;      (make-Huff "1000" 1 #\A empty empty)
;      (make-Huff "" 2 #\nul 
;                 (make-Huff "10010" 1 #\f empty empty) 
;                 (make-Huff "10011" 1 #\r empty empty)))
;     (make-Huff "101" 3 #\space empty empty))
;    (make-Huff
;     ""
;     7
;     #\nul
;     (make-Huff "110" 3 #\e empty empty)
;     (make-Huff
;      ""
;      4
;      #\nul
;      (make-Huff "" 2 #\nul 
;                 (make-Huff "11100" 1 #\n empty empty) 
;                 (make-Huff "11101" 1 #\t empty empty))
;      (make-Huff "1111" 2 #\o empty empty)))))))
;
;;;Compress Function w/o contracts (for easier viewing) 
;;only name change to compress2
;
;;;compress: string -> (pairof string Huff-tree)
;;;Returns a pair of the bitlist and augmented Huffman tree
;;;that corresponds to the msg input
;(define (compress2 msg)
;  (local
;    {(define (str->aht msg)
;       (local
;         {(define (slopmaker msg)
;            (local
;              {(define (lopmaker msg)
;                 (local
;                   {(define ohlist (string->list msg))
;                    (define (numlistmaker clist)
;                      (cond
;                        [(empty? clist) empty]
;                        [else
;                         (local
;                           {(define charimon (first clist))
;                            (define firstnum 
;                              (length (filter (λ (e) (char=? charimon e)) 
;                                              ohlist)))}
;                           (cons firstnum (numlistmaker (rest clist))))]))}
;                   (remove-duplicates
;                    (build-list (length ohlist) 
;                                (λ (n) (make-pair 
;                                        (list-ref (numlistmaker ohlist) n)
;                                        (list-ref ohlist n)))))))}
;              (sort (lopmaker msg) (λ (p1 p2) (< (pair-a p1) (pair-a p2))))))
;          (define (treemaker list)
;            (map (λ (p) (make-Huff "" (pair-a p) (pair-b p) empty empty)) list))
;          (define (tree-alg tlist)
;            (local
;              {(define (extract tlist)
;                 (local
;                   {(define sortedlist 
;                      (sort tlist (λ (t1 t2) 
;                                    (< (Huff-weight t1) (Huff-weight t2)))))}
;                   (take sortedlist 2)))
;               (define (tmerge 2tlist)
;                 (local
;                   {(define t1 (list-ref 2tlist 0))
;                    (define t2 (list-ref 2tlist 1))}
;                   (make-Huff ""
;                              (+ (Huff-weight t1)
;                                 (Huff-weight t2))
;                              #\NUL
;                              t1
;                              t2)))
;               (define (reinsort t tlist)
;                 (sort (cons t tlist) (λ (t1 t2) 
;                                        (< (Huff-weight t1) (Huff-weight t2)))))
;               (define (pset 1t)
;                 (cond
;                   [(empty? (Huff-tl 1t)) 1t]
;                   [else 
;                    (local
;                      {(define 1lt (Huff-tl 1t))
;                       (define 1rt (Huff-tr 1t))
;                       (define temp (Huff-path 1t))}
;                      (begin
;                        (set-Huff-path! 1lt (string-append temp "0"))
;                        (set-Huff-path! 1rt (string-append temp "1"))
;                        (make-Huff "" (Huff-weight 1t) (Huff-char 1t)
;                                   (pset 1lt)
;                                   (pset 1rt))))]))}
;              (cond
;                [(= 1 (length tlist)) (pset (first tlist))]
;                [else (tree-alg (reinsort 
;                                 (tmerge (extract tlist)) (drop tlist 2)))])))}
;         (tree-alg (treemaker (slopmaker msg)))))
;     (define aht (str->aht msg))
;     (define (encode msg)
;       (local
;         {(define (char->code char tree)
;            (cond
;              [(char=? char (Huff-char tree)) (Huff-path tree)]
;              [(empty? (Huff-tl tree)) ""]
;              [else (string-append
;                     (char->code char (Huff-tl tree))
;                     (char->code char (Huff-tr tree)))]))}
;         (foldr string-append "" (map (λ (char) (char->code char aht))
;                                      (string->list msg)))))}
;    (make-pair (encode msg) aht)))
;
;;;Helper Function tests
;
;(define testlist (string->list "Hello"))
;;;numlistmaker: (listof char) -> (listof num)
;;;Returns a list of the frequency each char in a list
;(define (numlistmaker clist)
;  (cond
;    [(empty? clist) empty]
;    [else
;     (local
;       {(define charimon (first clist))
;        (define firstnum 
;          (length (filter (λ (e) (char=? charimon e)) 
;                          testlist)))}
;       (cons firstnum (numlistmaker (rest clist))))]))
;
;(check-expect (numlistmaker testlist) (list 1 1 2 2 1))
;
;;;lopmaker: string -> (listof (pairof num char))
;;;Creates a list of a pair for each unique character in the
;;;string that has itself and its frequency of occurrence
;(define (lopmaker msg)
;  (local
;    {(define ohlist (string->list msg))
;     (define (numlistmaker clist)
;       (cond
;         [(empty? clist) empty]
;         [else
;          (local
;            {(define charimon (first clist))
;             (define firstnum 
;               (length (filter (λ (e) (char=? charimon e)) 
;                               ohlist)))}
;            (cons firstnum (numlistmaker (rest clist))))]))}
;    (remove-duplicates
;     (build-list (length ohlist) 
;                 (λ (n) (make-pair 
;                         (list-ref (numlistmaker ohlist) n)
;                         (list-ref ohlist n)))))))
;
;(check-expect (lopmaker "SEASHELLS")
;              (list
;               (make-pair 3 #\S)
;               (make-pair 2 #\E)
;               (make-pair 1 #\A)
;               (make-pair 1 #\H)
;               (make-pair 2 #\L)))
;
;;;slopmaker: string -> (listof (pairof num char))
;;;Creates a list of a pair for each unique character in the string
;;;that has itself and its frequency of occurrence, sorted by freq
;(define (slopmaker msg)
;  (local
;    {(define (lopmaker msg)
;       (local
;         {(define ohlist (string->list msg))
;          (define (numlistmaker clist)
;            (cond
;              [(empty? clist) empty]
;              [else
;               (local
;                 {(define charimon (first clist))
;                  (define firstnum 
;                    (length (filter (λ (e) (char=? charimon e)) 
;                                    ohlist)))}
;                 (cons firstnum (numlistmaker (rest clist))))]))}
;         (remove-duplicates
;          (build-list (length ohlist) 
;                      (λ (n) (make-pair 
;                              (list-ref (numlistmaker ohlist) n)
;                              (list-ref ohlist n)))))))}
;    (sort (lopmaker msg) (λ (p1 p2) (< (pair-a p1) (pair-a p2))))))
;
;(check-expect (slopmaker "Sally Sells Seashells")
;              (list
;               (make-pair 1 #\y)
;               (make-pair 1 #\h)
;               (make-pair 2 #\a)
;               (make-pair 2 #\space)
;               (make-pair 3 #\S)
;               (make-pair 3 #\e)
;               (make-pair 3 #\s)
;               (make-pair 6 #\l)))
;
;;;treemaker: (listof (pairof num char)) -> (listof Huff-tree)
;;Returns the list of singleton Huff-trees corresponding to
;;a list of weight/character pairs
;(define (treemaker list)
;  (map (λ (p) (make-Huff "" (pair-a p) (pair-b p) empty empty)) list))
;
;(check-expect (treemaker (list
;                          (make-pair 1 #\y)
;                          (make-pair 1 #\h)
;                          (make-pair 2 #\a)
;                          (make-pair 2 #\space)
;                          (make-pair 3 #\S)
;                          (make-pair 3 #\e)
;                          (make-pair 3 #\s)
;                          (make-pair 6 #\l)))
;              (list
;               (make-Huff "" 1 #\y empty empty)
;               (make-Huff "" 1 #\h empty empty)
;               (make-Huff "" 2 #\a empty empty)
;               (make-Huff "" 2 #\space empty empty)
;               (make-Huff "" 3 #\S empty empty)
;               (make-Huff "" 3 #\e empty empty)
;               (make-Huff "" 3 #\s empty empty)
;               (make-Huff "" 6 #\l empty empty)))
;
;;;extract: (listof Huff-tree) -> (listof Huff-tree)
;;;Gives a list of the 2 Huff-trees with smallest weight in tlist
;(define (extract tlist)
;  (local
;    {(define sortedlist 
;       (sort tlist (λ (t1 t2) 
;                     (< (Huff-weight t1) (Huff-weight t2)))))}
;    (take sortedlist 2)))
;
;(check-expect (extract (list (make-Huff "" 1 #\y empty empty)
;                             (make-Huff "" 4 #\NUL  
;                                        (make-Huff "" 2 #\a empty empty) 
;                                        (make-Huff "" 2 #\space empty empty))
;                             (make-Huff "" 6 #\l empty empty)
;                             (make-Huff "" 3 #\s empty empty)
;                             (make-Huff "" 3 #\S empty empty)
;                             (make-Huff "" 3 #\e empty empty)))
;              (list (make-Huff "" 1 #\y empty empty)
;                    (make-Huff "" 3 #\s empty empty)))
;
;;;tmerge: (listof Huff-tree) -> Huff-tree
;;;Takes a list of 2 Huff-trees and combines them into 1 Huff-tree
;(define (tmerge 2tlist)
;  (local
;    {(define t1 (list-ref 2tlist 0))
;     (define t2 (list-ref 2tlist 1))}
;    (make-Huff ""
;               (+ (Huff-weight t1)
;                  (Huff-weight t2))
;               #\NUL
;               t1
;               t2)))
;
;(check-expect (tmerge (list (make-Huff "" 4 #\NUL  
;                                       (make-Huff "" 2 #\a empty empty) 
;                                       (make-Huff "" 2 #\space empty empty))
;                            (make-Huff "" 6 #\l empty empty)))
;              (make-Huff "" 10 #\NUL
;                         (make-Huff "" 4 #\NUL  
;                                    (make-Huff "" 2 #\a empty empty) 
;                                    (make-Huff "" 2 #\space empty empty))
;                         (make-Huff "" 6 #\l empty empty)))
;
;;;reinsort: Huff-tree (listof Huff-tree) -> (listof Huff-tree)
;;;Returns a list of t and everything in tlist, sorted by weight
;(define (reinsort t tlist)
;  (sort (cons t tlist) (λ (t1 t2) 
;                         (< (Huff-weight t1) (Huff-weight t2)))))
;
;(check-expect (reinsort 
;               (make-Huff "" 10 #\NUL
;                          (make-Huff "" 4 #\NUL  
;                                     (make-Huff "" 2 #\a empty empty) 
;                                     (make-Huff "" 2 #\space empty empty))
;                          (make-Huff "" 6 #\l empty empty))
;               (list (make-Huff "" 1 #\y empty empty)
;                     (make-Huff "" 15 #\p empty empty)
;                     (make-Huff "" 2 #\s empty empty)
;                     (make-Huff "" 7 #\S 
;                                (make-Huff "" 3 #\q empty empty)
;                                (make-Huff "" 4 #\u empty empty))
;                     (make-Huff "" 3 #\e empty empty)))
;              (list (make-Huff "" 1 #\y empty empty)
;                    (make-Huff "" 2 #\s empty empty)
;                    (make-Huff "" 3 #\e empty empty)
;                    (make-Huff "" 7 #\S 
;                               (make-Huff "" 3 #\q empty empty)
;                               (make-Huff "" 4 #\u empty empty))
;                    (make-Huff "" 10 #\NUL
;                               (make-Huff "" 4 #\NUL  
;                                          (make-Huff "" 2 #\a empty empty) 
;                                          (make-Huff "" 2 #\space empty empty))
;                               (make-Huff "" 6 #\l empty empty))
;                    (make-Huff "" 15 #\p empty empty)))
;
;;;pset: Huff-tree -> Huff-tree
;;;creates an augmented Huffman tree from a Huffman tree
;;;by giving leaves their proper paths
;(define (pset 1t)
;  (cond
;    [(empty? (Huff-tl 1t)) 1t]
;    [else 
;     (local
;       {(define 1lt (Huff-tl 1t))
;        (define 1rt (Huff-tr 1t))
;        (define temp (Huff-path 1t))}
;       (begin
;         (set-Huff-path! 1lt (string-append temp "0"))
;         (set-Huff-path! 1rt (string-append temp "1"))
;         (make-Huff "" (Huff-weight 1t) (Huff-char 1t)
;                    (pset 1lt)
;                    (pset 1rt))))]))
;
;(check-expect (pset (make-Huff "" 10 #\NUL
;                               (make-Huff "" 4 #\NUL  
;                                          (make-Huff "" 2 #\a empty empty) 
;                                          (make-Huff "" 2 #\space empty empty))
;                               (make-Huff "" 6 #\l empty empty)))
;              (make-Huff "" 10 #\NUL
;                         (make-Huff "" 4 #\NUL
;                                    (make-Huff "00" 2 #\a empty empty)
;                                    (make-Huff "01" 2 #\space empty empty))
;                         (make-Huff "1" 6 #\l empty empty)))
;
;;;tree-alg: (listof Huff-tree) -> Huff-tree
;;;Returns 1 augmented Huff-tree from a list of singleton Huff-trees
;(define (tree-alg tlist)
;  (local
;    {(define (extract tlist)
;       (local
;         {(define sortedlist 
;            (sort tlist (λ (t1 t2) 
;                          (< (Huff-weight t1) (Huff-weight t2)))))}
;         (take sortedlist 2)))
;     (define (tmerge 2tlist)
;       (local
;         {(define t1 (list-ref 2tlist 0))
;          (define t2 (list-ref 2tlist 1))}
;         (make-Huff ""
;                    (+ (Huff-weight t1)
;                       (Huff-weight t2))
;                    #\NUL
;                    t1
;                    t2)))
;     (define (reinsort t tlist)
;       (sort (cons t tlist) (λ (t1 t2) 
;                              (< (Huff-weight t1) (Huff-weight t2)))))
;     (define (pset 1t)
;       (cond
;         [(empty? (Huff-tl 1t)) 1t]
;         [else 
;          (local
;            {(define 1lt (Huff-tl 1t))
;             (define 1rt (Huff-tr 1t))
;             (define temp (Huff-path 1t))}
;            (begin
;              (set-Huff-path! 1lt (string-append temp "0"))
;              (set-Huff-path! 1rt (string-append temp "1"))
;              (make-Huff "" (Huff-weight 1t) (Huff-char 1t)
;                         (pset 1lt)
;                         (pset 1rt))))]))}
;    (cond
;      [(= 1 (length tlist)) (pset (first tlist))]
;      [else (tree-alg (reinsort 
;                       (tmerge (extract tlist)) (drop tlist 2)))])))
;
;(check-expect (tree-alg (list (make-Huff "" 1 #\y empty empty)
;                              (make-Huff "" 1 #\h empty empty)
;                              (make-Huff "" 2 #\a empty empty)
;                              (make-Huff "" 2 #\space empty empty)
;                              (make-Huff "" 3 #\S empty empty)
;                              (make-Huff "" 3 #\e empty empty)
;                              (make-Huff "" 3 #\s empty empty)
;                              (make-Huff "" 6 #\l empty empty)))
;              (make-Huff
;               ""
;               21
;               #\nul
;               (make-Huff
;                ""
;                9
;                #\nul
;                (make-Huff
;                 ""
;                 4
;                 #\nul
;                 (make-Huff "" 2 #\nul 
;                            (make-Huff "0000" 1 #\y empty empty) 
;                            (make-Huff "0001" 1 #\h empty empty))
;                 (make-Huff "001" 2 #\a empty empty))
;                (make-Huff "" 5 #\nul 
;                           (make-Huff "010" 2 #\space empty empty) 
;                           (make-Huff "011" 3 #\S empty empty)))
;               (make-Huff
;                ""
;                12
;                #\nul
;                (make-Huff "" 6 #\nul 
;                           (make-Huff "100" 3 #\e empty empty) 
;                           (make-Huff "101" 3 #\s empty empty))
;                (make-Huff "11" 6 #\l empty empty))))
;
;;;str->aht: string -> Huff-tree
;;;Returns the augmented Huff tree corresponding to msg
;(define (str->aht msg)
;  (local
;    {(define (slopmaker msg)
;       (local
;         {(define (lopmaker msg)
;            (local
;              {(define ohlist (string->list msg))
;               (define (numlistmaker clist)
;                 (cond
;                   [(empty? clist) empty]
;                   [else
;                    (local
;                      {(define charimon (first clist))
;                       (define firstnum 
;                         (length (filter (λ (e) (char=? charimon e)) 
;                                         ohlist)))}
;                      (cons firstnum (numlistmaker (rest clist))))]))}
;              (remove-duplicates
;               (build-list (length ohlist) 
;                           (λ (n) (make-pair 
;                                   (list-ref (numlistmaker ohlist) n)
;                                   (list-ref ohlist n)))))))}
;         (sort (lopmaker msg) (λ (p1 p2) (< (pair-a p1) (pair-a p2))))))
;     (define (treemaker list)
;       (map (λ (p) (make-Huff "" (pair-a p) (pair-b p) empty empty)) list))
;     (define (tree-alg tlist)
;       (local
;         {(define (extract tlist)
;            (local
;              {(define sortedlist 
;                 (sort tlist (λ (t1 t2) 
;                               (< (Huff-weight t1) (Huff-weight t2)))))}
;              (take sortedlist 2)))
;          (define (tmerge 2tlist)
;            (local
;              {(define t1 (list-ref 2tlist 0))
;               (define t2 (list-ref 2tlist 1))}
;              (make-Huff ""
;                         (+ (Huff-weight t1)
;                            (Huff-weight t2))
;                         #\NUL
;                         t1
;                         t2)))
;          (define (reinsort t tlist)
;            (sort (cons t tlist) (λ (t1 t2) 
;                                   (< (Huff-weight t1) (Huff-weight t2)))))
;          (define (pset 1t)
;            (cond
;              [(empty? (Huff-tl 1t)) 1t]
;              [else 
;               (local
;                 {(define 1lt (Huff-tl 1t))
;                  (define 1rt (Huff-tr 1t))
;                  (define temp (Huff-path 1t))}
;                 (begin
;                   (set-Huff-path! 1lt (string-append temp "0"))
;                   (set-Huff-path! 1rt (string-append temp "1"))
;                   (make-Huff "" (Huff-weight 1t) (Huff-char 1t)
;                              (pset 1lt)
;                              (pset 1rt))))]))}
;         (cond
;           [(= 1 (length tlist)) (pset (first tlist))]
;           [else (tree-alg (reinsort 
;                            (tmerge (extract tlist)) (drop tlist 2)))])))}
;    (tree-alg (treemaker (slopmaker msg)))))
;
;(check-expect (str->aht "Hello World")
;              (make-Huff
;               ""
;               11
;               #\nul
;               (make-Huff
;                ""
;                4
;                #\nul
;                (make-Huff "" 2 #\nul 
;                           (make-Huff "000" 1 #\r empty empty) 
;                           (make-Huff "001" 1 #\d empty empty))
;                (make-Huff "" 2 #\nul 
;                           (make-Huff "010" 1 #\space empty empty) 
;                           (make-Huff "011" 1 #\W empty empty)))
;               (make-Huff
;                ""
;                7
;                #\nul
;                (make-Huff "10" 3 #\l empty empty)
;                (make-Huff
;                 ""
;                 4
;                 #\nul
;                 (make-Huff "" 2 #\nul 
;                            (make-Huff "1100" 1 #\H empty empty) 
;                            (make-Huff "1101" 1 #\e empty empty))
;                 (make-Huff "111" 2 #\o empty empty)))))
;
;;;char->code: char Huff-tree -> string
;;;Returns the Huffcode bitstring of a character given its aug Huff-tree
;(define (char->code char tree)
;  (cond
;    [(char=? char (Huff-char tree)) (Huff-path tree)]
;    [(empty? (Huff-tl tree)) ""]
;    [else (string-append
;           (char->code char (Huff-tl tree))
;           (char->code char (Huff-tr tree)))]))
;
;(check-expect (char->code #\W 
;                          (make-Huff
;                           ""
;                           11
;                           #\nul
;                           (make-Huff
;                            ""
;                            4
;                            #\nul
;                            (make-Huff "" 2 #\nul 
;                                       (make-Huff "000" 1 #\r empty empty) 
;                                       (make-Huff "001" 1 #\d empty empty))
;                            (make-Huff "" 2 #\nul 
;                                       (make-Huff "010" 1 #\space empty empty) 
;                                       (make-Huff "011" 1 #\W empty empty)))
;                           (make-Huff
;                            ""
;                            7
;                            #\nul
;                            (make-Huff "10" 3 #\l empty empty)
;                            (make-Huff
;                             ""
;                             4
;                             #\nul
;                             (make-Huff "" 2 #\nul 
;                                        (make-Huff "1100" 1 #\H empty empty) 
;                                        (make-Huff "1101" 1 #\e empty empty))
;                             (make-Huff "111" 2 #\o empty empty)))))
;              "011")
;
;
;(define testaht (make-Huff
;                 ""
;                 11
;                 #\nul
;                 (make-Huff
;                  ""
;                  4
;                  #\nul
;                  (make-Huff "" 2 #\nul 
;                             (make-Huff "000" 1 #\r empty empty) 
;                             (make-Huff "001" 1 #\d empty empty))
;                  (make-Huff "" 2 #\nul 
;                             (make-Huff "010" 1 #\space empty empty) 
;                             (make-Huff "011" 1 #\W empty empty)))
;                 (make-Huff
;                  ""
;                  7
;                  #\nul
;                  (make-Huff "10" 3 #\l empty empty)
;                  (make-Huff
;                   ""
;                   4
;                   #\nul
;                   (make-Huff "" 2 #\nul 
;                              (make-Huff "1100" 1 #\H empty empty) 
;                              (make-Huff "1101" 1 #\e empty empty))
;                   (make-Huff "111" 2 #\o empty empty)))))
;;;encode: string -> string
;;;Takes a msg and returns its bitstring
;(define (encode msg)
;  (local
;    {(define (char->code char tree)
;       (cond
;         [(char=? char (Huff-char tree)) (Huff-path tree)]
;         [(empty? (Huff-tl tree)) ""]
;         [else (string-append
;                (char->code char (Huff-tl tree))
;                (char->code char (Huff-tr tree)))]))}
;    (foldr string-append "" (map (λ (char) (char->code char testaht))
;                                 (string->list msg)))))
;
;(check-expect (encode "Hello World")
;              "11001101101011101001111100010001")
;
;;;extra stuff (if you'd prefer to see the tree than having to interpret it)
;
;;; show-tree : Huff-tree -> image
;(define (show-tree t)
;  (cond
;    [(empty? t) (square 30 "solid" "white")]
;    [else
;     (local
;       {(define node (overlay (above 
;                               (text (Huff-path t) 10 "black")
;                               (overlay/align/offset 
;                                "right" "center"
;                                (text (make-string 1 (Huff-char t)) 
;                                      10 "black")
;                                10 0
;                                (text (number->string (Huff-weight t))
;                                      10 "black")))
;                              (circle 20 "outline" "black")))
;        (define l-img (show-tree (Huff-tl t)))
;        (define r-img (show-tree (Huff-tr t)))
;        (define l-w (image-width l-img))
;        (define r-w (image-width r-img))
;        (define w (/ (+ (image-width l-img) (image-width r-img)) 2))}
;       (above node
;              (line 0 10 "black")
;              (line w 0 "black")
;              (cond
;                [(> l-w r-w)
;                 (beside/align
;                  "top"
;                  (above (line 0 10 "black") l-img)
;                  (above (line 0 10 "black") r-img)
;                  (line (/ (- l-w r-w) 2) 0 "white"))]
;                [else
;                 (beside/align
;                  "top"
;                  (line (/ (- r-w l-w) 2) 0 "white")
;                  (above (line 0 10 "black") l-img)
;                  (above (line 0 10 "black") r-img))])))]))
;
;;;helpergraderf: string -> (pairof string img)
;;;takes a string and returns the bitlist and the image corresponding
;;;to the augmented Huffman tree 
;(define (helpergraderf msg)
;  (local
;    {(define myfout (compress msg))
;     (define outtree (pair-b myfout))}
;    (begin (set-pair-b! myfout (show-tree outtree))
;           myfout)))