;;; smodels-mode.el -- Major mode for editing smodels programs under
;;; 		       Emacs. 
;;; Copyright (C) 1999-2000 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
;;;
;;; Author: Tommi Syrjänen
;;;  
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;  
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;  
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;;;  

;;; Commentary:
;;; This package provides a major mode for editing smodels programs.
;;; It handles indentation and syntax highlighting and knows how to
;;; start smodels processes. 

;;; This mode is loosely based on prolog.el by Masanobu UMEDA
;;; <umerin@mse.kyutech.ac.jp> and compile.el by Roland McGrath
;;; <roland@prep.ai.mit.edu> 

;;;
;;; Recent Changes:

;;; - added font lock directives for `#foo' statements.


;;; BUGS:
;;; - none that I know


;;; Code:


(defgroup smodels nil
  "Major mode for editing and running Smodels under Emacs"
  :group 'languages)

(defvar smodels-mode-syntax-table nil
  "Syntax table used while in smodels-mode")

(defvar smodels-mode-abbrev-table nil
  "Abbrev table used while in smodels-mode")

(defvar smodels-mode-map nil
  "Keymap used while in smodels-mode")

  
(defcustom smodels-program-name "smodels"
  "*Program name for finding stable models using `smodels-find-models'"
  :type 'string
  :group 'smodels)

(defcustom smodels-parser-name "lparse"
  "*Program that is used to parse the smodels programs using
`smodels-parse-buffer' and `smodels-find-models'"
  :type 'string
  :group 'smodels)

(defcustom smodels-indent-width 4
  "Level of indentation in Smodels buffers."
  :type 'integer
  :group 'smodels)

(if smodels-mode-syntax-table
    ()
  (let ((table (make-syntax-table)))
    (modify-syntax-entry ?_ "w" table)
    (modify-syntax-entry ?\\ "\\" table)
    (modify-syntax-entry ?/ "." table)
    (modify-syntax-entry ?* "." table)
    (modify-syntax-entry ?+ "." table)
    (modify-syntax-entry ?- "." table)
    (modify-syntax-entry ?= "." table)
    (modify-syntax-entry ?% "<" table)
    (modify-syntax-entry ?\n ">" table)
    (modify-syntax-entry ?< "." table)
    (modify-syntax-entry ?> "." table)
    (modify-syntax-entry ?\' "w" table)
    (modify-syntax-entry ?\` "w" table)
    (modify-syntax-entry ?! "." table)
    (modify-syntax-entry ?' "w" table)
    (modify-syntax-entry ?# "w" table)
    (setq smodels-mode-syntax-table table)))

(define-abbrev-table 'smodels-mode-abbrev-table ())

(defun smodels-mode-variables ()
  (set-syntax-table smodels-mode-syntax-table)
  (setq local-abbrev-table smodels-mode-abbrev-table)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "%%\\|$\\|" page-delimiter)) ;'%%..'
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'smodels-indent-line)
  (make-local-variable 'comment-start)
  (setq comment-start "%")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "%+ *")
  (make-local-variable 'comment-column)
  (setq comment-column 0)
  (make-local-variable 'comment-indent-function)
  (setq comment-indent-function 'smodels-comment-indent)
  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults '(smodels-font-lock-keywords)))
				 

(defconst smodels-font-lock-keywords
  (list
   (list
    (concat
     "\\<\\("
     "not\\|"
     "weight\\|"
     "compute\\|"
     "minimize\\|"
     "maximize\\|"
     "function\\|"
     "define\\|"
     "const\\|"
     "hide\\|"
     "show\\|"
     "external\\|"
     "#[A-Za-z]+"
     "\\)\\>")
    '(1 font-lock-keyword-face))
   (cons "\\<[A-Z][_a-zA-Z0-9]*\\>"
	 'font-lock-variable-name-face)
   (cons (concat "\\<\\("
		 "eq\\|"
		 "neq\\|"
		 "lt\\|"
		 "gt\\|"
		 "le\\|"
		 "ge\\|"
		 "assign\\|"
		 "plus\\|"
		 "minus\\|"
		 "times\\|"
		 "mod"
		 "\\)\\>")
	 'font-lock-builtin-face)))


(defun smodels-mode-commands (map)
  (define-key map "\t" 'smodels-indent-line)
  (define-key map "\C-c\C-b" 'smodels-parse-buffer)
  (define-key map "\C-c\C-f" 'smodels-parse-files)
  (define-key map "\C-t\C-b" 'smodels-compute-buffer)
  (define-key map "\C-t\C-f" 'smodels-compute-files))

(if smodels-mode-map
    nil
  (setq smodels-mode-map (make-sparse-keymap))
  (smodels-mode-commands smodels-mode-map))

;;;###autoload
(defun smodels-mode ()
  "Major mode for editing Smodels code.
Blank lines and `%%...' separate paragraphs.  `%'s start comments.
Commands:
\\{smodels-mode-map}
Entry to this mode calls the value of `smodels-mode-hook'
if that value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map smodels-mode-map)
  (setq major-mode 'smodels-mode)
  (setq mode-name "Smodels")
  (smodels-mode-variables)
  (run-hooks 'smodels-mode-hook))

(defun smodels-indent-line ()
  "Indent current line as Smodels code."
  (interactive)
  (let ((indent (smodels-indent-level))
	(pos (- (point-max) (point))) beg)
    (beginning-of-line)
    (setq beg (point))
    (skip-chars-forward " \t")
    (if (zerop (- indent (current-column)))
	nil
      (delete-region beg (point))
      (indent-to indent))
    (if (> (- (point-max) pos) (point))
	(goto-char (- (point-max) pos)))
    ))

(defun smodels-indent-level ()
  "Compute smodels indentation level."
  (save-excursion
    (beginning-of-line)
    (skip-chars-forward " \t")
    (let ((paren-indent-column (smodels-inside-parens)))
      (cond
       ;; Are we looking at a major comment?
       ((looking-at "%%") 0)
       ;; Beginning of the buffer?
       ((bobp) 0)
       ;; inside parenthesis?
       (paren-indent-column
	paren-indent-column)
       ;; inside a rule?
       ((smodels-inside-rule) 
	smodels-indent-width)
       ;; else we are in global level 
       (t 0)))))
  
(defun smodels-inside-parens ()
  "Go backwards the buffer and count all parenthesis (plus brackets
  and braces). If an open parenthesis is found return the correct
  intendation column. If a '.' or beginning of buffer is foud
  return 'nil'"
  (save-excursion
    (forward-line -1)
    (end-of-line)
    (let ((found-end nil)
	  (paren-depth 0)
	  (paren-column 0))
      (while (and (not found-end) (>= paren-depth 0))
	(cond ((or (bobp) (looking-at "\\.") )
	       (setq found-end t))
	      ((looking-at ")\\|}\\|\\]") ; close parenthesis
	       (setq paren-depth (+ paren-depth 1)))
	      ((looking-at "(\\|{\\|\\[") ; open parenthesis
	       (setq paren-depth (- paren-depth 1))
	       ; skip forward all whitespace, and set the indent level
	       ; there
	       (if (< paren-depth 0)
		   (progn
		     (forward-char)
		     (skip-chars-forward " \t")
		     (setq paren-column (current-column))))))
	(if (not found-end)
	    (backward-char)))
      (if (< paren-depth 0)
	  paren-column
	nil))))

(defun smodels-inside-rule ()
  "Find out if the current point is within a rule by searching
backwards to find either ':-', '.' or beginning of buffer"
  (save-excursion
    (forward-line -1)
    (end-of-line)
    (let ((found-end nil)
	  (inside-quoted nil)
	  (found-implication nil))
      (while (and (not found-end) (not found-implication))
	(cond ((looking-at "\"")
	       (setq inside-quoted (not inside-quoted)))
	      ((or (bobp)
		   (and (looking-at "\\.")
			(not inside-quoted)))
	       (setq found-end t))
	      ((looking-at ":-")
	       (setq found-implication t)))
	(if (not found-end)
	    (backward-char)))
	found-implication)))

;;;
;;; Interaction with lparse and smodels
;;;


(defvar smodels-program-history '()
  "minibuffer history to use with smodels program related queries")

(defvar smodels-parser-history nil
  "minibuffer history to use with smodels parser related queries")

(defvar smodels-arguments-history nil
  "minibuffer history to use with argument related queries")

(defvar smodels-parser-arguments "-W all"
  "Arguments that are passed to smodels parser. The
argument `-t' shouldn't be used here, since `smodels-parse-*'
functions add it automatically")

(defvar smodels-program-arguments ""
  "Arguments that are passed to smodels program when it is invoked
with `smodels-compute-buffer'.")

(defvar smodels-file-list nil
  "List of files that are processed by `smodels-parse-files' and
`smodels-compute-files'.")

(defun smodels-set-program (&optional filename)
  "Choose the program that calculates stable models. By default 'smodels'"
  (interactive "P")
  (setq smodels-program-name
	(if filename
	    filename
	  (read-from-minibuffer "Enter smodels program name: "
				"" nil nil
				smodels-program-history))))

(defun smodels-set-parser (&optional filename)
  "Choose the program that parses smodels programs. By default 'lparse'"
  (interactive "P")
  (setq smodels-parser-name
	(if filename
	    filename
	  (read-from-minibuffer "Enter smodels parser name: "
				"" nil nil
				smodels-parser-history))))


(defun smodels-set-parser-arguments (&optional args)
  "Set smodels-parser-arguments."
  (interactive "P") 
  (setq smodels-parser-arguments
	(if args
	    args
	  (read-from-minibuffer "Parser arguments (with smodels): "
				smodels-parser-arguments nil nil
				smodels-arguments-history))))


(defun smodels-set-program-arguments (&optional args)
  "Set smodels-program-arguments"
  (interactive "P")
  (setq smodels-program-arguments
	(if args
	    args
	  (read-from-minibuffer "Smodels arguments: "
				smodels-program-arguments
				nil nil
				smodels-arguments-history))))

(defun smodels-set-file-list (&optional args)
  "Set the list of files that belong to this smodels program"
  (interactive "P")
  (setq smodels-file-list
	(if args
	    args
	  (read-from-minibuffer "Smodels files: "
				smodels-file-list
				nil
				nil
				smodels-arguments-history))))

(defun smodels-parse-buffer (&optional non-text)
  "Run current buffer through smodels parser and display the results
in a new window."
  (interactive "P")
  (smodels-start-process t non-text (buffer-name)))

(defun smodels-parse-files (&optional non-text)
  "Run the files of file-list  through smodels parser and display 
the results in a new window. Uses smodels-parse-buffer-arguments as its 
arguments. If `file-list' argument is not given,
`smodels-file-list' is used instead."
  (interactive "P")
  (save-excursion
    (let* ((files (smodels-get-file-list)))
      (smodels-start-process t non-text files))))
	   

(defun smodels-compute-buffer ()
  "Find stable models of the program in this buffer."
  (interactive)
  (smodels-start-process nil nil (buffer-name)))


(defun smodels-compute-files (&optional file-list)
  "Run the files of file-list through smodels parser, pipe the output to
smodels program and display the results in a new window. If
`file-list' argument is not given, `smodels-file-list' is used
instead." 
  (interactive "P")
  (save-excursion
    (let* ((files (if file-list
		      file-list
		    (smodels-get-file-list))))
      (smodels-start-process nil nil files))))


(defun smodels-comment-indent ()
  "Compute smodels comment indentation"
  0)

(defun smodels-start-process (only-parse non-text file-list)
  "Run a smodels/parser process to files of `file-list'. If
`only-parse' is true, output the ground program, otherwise run it
through smodels. If both `only-parse' and `non-text' are true the
output is ground program in smodels internal format."
  (save-excursion 
    (let* ((name-of-mode smodels-parser-name)
	   (mode "smodels")
	   (bufname (concat "*" (downcase mode) "*"))
	   (outbuf (get-buffer bufname)))
      (if (not outbuf)
	  (setq outbuf (get-buffer-create bufname)))
      (set-buffer outbuf)
      (let ((parse-proc (get-buffer-process (current-buffer))))
	(if parse-proc
	    (if (or (not (eq (process-status parse-proc) 'run))
		    (yes-or-no-p
		     (format "A %s process is running; kill it? "
			     name-of-mode)))
		(condition-case ()
		    (progn
		      (interrupt-process parse-proc)
		      (sit-for 1)
		      (delete-process parse-proc))
		  (error nil))
	      (error "Cannot have two processes in `%s' at once"
		     (buffer-name))
	      ))
	(kill-all-local-variables)
	(setq buffer-read-only nil)
	(buffer-enable-undo (current-buffer))
	;; construct the actual command
	(let ((command nil))
	  (if only-parse
	      (setq command (concat smodels-parser-name
				    (if non-text
					" "
				      " -t ")
				    smodels-parser-arguments " "
				    file-list))
	    (setq command (concat smodels-parser-name " "
				  smodels-parser-arguments " "
				  file-list " | "
				  smodels-program-name " "
				  smodels-program-arguments)))
	  (set-buffer-modified-p nil)
	  ;; go to the end of buffer
	  (if (eq outbuf (current-buffer))
	      (goto-char (point-max)))
	  ;; pop up the window
	  (let ((outwin (display-buffer outbuf)))
	    (save-excursion
	      (set-buffer outbuf)
	      ;; start the actual parsing
	      (insert command "\n")
	      (start-process-shell-command (downcase mode-name)
					   outbuf
					   command))))))))


(defun smodels-get-file-list ()
  (interactive "P")
  "Initialize the file list if needed"
  (if smodels-file-list
      smodels-file-list
    (setq smodels-file-list (read-from-minibuffer "Input files: "))
    smodels-file-list))

(provide 'smodels)

;;; smodels.el ends here
