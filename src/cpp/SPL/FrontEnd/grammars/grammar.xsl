<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   version="1.0"
   xmlns:xalan="http://xml.apache.org/xslt">

   <xsl:output indent="no" method="text"/>
   <xsl:strip-space elements="*"/>

   <xsl:template match="/">
      <!--xsl:call-template name="header"/-->
      <xsl:apply-templates select="grammar"/>
      <!--xsl:call-template name="trailer"/-->
   </xsl:template>

<xsl:variable name="lcalpha" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="ucalpha" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="grammar">
   <xsl:apply-templates select="production"/>
</xsl:template>

<xsl:template match="production">
<xsl:value-of select="./@name"/>&#09;&#09;//-&gt; <xsl:apply-templates select="child::*" mode="comment"></xsl:apply-templates><xsl:text>
&#09;&#09;&#09;</xsl:text><xsl:apply-templates select="." mode="init"></xsl:apply-templates><xsl:text>
&#09;&#09;&#09;:</xsl:text><xsl:apply-templates select="child::*" mode="rule"></xsl:apply-templates><xsl:text>

</xsl:text>
</xsl:template>

<xsl:template match="production" mode="init">@init { SPL::<xsl:value-of select="translate(substring(@name, 1, 1), $lcalpha, $ucalpha)"/><xsl:value-of select="substring(@name, 2)"/>Context cc(context(ctx), *LT(1)); }</xsl:template>

<xsl:template match="nonTerminal" mode="comment"><xsl:text> </xsl:text><xsl:value-of select="."/><xsl:apply-templates select="@repetition"/></xsl:template>
<xsl:template match="nonTerminal" mode="comment-tail"><xsl:text> | </xsl:text><xsl:value-of select="."/><xsl:apply-templates select="@repetition"/></xsl:template>

<xsl:template match="nonTerminal" mode="rule"><xsl:text> </xsl:text><xsl:value-of select="."/><xsl:apply-templates select="@repetition"/> {cc &lt;&lt; <xsl:value-of select="position()"/>;}</xsl:template>
<xsl:template match="nonTerminal" mode="rule-tail"><xsl:text> | </xsl:text><xsl:value-of select="."/><xsl:apply-templates select="@repetition"/> {cc &lt;&lt; <xsl:value-of select="position()"/>;}</xsl:template>

<xsl:template match="nonTerminal" mode="alternate-rule"><xsl:text> </xsl:text><xsl:value-of select="."/><xsl:apply-templates select="@repetition"/></xsl:template>
<xsl:template match="nonTerminal" mode="alternate-rule-tail"><xsl:text> | </xsl:text><xsl:value-of select="."/><xsl:apply-templates select="@repetition"/></xsl:template>


<xsl:template match="terminal" mode="comment"> '<xsl:value-of select="."/>'</xsl:template>
<xsl:template match="terminal" mode="rule"> '<xsl:value-of select="."/>'</xsl:template>
<xsl:template match="terminal" mode="rule-tail"> '<xsl:value-of select="."/>'</xsl:template>
<xsl:template match="terminal" mode="alternate-rule"> '<xsl:value-of select="."/>'</xsl:template>
<xsl:template match="terminal" mode="alternate-rule-tail"> '<xsl:value-of select="."/>'</xsl:template>

<xsl:template match="specialTerminal" mode="comment"><xsl:text> </xsl:text><xsl:value-of select="."/></xsl:template>
<xsl:template match="specialTerminal" mode="rule"><xsl:text> </xsl:text><xsl:value-of select="."/></xsl:template>

<xsl:template match="alternate" mode="comment"> (<xsl:apply-templates select="child::*[position()=1]" mode="comment"/>
<xsl:apply-templates select="child::*[position()!=1]" mode="comment-tail"/> )<xsl:apply-templates select="@repetition"/> </xsl:template>

<xsl:template match="alternate" mode="rule"> (<xsl:apply-templates select="child::*[position()=1]" mode="alternate-rule"/>
<xsl:apply-templates select="child::*[position()!=1]" mode="alternate-rule-tail"/> )<xsl:apply-templates select="@repetition"/> {cc &lt;&lt; <xsl:value-of select="position()"/>;}</xsl:template>


<xsl:template match="group" mode="comment"> (<xsl:apply-templates select="child::*" mode="comment"/> )<xsl:apply-templates select="@repetition"/> </xsl:template>
<xsl:template match="group" mode="rule"> (<xsl:apply-templates select="child::*" mode="rule"/> )<xsl:apply-templates select="@repetition"/> </xsl:template>



   <xsl:template name="header">
grammar SPL;
options {language = C;}
tokens {
  AMP         = '&amp;';
  AMP_AMP     = '&amp;&amp;';
  AMP_EQ      = '&amp;=';
  AS          = 'as';
  ATTRIBUTE   = 'attribute';
  BANG        = '!';
  BAR         = '|';
  BAR_BAR     = '||';
  BAR_EQ      = '|=';
  BLOB        = 'blob';
  BOOLEAN     = 'boolean';
  BREAK       = 'break';
  COLON       = ':';
  COLON_COLON = '::';
  COMMA       = ',';
  COMPLEX128  = 'complex128';
  COMPLEX32   = 'complex32';
  COMPLEX64   = 'complex64';
  COMPOSITE   = 'composite';
  CONFIG      = 'config';
  CONTINUE    = 'continue';
  DECIMAL128  = 'decimal128';
  DECIMAL32   = 'decimal32';
  DECIMAL64   = 'decimal64';
  DOLLAR      = '$';
  DOT         = '.';
  DOT_AMP     = '.&amp;';
  DOT_BAR     = '.|';
  DOT_GEQ     = '.&gt;=';
  DOT_GREATER = '.&gt;';
  DOT_EQ_EQ   = '.==';
  DOT_HAT     = '.^';
  DOT_LEQ     = '.&lt;=';
  DOT_LESS    = '.&lt;';
  DOT_LSHIFT  = '.&lt;&lt;';
  DOT_MINUS   = '.-';
  DOT_MOD     = '.%';
  DOT_NEQ     = '.!=';
  DOT_PLUS    = '.+';
  DOT_RSHIFT  = '.&gt;&gt;';
  DOT_SLASH   = './';
  DOT_STAR    = '.*';
  DQUOTE      = '"';
  ELSE        = 'else';
  ENUM        = 'enum';
  EQ          = '=';
  EQ_EQ       = '==';
  EXPRESSION  = 'expression';
  FALSE_LITERAL = 'false';
  FLOAT128    = 'float128';
  FLOAT32     = 'float32';
  FLOAT64     = 'float64';
  FOR         = 'for';
  FUNCTION    = 'function';
  GRAPH       = 'graph';
  GREATER     = '&gt;';
  HAT         = '^';
  HAT_EQ      = '^=';
  IF          = 'if';
  IN          = 'in';
  INPUTLIT    = 'input';
  INT16       = 'int16';
  INT32       = 'int32';
  INT64       = 'int64';
  INT8        = 'int8';
  LBRACKET    = '[';
  LCURLY      = '{';
  LEQ         = '&lt;=';
  LESS        = '&lt;';
  LIST        = 'list';
  LOGIC       = 'logic';
  LPAREN      = '(';
  LSHIFT      = '&lt;&lt;';
  LSHIFT_EQ   = '&lt;&lt;=';
  MAP         = 'map';
  MATRIX      = 'matrix';
  MINUS       = '-';
  MINUS_EQ    = '-=';
  MINUS_MINUS = '--';
  MOD         = '%';
  MOD_EQ      = '%=';
  MUTABLE     = 'mutable';
  NAMESPACE   = 'namespace';
  NEQ         = '!=';
  ONPUNCT     = 'onPunct';
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
#if 0
  ONWATERMARK = 'onWatermark';
#endif
  ONTUPLE     = 'onTuple';
  OPERATOR    = 'operator';
  OUTPUT      = 'output';
  PARAM       = 'param';
  PLUS        = '+';
  PLUS_EQ     = '+=';
  PLUS_PLUS   = '++';
  PUBLIC      = 'public';
  QMARK       = '?';
  RBRACKET    = ']';
  RCURLY      = '}';
  RETURN      = 'return';
  RPAREN      = ')';
  SEMI        = ';';
  SET         = 'set';
  SLASH       = '/';
  SLASH_EQ    = '/=';
  SLASH_SLASH = '//';
  SLASH_STAR  = '/*';
  STAR        = '*';
  STAR_EQ     = '*=';
  STAR_SLASH  = '*/';
  STATEFUL    = 'stateful';
  STATIC      = 'static';
  STREAM      = 'stream';
  RSTRING     = 'rstring';
  USTRING    = 'ustring';
  TILDE       = '~';
  TIMESTAMP   = 'timestamp';
  TRUE_LITERAL = 'true';
  TUPLE       = 'tuple';
  TYPE        = 'type';
  UINT16      = 'uint16';
  UINT32      = 'uint32';
  UINT64      = 'uint64';
  UINT8       = 'uint8';
  USE         = 'use';
  VAR         = 'var';
  VOID        = 'void';
  WHILE       = 'while';
  WINDOW      = 'window';
  XML         = 'xml';
  ACTUALCLAUSE; ATTRIBUTEASSIGN; ATTRIBUTEDECL; BLOCKSTMT; BLOCKSTMTNOSCOPE; BOUNDSFORMALS; CALLEXPR; CASTEXPR; COMPILATIONUNIT;
  COMPOSITEBODY; COMPOSITEDEF; COMPOSITEFORMAL; COMPOSITEFORMALINIT; COMPOSITEHEAD;
  COMPOSITEIN; COMPOSITEOUT; COMPOSITEPORT; CONFIGITEM; CURLYLITERAL; DEFTYPE; ENUMTYPE; EXPR; EXPRESSIONMODE; EXPRSTMT;
  FORINIT; FORMALCLAUSE; FUNCTIONDEF; FUNCTIONFORMAL; FUNCTIONHEAD; FUNCTIONPROTOTYPE; GEQ;
  LISTLITERAL; LISTTYPE; LOCALDECL; LOCALDECLITEM; MAPLITERAL; MAPTYPE;
  MODIFIERS; NAMESPACENAME; NONE; OPACTUALEXPRS; OPACTUALNAME; OPACTUALTYPE;
  OPINPUTS; OPINVOKE; OPINVOKEACTUAL; OPINVOKEBODY; OPINVOKEHEAD;
  OPINVOKEOUTPUT; OPINVOKEWINDOW; OPOUTPUT; OPOUTPUTS; OUTPUTCLAUSE; PARAMSIGNATURE;
  PARENTHESIZEDEXPR; PORTINPUTS; POSTFIXEXPR; PREFIXEXPR; PRIMITIVETYPE; PROTOFORMALS;
  RSHIFT; RSHIFT_EQ; SETLITERAL; SETTYPE; SHAREDVARDEF; SHAREDVARCONFIG; SPLDOC; SPLNAMESPACE;
  SLICE; STMT; STATE; SUBSCRIPTEXPR; TUPLEATTRIB; TUPLEEXTEND; TUPLELITERAL; TUPLETYPE; TYPEARGS; TYPECLAUSE; TYPEDIMS;
  TYPEFORMAL; TYPEFORMALS; TYPENAME; UNARY_MINUS; USEDIRECTIVES; USEDIRECTIVETAIL; USE_WILDCARD;
}

@preincludes {
  #include &lt;SPL/FrontEnd/TreeHelper.h&gt;
  #include &lt;SPL/FrontEnd/ParserContexts.h&gt;
  PUSH_DLL_PUBLIC
}

@members {
  static SPL::AstPool &amp; _p() { return SPL::AstPool::instance(); }

  static SPL::ParserContext &amp; context(pSPLParser ctx) {
    void* u = ctx->pParser->rec->state->userp;
    SPL::ParserContext &amp; p = *(SPL::ParserContext*)u;
    return p;
  }

  static void ppush(pSPLParser ctx, int syntaxType) {
    SPL::ParserContext &amp; pCtx = context(ctx);
    pCtx._paraphrase.push_back(syntaxType);
  }

  static void ppop(pSPLParser ctx) {
    SPL::ParserContext &amp; pCtx = context(ctx);
    if (!pCtx._paraphrase.empty())
      pCtx._paraphrase.pop_back();
  }

  static SPL::AstNode * _e() {
    static SPL::AstNode * result = &amp;SPL::AST::errorT::create(_p());
    return result;
  }

  static SPL::ContextBase &amp; currentContext(pSPLParser ctx) {
    SPL::ParserContext &amp; pCtx = context(ctx);
    return pCtx.currentContext();
  }
}

// ----- The compilation unit is the start symbol of the SPL grammar.
</xsl:template>
   <xsl:template name = "trailer">
// ----- Helper productions involving C++ code.
assertNoSpace       [pANTLR3_COMMON_TOKEN t1, pANTLR3_COMMON_TOKEN t2]
                    : { $t1->line == $t2->line &amp;&amp; $t1->getCharPositionInLine($t1) + 1 == $t2->getCharPositionInLine($t2) }? ;

semiKeyword         [char* w]
                    : ID assertEquals[(char*)$ID.text->chars, w];

assertEquals        [char* s, char* t]
                    : { 0 == strcmp(s, t) }?;
   </xsl:template>

</xsl:stylesheet>
