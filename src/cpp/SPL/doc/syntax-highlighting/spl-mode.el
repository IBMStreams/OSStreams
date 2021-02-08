(defvar spl-mode-hook nil)

(defvar spl-mode-map
  (let ((spl-mode-map (make-keymap)))
    (define-key spl-mode-map "\C-j" 'newline-and-indent)
    spl-mode-map)
  "Keymap for SPL major mode")

(add-to-list 'auto-mode-alist '("\\.spl\\'" . spl-mode))
(add-to-list 'auto-mode-alist '("\\.splmm\\'" . spl-mode))

(defvar spl-font-lock-comment-face 'spl-font-lock-comment-face
  "Face name to use for comments.")

(defvar spl-font-lock-config-face 'spl-font-lock-config-face
  "Face name to use for config directives.")

(defvar spl-font-lock-function-name-face 'spl-font-lock-function-name-face
  "Face name to use for function definitions.")

(defvar spl-font-lock-section-name-face 'spl-font-lock-section-name-face
  "Face name to use for section definitions.")

(defvar spl-font-lock-keyword-face 'spl-font-lock-keyword-face
  "Face name to use for SPL keywords.")

(defvar spl-font-lock-operator-name-face 'spl-font-lock-operator-name-face
  "Face name to use for SPL operators.")

(defvar spl-font-lock-control-flow-face 'spl-font-lock-control-flow-face
  "Face name to use for stream type definitions and declarations.")

(defvar spl-font-lock-literal-face 'spl-font-lock-literal-face
  "Face name to use for SPL literals.")

(defvar spl-font-lock-type-name-face 'spl-font-lock-type-name-face
  "Face name to use for type definitions and declarations.")

(defvar spl-font-lock-variable-name-face 'spl-font-lock-variable-name-face
  "Face name to use for type definitions and declarations.")

;; Custom faces to use for spl-font-lock.  Initialized from the standard
;; font lock faces
(defgroup spl-font-lock-faces nil
  "Faces used by the spl-font-lock package."
  :prefix "spl-font-lock"
  :group 'font-lock-faces)

(defface spl-font-lock-comment-face 
  '((((class color)(background light)) (:foreground "LavenderBlush4"))
    (((class color)) (:foreground "CadetBlue"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight comments."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-control-flow-face 
  '((((class color)(background light)) (:foreground "Goldenrod" :bold t))
    (((class color)) (:foreground "LightGoldenrod" :bold t))
    (t (:inverse-video t)))
  "Font Lock face used to highlight special types."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-function-name-face 
  '((((class color)(background light)) (:foreground "NavajoWhite3"))
    (((class color)) (:foreground "NavajoWhite1"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight function names."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-section-name-face 
  '((((class color)(background light)) (:foreground "Blue" :bold t))
    (((class color)) (:foreground "SkyBlue" :bold t))
    (t (:inverse-video t)))
  "Font Lock face used to highlight sections."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-keyword-face 
  '((((class color)(background light)) (:foreground "DarkYellow" :bold t))
    (((class color)) (:foreground "Yellow" :bold t))
    (t (:inverse-video t)))
  "Font Lock face used to highlight keywords."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-config-face 
  '((((class color)(background light)) (:foreground "IndianRed"))
    (((class color)) (:foreground "NavajoWhite3"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight config directives."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-literal-face 
  '((((class color) (background light)) (:foreground "ForestGreen"))
    (((class color)) (:foreground "SpringGreen"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight literals."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-type-name-face 
  '((((class color)(background light)) (:foreground "DarkOliveGreen"))
    (((class color)) (:foreground "DarkSeaGreen1"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight type names."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-operator-name-face 
  '((((class color)(background light)) (:foreground "SlateBlue"))
    (((class color)) (:foreground "MediumSlateBlue"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight operators."
  :group 'spl-font-lock-faces)

(defface spl-font-lock-variable-name-face 
  '((((class color)(background light)) (:foreground "DarkRed"))
    (((class color)) (:foreground "Red"))
    (t (:inverse-video t)))
  "Font Lock face used to highlight variable names."
  :group 'spl-font-lock-faces)

(defun set-my-colors ()  
  (progn (setq font-lock-string-face spl-font-lock-literal-face))
  (progn (setq font-lock-comment-face spl-font-lock-comment-face))
)

(defconst spl-font-lock-keywords-1
  (list '("\\<\\(namespace\\|composite\\|param\\|type\\|var\\|graph\\|config\\|logic\\|window\\|state\\input\\|state\\|onTuple\\|onPunct\\|output\\)\\>" 
         . spl-font-lock-section-name-face)
        '("\\<\\(use\\|stream\\|public\\|stateful\\|mutable\\|static\\|attribute\\|expression\\|function\\|sliding\\|tumbling\\)\\>" 
         . spl-font-lock-keyword-face)
        '("\\(\\$\\)\\([a-zA-Z][a-zA-Z0-9_]*\\)" 1 spl-font-lock-section-name-face)
        '("\\<\\(Custom\\|Beacon\\|Functor\\|Filter\\|DynamicFilter\\|Punctor\\|Aggregate\\|DeDuplicate\\|ThreadedSplit\\|Barrier\\|Pair\\|Sort\\|Split\\|Delay\\|Throttle\\|Join\\|Union\\|DirectoryScan\\|FileSource\\|FileSink\\|TCPSource\\|TCPSink\\|UDPSource\\|UDPSink\\|PerfSink\\|Import\\|Gate\\|Parse\\|Format\\|Compress\\|Decompress\\|CharacterTransform\\|Export\\)\\>" 
         . spl-font-lock-operator-name-face)
        '("\\<\\(if\\|else\\|for\\|while\\|continue\\|break\\)\\>" 
         . spl-font-lock-control-flow-face))
  "Minimal highlighting expressions for SPL mode")

(defconst spl-font-lock-keywords-2
  (append spl-font-lock-keywords-1
    (list '("\\<\\(threadedPort\\|queue\\|checkpoint\\|defaultPoolSize\\|haReplicas\\|placement\\|partition\\|host\\|\\(\\(partition\\|host\\)\\(Exlocation\\|Colocation\\|Isolation\\)\\)\\|hostPool\\|logLevel\\|relocatable\\|restartable\\|wrapper\\consistency\\|atomic\\|causal\\|loose\\|lifetime\\|ethernal\\|transient\\|readsPerSecond\\|sizeHint\\|writesPerSecond\\)\\>" 
           . spl-font-lock-config-face)
          '("\\<\\(partitioned\\|time\\|count\\|delta\\|punct\\|Any\\|Avg\\|Sum\\|Cnt\\|DCnt\\|VCnt\\|MCnt\\|GCnt\\|BCnt\\|Col\\|DCol\\|Last\\|First\\|Min\\|Max\\)\\>" 
           . font-lock-function-name-face)))
  "Additional Keywords to highlight in SPL mode")

(defconst spl-font-lock-keywords-3
  (append spl-font-lock-keywords-2
    (list '("\\<\\(void\\|boolean\\|enum\\|\\(\\(int\\|uint\\|float\\|decimal\\|complex\\)\\(8\\|16\\|32\\|64\\|128\\)\\)\\|timestamp\\|blob\\|xml\\|rstring\\|ustring\\|list\\|map\\|set\\|tuple\\)\\>" 
           . spl-font-lock-type-name-face)
          '("\\<\\(\\true\\|false\\|\\(\\([0-9]+\\|\\([0-9]*\\.[0-9]+\\([+-]e[0-9]+\\)?\\)\\)\\(s\\|u\\|f\\|d\\)?\\(b\\|h\\|w\\|l\\|q\\)?\\)\\)\\>" 
           . spl-font-lock-literal-face)))
   "Highlighting in SPL mode")

(defvar spl-font-lock-keywords spl-font-lock-keywords-3
  "Default highlighting expressions for SPL mode")

(defvar spl-mode-syntax-table
  (let ((spl-mode-syntax-table (make-syntax-table)))
       (modify-syntax-entry ?/ ". 124b" spl-mode-syntax-table)
       (modify-syntax-entry ?* ". 23" spl-mode-syntax-table)
       (modify-syntax-entry ?\n "> b" spl-mode-syntax-table)
       spl-mode-syntax-table)
  "Syntax table for spl-mode")

(defun spl-mode ()
  "Major mode for editing SPL (SPL) Language files"
  (interactive)
  (kill-all-local-variables)
  (c-mode) ;; borrow c-mode
  (custom-set-variables '(c-basic-offset 2))
  (set-syntax-table spl-mode-syntax-table)
  (use-local-map spl-mode-map)
  (set (make-local-variable 'font-lock-defaults) '(spl-font-lock-keywords))
  (set (make-local-variable 'indent-line-function) 'c-indent-line)  
  (setq major-mode 'spl-mode)
  (setq mode-name "SPL")
  (run-hooks 'spl-mode-hook))

(provide 'spl-mode)







