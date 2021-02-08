" Vim syntax file
" Language:	SPL

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword     splSTOperator	Custom Beacon Functor Filter DirectoryScan DynamicFilter Punctor Aggregate DeDuplicate ThreadedSplit Barrier Pair Sort Split Delay Throttle Join Union DirectoryScan FileSource FileSink TCPSource TCPSink UDPSource UDPSink PerfSink Import Export JavaOp Punctor Gate XMLParse Switch MetricsSink V1TCPSource V1TCPSink Gate Parse Format Compress Decompress CharacterTransform
syn keyword     splStorageClass	mutable static public stateful
syn keyword	splType           uint8 int8 uint16 int16 uint32 int32 uint64 int64 float32 float64 decimal32 decimal64 decimal128 complex32 complex64 tuple boolean void timestamp rstring ustring map list set enum blob xml expression operator function attribute
syn keyword	splStatement      namespace composite stream as return use state var onTuple onPunct onProcess
syn keyword	splStructure	logic param input output config window graph type
syn keyword	splRepeat		if else while break for in
syn keyword	splPunctMark		Sys FinalMarker WindowMarker
syn keyword	splConstant		false true

syn match       splAnnotation '@\(autonomous\|catch\|consistent\|parallel\|threading\|view\|spl_note\|spl_category\|spl_tag\)'

" splCommentGroup allows adding matches for special things in comments
syn cluster	splCommentGroup	contains=splTodo

" String and Character constants
" Highlight special characters (those which have a backslash) differently
syn match	splSpecial	display contained "\\\(x\x\+\|\o\{1,3}\|.\|$\)"
if !exists("c_no_utf")
  syn match	splSpecial	display contained "\\\(u\x\{4}\|U\x\{8}\)"
endif
if exists("c_no_cformat")
  syn region	splString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=splSpecial,@Spell
else
  if !exists("c_no_c99") " ISO C99
    syn match	splFormat		display "%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlLjzt]\|ll\|hh\)\=\([aAbdiuoxXDOUfFeEgGcCsSpn]\|\[\^\=.[^]]*\]\)" contained
  else
    syn match	splFormat		display "%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlL]\|ll\)\=\([bdiuoxXDOUfeEgGcCsSpn]\|\[\^\=.[^]]*\]\)" contained
  endif
  syn match	splFormat		display "%%" contained
  syn region	splString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=splSpecial,splFormat,@Spell
  syn region	splString		start=+L\='+ skip=+\\\\\|\\'+ end=+'+ contains=splSpecial,splFormat,@Spell
endif

syn match	splCharacter	"L\='[^\\]'"
syn match	splCharacter	"L'[^']*'" contains=splSpecial
if exists("c_gnu")
  syn match	splSpecialError	"L\='\\[^'\"?\\abefnrtv]'"
  syn match	splSpecialCharacter "L\='\\['\"?\\abefnrtv]'"
else
  syn match	splSpecialError	"L\='\\[^'\"?\\abfnrtv]'"
  syn match	splSpecialCharacter "L\='\\['\"?\\abfnrtv]'"
endif
syn match	splSpecialCharacter display "L\='\\\o\{1,3}'"
syn match	splSpecialCharacter display "'\\x\x\{1,2}'"
syn match	splSpecialCharacter display "L'\\x\x\+'"

"when wanted, highlight trailing white space
if exists("c_space_errors")
  if !exists("c_no_trail_space_error")
    syn match	splSpaceError	display excludenl "\s\+$"
  endif
  if !exists("c_no_tab_space_error")
    syn match	splSpaceError	display " \+\t"me=e-1
  endif
endif

"integer number (optional suffixes), or floating point number without a dot and with suffixes
syn case ignore
syn match	splNumbers	display transparent "\<\d\|\.\d" contains=splNumber,splFloat
" Same, but without octal error (for comments)
syn match	splNumbersCom	display contained transparent "\<\d\|\.\d" contains=splNumber,splFloat
syn match	splNumber	display contained "\d\+[bhwl][us]\>"
syn match	splNumber	display contained "\d\+[us][bhwl]\>"
syn match	splNumber	display contained "\d\+[usbhwl]\=\>"
"hex number
syn match	splNumber	display contained "0x\x\+[us]\=\>"

syn match	splFloat	display contained "\d\+[wlq][df]"
syn match	splFloat	display contained "\d\+[df][wlq]\="
"floating point number, with dot, optional exponent
syn match	splFloat	display contained "\d\+\.\d*\(e[-+]\=\d\+\)\=\(\([df][wlq]\)\|\([wlq][df]\)\|[wlq]\|[df]\)\="
"floating point number, starting with a dot, optional exponent
syn match	splFloat	display contained "\.\d*\(e[-+]\=\d\+\)\=\(\([df][wlq]\)\|\([wlq][df]\)\|[wlq]\|[df]\)\="
"floating point number, without dot, with exponent
syn match	splFloat	display contained "\d\+e[-+]\=\d\+\(\([df][wlq]\)\|\([wlq][df]\)\|[wlq]\|[df]\)\="

syn case match

if exists("c_comment_strings")
  " A comment can contain cString, cCharacter and splNumber.
  " But a "*/" inside a splString in a splComment DOES end the splComment!  So we
  " need to use a special type of splString: splCommentString, which also ends on
  " "*/", and sees a "*" at the start of the line as comment again.
  " Unfortunately this doesn't very well work for // type of comments :-(
  syntax match	splCommentSkip	contained "^\s*\*\($\|\s\+\)"
  syntax region splCommentString	contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=splSpecial,splCommentSkip
  syntax region splComment2String	contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=splSpecial
  syntax region  splCommentL	start="//" skip="\\$" end="$" keepend contains=@cCommentGroup,splComment2String,splCharacter,splNumbersCom,splSpaceError,@Spell
  if exists("c_no_comment_fold")
    syntax region splComment	matchgroup=splCommentStart start="/\*" end="\*/" contains=@cCommentGroup,splCommentStartError,splCommentString,splCharacter,splNumbersCom,splSpaceError,@Spell
  else
    syntax region splComment	matchgroup=splCommentStart start="/\*" end="\*/" contains=@cCommentGroup,splCommentStartError,splCommentString,splCharacter,splNumbersCom,splSpaceError,@Spell fold
  endif
else
  syn region	splCommentL	start="//" skip="\\$" end="$" keepend contains=@cCommentGroup,splSpaceError,@Spell
  if exists("c_no_comment_fold")
    syn region	splComment	matchgroup=splCommentStart start="/\*" end="\*/" contains=@cCommentGroup,splCommentStartError,splSpaceError,@Spell
  else
    syn region	splComment	matchgroup=splCommentStart start="/\*" end="\*/" contains=@cCommentGroup,splCommentStartError,splSpaceError,@Spell fold
  endif
endif
" keep a // comment separately, it terminates a preproc. conditional
syntax match	splCommentError	display "\*/"
syntax match	splCommentStartError display "/\*"me=e-1 contained

syntax region	splBlock		start="{" end="}" transparent fold
syntax match    splXML                  /xml\s*<\s*\(\(\".*\"\)\|\('.*'\)\)r\?\s*>\|\<xml\>/

if !exists("c_no_if0")
  if !exists("c_no_if0_fold")
    syn region	splCppOut		start="^\s*\(%:\|#\)\s*if\s\+0\+\>" end=".\@=\|$" contains=splCppOut2 fold
  else
    syn region	splCppOut		start="^\s*\(%:\|#\)\s*if\s\+0\+\>" end=".\@=\|$" contains=splCppOut2
  endif
  syn region	splCppOut2	contained start="0" end="^\s*\(%:\|#\)\s*\(endif\>\|else\>\|elif\>\)" contains=splSpaceError,splCppSkip
  syn region	splCppSkip	contained start="^\s*\(%:\|#\)\s*\(if\>\|ifdef\>\|ifndef\>\)" skip="\\$" end="^\s*\(%:\|#\)\s*endif\>" contains=splSpaceError,splCppSkip
endif
syn region	splIncluded	display contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	splIncluded	display contained "<[^>]*>"
syn match	splInclude	display "^\s*\(%:\|#\)\s*include\>\s*["<]" contains=splIncluded
"syn match cLineSkip	"\\$"
syn region	splDefine		start="^\s*\(%:\|#\)\s*\(define\|undef\)\>" skip="\\$" end="$" end="//"me=s-1 contains=ALLBUT,@cPreProcGroup,@Spell

" Highlight User Labels
syn cluster	splMultiGroup	contains=splIncluded,splSpecial,splCommentSkip,splCommentString,splComment2String,@cCommentGroup,splCommentStartError,splUserCont,splUserLabel,splBitField,splCppOut,splCppOut2,splCppSkip,splFormat,splNumber,splFloat,splNumbersCom
" Avoid matching foo::bar() in C++ by requiring that the next char is not ':'

syn match	splUserLabel	display "\I\i*" contained

" Avoid recognizing most bitfields as labels
syn match	splBitField	display "^\s*\I\i*\s*:\s*[1-9]"me=e-1 contains=splType
syn match	splBitField	display ";\s*\I\i*\s*:\s*[1-9]"me=e-1 contains=splType

if exists("c_minlines")
  let b:c_minlines = c_minlines
else
  if !exists("c_no_if0")
    let b:c_minlines = 50	" #if 0 constructs can be long
  else
    let b:c_minlines = 15	" mostly for () constructs
  endif
endif
exec "syn sync ccomment cComment minlines=" . b:c_minlines

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link splFormat		splSpecial
hi def link splCommentL		splComment
hi def link splCommentStart	splComment
hi def link splConditional	Conditional
hi def link splRepeat		Repeat
hi def link splCharacter		Character
hi def link splSpecialCharacter	splSpecial
hi def link splNumber		Number
hi def link splFloat		Float
hi def link splParenError		splError
hi def link splErrInParen		splError
hi def link splErrInBracket	splError
hi def link splCommentError	splError
hi def link splCommentStartError	splError
hi def link splSpaceError		splError
hi def link splSpecialError	splError
hi def link splOperator		Operator
hi def link splAnnotation       splStructure	
hi def link splStructure		Structure
hi def link splStorageClass	StorageClass
hi def link splInclude		Include
hi def link splSTOperator		PreProc
hi def link splDefine		Macro
hi def link splIncluded		splString
hi def link splError		Error
hi def link splStatement		Statement
hi def link splPunctMark		Operator
hi def link splType		Type
hi def link splXML              Type
hi def link splConstant		Constant
hi def link splCommentString	splString
hi def link splComment2String	splString
hi def link splCommentSkip	splComment
hi def link splString		String
hi def link splComment		Comment
hi def link splSpecial		SpecialChar
hi def link splTodo		Todo

" vim: ts=8
