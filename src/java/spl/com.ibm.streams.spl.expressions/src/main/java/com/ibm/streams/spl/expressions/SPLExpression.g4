//
// Copyright 2021 IBM Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

grammar SPLExpression;
 
@header {
}

// antlr4 has a bug where if you don't have an explicit EOF transition for 
// left-recursive rules, it warns; the start rule avoids it
expression: orExpression EOF;

orExpression:      andExpression
            |      andExpression BAR_BAR orExpression;

andExpression:     bitOrExpression 
             |     bitOrExpression AMP_AMP andExpression;

bitOrExpression:   bitXorExpression
             |     bitXorExpression BAR bitOrExpression;

bitXorExpression:  bitAndExpression
             |     bitAndExpression CARET bitXorExpression;

bitAndExpression:  inExpression
             |     inExpression AMP bitAndExpression;


inExpression:      relExpression (IN relExpression)?;
relExpression:     shiftExpression (relOp shiftExpression)?;

shiftExpression:   addExpression 
               |   addExpression shiftOp shiftExpression; 

addExpression:     mulExpression
             |     mulExpression addOp addExpression;

mulExpression:     castExpression
             |     castExpression mulOp mulExpression;

castExpression:    postfixExpression 
              |    '(' type ')' castExpression;

postfixExpression: prefixExpression ('[' addExpression ']')?;

prefixExpression: notExpression
                | bitNotExpression
                | primaryExpression;

notExpression: NOT primaryExpression;
bitNotExpression: BNOT primaryExpression;

primaryExpression: identifier 
                 | intrinsic
                 | functionCall
                 | integer 
                 | floating 
                 | string
                 | bool
                 | '(' orExpression ')';

relOp: LT 
     | LTE 
     | GT 
     | GTE 
     | EQ 
     | NEQ;

shiftOp: LSFT
       | RSFT;

addOp: PLUS 
     | MINUS;

mulOp: MUL 
     | DIV 
     | MOD;

notOp: NOT
     | BNOT;

type: 'boolean' 
    | 'int64' 
    | 'float64'
    | 'rstring';

intrinsic: 'getChannel()' 
         | 'getLocalChannel()'
         | 'getMaxChannels()'
         | 'getLocalMaxChannels()';

// Only unary function for now.
functionCall: function '(' orExpression ')';         

function : 'hash';

identifier: ID;
integer:    INT_LITERAL;
floating:   FLOAT_LITERAL;
string:     STRING_LITERAL;
bool:    BOOLEAN_LITERAL;

AMP:     '&';
CARET:   '^';
BAR:     '|';
AMP_AMP: '&&';
BAR_BAR: '||';
IN:      'in';

LT:  '<';
LTE: '<=';
GT:  '>';
GTE: '>=';
EQ:  '==';
NEQ: '!=';

LSFT: '<<';
RSFT: '>>';

PLUS:  '+';
MINUS: '-';

MUL: '*';
DIV: '/';
MOD: '%';

NOT: '!';
BNOT: '~';

DOLLAR: '$';
DOT:    '.';

DQUOTE: '"';
SQUOTE: '\'';

fragment HEXDIGIT:  DIGIT | 'a'..'f' | 'A'..'F';
fragment LETTER:    'a'..'z' | 'A'..'Z' | '_';
fragment DIGIT:     '0'..'9';
fragment POSDIGIT:  '1'..'9';
fragment EXPONENT:  ('e'|'E') (PLUS | MINUS)? DIGIT+;
fragment STRINGESC: '\\' (DQUOTE|'\\'|'?'|'\''|'a'|'b'|'f'|'n'|'r'|'t'|'v'|'u' HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT | '0');
fragment STRINGCHAR: ~('\\' | '"' | '\'');

ID:     DOLLAR? LETTER (DIGIT | LETTER)*;

INT_LITERAL:    ('0' | MINUS? POSDIGIT DIGIT*);
STRING_LITERAL: (DQUOTE (STRINGESC | STRINGCHAR | SQUOTE)* DQUOTE | SQUOTE (STRINGESC | STRINGCHAR | DQUOTE)* SQUOTE);
FLOAT_LITERAL:  ('0' | POSDIGIT DIGIT*) DOT DIGIT* EXPONENT?
             |  DOT DIGIT+ EXPONENT? 
             |  ('0' | POSDIGIT DIGIT*) EXPONENT;
BOOLEAN_LITERAL: ('true' | 'false');

WS:     [ \t\n\r]+ -> skip;
