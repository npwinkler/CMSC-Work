;; The first three lines of this file were inserted by DrRacket. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-advanced-reader.ss" "lang")((modname |long-winded working lab5|) (read-case-sensitive #t) (teachpacks ()) (htdp-settings #(#t constructor repeating-decimal #t #t none #f ())))
;;Lab 5 - Nolan Winkler

(require racket/list)

;; an fd (file or directory) is either
;; - a (make-file name type size)
;;   where name and type are strings, and 
;;   size (in bytes) is a num, or
;; - a (make-dir name contents)
;;   where name is a string and contents is a (listof fd)

;; a (listof fd) is either
;; - empty, or
;; - (cons f fs) where f is an fd and fs is a (listof fd)

(define-struct file (name type size))
(define-struct dir (name contents))

(define filesys-ex0
  (make-dir
   "d0"
   (list (make-dir 
          "d1"
          (list (make-file "a" "rkt" 1)
                (make-file "b" "jpg" 2)
                (make-file "c" "txt" 4)))
         (make-dir 
          "d2"
          (list (make-file "a" "jpg" 8)
                (make-file "b" "xls" 16)
                (make-dir 
                 "d3"
                 (list (make-file "a" "png" 32)
                       (make-file "b" "doc" 64))))))))

;Problem 1
;; image-files : fd -> (listof file)
;;Returns the files that are images (.png, .jpg, .png) in a fd
(define (image-files fd)
  (cond
    [(file? fd) (if 
                 (or (string=? (file-type fd) "jpg")
                     (string=? (file-type fd) "gif")
                     (string=? (file-type fd) "png"))
                 (list fd)
                 empty)]
    [(dir? fd) (image-files/list (dir-contents fd))]))

;;image-files/list: fdlist -> fdlist
(define (image-files/list fds)
  (cond
    [(empty? fds) empty]
    [else (append (image-files (first fds)) (image-files/list (rest fds)))]))

(check-expect (image-files filesys-ex0) (list (make-file "b" "jpg" 2)
                                              (make-file "a" "jpg" 8)
                                              (make-file "a" "png" 32)))
(check-expect (image-files (make-file "a" "jpg" 8))
              (list (make-file "a" "jpg" 8)))
(check-expect (image-files (make-file "c" "txt" 4))
              empty)

;Problem 2
;;files-larger-than: num fd -> (listof file)
;;Returns the files with sizes larger than num (bytes) in a fd
(define (files-larger-than num fd)
  (cond
    [(file? fd) (if
                 (> (file-size fd) num)
                 (list fd)
                 empty)]
    [(dir? fd) (files-larger-than/list num (dir-contents fd))]))

;;files-larger-than/list: num fdlist -> fdlist
(define (files-larger-than/list num fds)
  (cond
    [(empty? fds) empty]
    [else (append (files-larger-than num (first fds)) (files-larger-than/list num (rest fds)))]))

(check-expect (files-larger-than 16 filesys-ex0)
              (list (make-file "a" "png" 32)
                    (make-file "b" "doc" 64)))
(check-expect (files-larger-than 16 (make-file "c" "txt" 4))
              empty)
(check-expect (files-larger-than 16 (make-file "a" "png" 32))
              (list (make-file "a" "png" 32)))

;Problem 3
;;distinct-file-types: fd -> (listof string)
;;Returns the distinct file types in fd
(define (distinct-file-types fd)
  (cond
    [(file? fd) (list (file-type fd))]
    [(dir? fd) (distinct-file-types/list (dir-contents fd))]))

;;distinct-file-types/list: fdlist -> fdlist
(define (distinct-file-types/list fds)
  (cond
    [(empty? fds) empty]
    [else (remove-duplicates (append (distinct-file-types (first fds))
                                     (distinct-file-types/list (rest fds))))]))

(check-expect (distinct-file-types filesys-ex0)
              (list "rkt" "jpg" "txt" "xls" "png" "doc"))
(check-expect (distinct-file-types (make-file "a" "png" 32))
              (list "png"))

;Problem 4

;; a path is a (listof string)

;;file=? file file -> bool
;;Returns true if two files are equal, false otherwise
(define (file=? f1 f2)
  (and (string=? (file-name f1) (file-name f2))
       (string=? (file-type f1) (file-type f2))
       (= (file-size f1) (file-size f2))))

(check-expect (file=? (make-file "b" "jpg" 2) (make-file "b" "jpg" 2)) true)
(check-expect (file=? (make-file "b" "jpg" 2) (make-file "b" "xls" 16)) false)

;;get: file fd -> (listof file)
;;Returns a list of just that file if it is in fd, empty otherwise
(define (get file fd)
  (cond
    [(file? fd) (if (file=? file fd)
                    (list file)
                    empty)]
    [(dir? fd) (append (get/list file (dir-contents fd)))]))

(define (get/list file fds)
  (cond
    [(empty? fds) empty]
    [else (append (get file (first fds)) (get/list file (rest fds)))]))

(check-expect (get (make-file "b" "xls" 16) filesys-ex0)
              (list (make-file "b" "xls" 16)))
(check-expect (get (make-file "b" "wfwfw" 16) filesys-ex0)
              empty)

;;filein: file dir -> bool
;;Tests if file is in a directory (including its subdirectories)
(define (filein? file dir)
  (< 0 (length (get file dir))))

(check-expect (filein? (make-file "b" "xls" 16) filesys-ex0) true)
(check-expect (filein? (make-file "b" "wfwfw" 16) filesys-ex0) false)

;;files-with-name: name fd -> (listof file)
;;Returns a list of all files with name name in fd
(define (files-with-name name fd)
       (cond
         [(file? fd) (if (string=? (file-name fd) name)
                         (list fd)
                         empty)]
         [(dir? fd) (if (string=? (dir-name fd) name)
                        (list fd)
                        (append (files-with-name/list name (dir-contents fd))))]))

(define (files-with-name/list name fds)
  (cond
    [(empty? fds) empty]
    [else (append (files-with-name name (first fds)) (files-with-name/list name (rest fds)))]))

(check-expect (files-with-name "a" filesys-ex0)
              (list 
               (make-file "a" "rkt" 1)
               (make-file "a" "jpg" 8)
               (make-file "a" "png" 32)))
(check-expect (files-with-name "q" filesys-ex0)
              empty)

;;pathget: file fd -> (listof path)
;;Returns the path of a single file in a fd
(define (pathget file fd)
  (cond
    [(empty? fd) empty]
    [(file? fd) (if (file=? file fd) 
                    (list (string-append (file-name fd) "." (file-type fd)))
                    empty)]
    [(dir? fd) (if (filein? file fd) 
                   (append (list (dir-name fd)) (pathget/list file (dir-contents fd)))
                   (pathget/list file (dir-contents fd)))]))

(define (pathget/list file fds)
  (cond
    [(empty? fds) empty]
    [else (append (pathget file (first fds)) (pathget/list file (rest fds)))]))

(check-expect (pathget (make-file "b" "doc" 64) filesys-ex0)
              (list "d0" "d2" "d3" "b.doc"))
(check-expect (pathget (make-file "a" "rkt" 1) filesys-ex0)
              (list "d0" "d1" "a.rkt"))
(check-expect (pathget (make-file "q" "sjsj" 3) filesys-ex0)
              empty)

;;paths-to: name fd -> (listof path)
;;Returns the path of all files with name name in a fd
(define (paths-to name fd)
  (local  
    {(define filelist (files-with-name name fd))}
    (map (λ (file) (pathget file fd)) filelist)))

(check-expect (paths-to "a" filesys-ex0)
              (list 
               (list "d0" "d1" "a.rkt")
               (list "d0" "d2" "a.jpg")
               (list "d0" "d2" "d3" "a.png")))

(check-expect (paths-to "b" filesys-ex0)
              (list
               (list "d0" "d1" "b.jpg")
               (list "d0" "d2" "b.xls")
               (list "d0" "d2" "d3" "b.doc")))

              

                 