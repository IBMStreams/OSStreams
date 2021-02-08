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

grammar SPL;

options {
	language = C;
}

tokens {
  AMP         = '&';
  AMP_AMP     = '&&';
  AMP_EQ      = '&=';
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
  DOT_AMP     = '.&';
  DOT_BAR     = '.|';
  DOT_GEQ     = '.>=';
  DOT_GREATER = '.>';
  DOT_EQ_EQ   = '.==';
  DOT_HAT     = '.^';
  DOT_LEQ     = '.<=';
  DOT_LESS    = '.<';
  DOT_LSHIFT  = '.<<';
  DOT_MINUS   = '.-';
  DOT_MOD     = '.%';
  DOT_NEQ     = '.!=';
  DOT_PLUS    = '.+';
  DOT_RSHIFT  = '.>>';
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
  GREATER     = '>';
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
  LEQ         = '<=';
  LESS        = '<';
  LIST        = 'list';
  LOGIC       = 'logic';
  LPAREN      = '(';
  LSHIFT      = '<<';
  LSHIFT_EQ   = '<<=';
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
  NULL_LITERAL = 'null';
  OPTIONAL    = 'optional';
  ONPUNCT     = 'onPunct';
  ONTUPLE     = 'onTuple';
/*
  // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
  ONWATERMARK = 'onWatermark';
*/
  OPERATOR    = 'operator';
  OUTPUT      = 'output';
  PARAM       = 'param';
  PLUS        = '+';
  PLUS_EQ     = '+=';
  PLUS_PLUS   = '++';
  POUND       = '#';
  PUBLIC      = 'public';
  QMARK       = '?';
  QMARK_COLON = '?:';
  QMARK_QMARK = '??';
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
  SQUOTE      = '\'';
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
  ACTUALCLAUSE; ANNOTATION; ANNOTATIONT; ANNOTATIONS;
  ATTRIBUTEASSIGN; ATTRIBUTEDECL; BLOCKSTMT; BLOCKSTMTNOSCOPE; BOUNDSFORMALS; CALLEXPR; CASTEXPR; COMPILATIONUNIT;
  COMPOSITEBODY; COMPOSITEDEF; COMPOSITEFORMAL; COMPOSITEFORMALINIT; COMPOSITEHEAD;
  COMPOSITEIN; COMPOSITEOUT; COMPOSITEPORT; CONDITIONAL_EXPR; CONFIGITEM; CURLYLITERAL; DEFTYPE; ENUMTYPE; EXPR; EXPRESSIONMODE; EXPRSTMT;
  FORINIT; FORMALCLAUSE; FUNCTIONDEF; FUNCTIONFORMAL; FUNCTIONHEAD; FUNCTIONPROTOTYPE; GEQ; ISPRESENT_EXPR; KEYVALUEPAIR;
  LISTLITERAL; LISTTYPE; LOCALDECL; LOCALDECLITEM; MAPLITERAL; MAPTYPE;
  MODIFIERS; NAMESPACENAME; NONE; ONPROCESS; OPACTUALEXPRS; OPACTUALNAME; OPACTUALTYPE;
  OPINPUTS; OPINVOKE; OPINVOKEACTUAL; OPINVOKEBODY; OPINVOKEHEAD;
  OPINVOKEOUTPUT; OPINVOKEWINDOW; OPOUTPUT; OPOUTPUTS; OPTIONALTYPE; OUTPUTCLAUSE; PARAMSIGNATURE;
  PARENTHESIZEDEXPR; PORTINPUTS; POSTFIXEXPR; PREFIXEXPR; PRIMITIVETYPE; PROTOFORMALS; PROTOFORMAL;
  RSHIFT; RSHIFT_EQ; SETLITERAL; SETTYPE; SPLDOC; SPLNAMESPACE;
  SLICE; STMT; STATE; SUBSCRIPTEXPR; TUPLEATTRIB; TUPLEEXTEND; TUPLELITERAL; TUPLETYPE; TYPEARGS; TYPECLAUSE; TYPEDIMS;
  TYPEFORMAL; TYPEFORMALS; TYPENAME; UNARY_MINUS; UNWRAP_EXPR; UNWRAP_OR_ELSE_EXPR; USEDIRECTIVES; USEDIRECTIVETAIL; USE_WILDCARD; ZZZZ_LAST_TOKEN;
}

@lexer::header {
  #define _empty NULL
} 

@preincludes {
  #include <SPL/FrontEnd/TreeHelper.h>
  #include <SPL/FrontEnd/ParserContexts.h>
  #define NEED_ID(id) strcmp((const char*)LT(1)->getText(LT(1))->chars, #id ) == 0
  PUSH_DLL_PUBLIC
}

@parser::header {
  #define _empty NULL
  
  #ifdef _SPLParser_H
  pANTLR3_UINT8 *getTokenNamesExtern();
  #endif
}

@members {
  static SPL::AstPool & _p() { return SPL::AstPool::instance(); }

  static SPL::ParserContext & context(pSPLParser ctx) {
    void* u = ctx->pParser->rec->state->userp;
    SPL::ParserContext & p = *(SPL::ParserContext*)u;
    return p;
  }

  static void ppush(pSPLParser ctx, int syntaxType) {
    SPL::ParserContext & pCtx = context(ctx);
    pCtx._paraphrase.push_back(syntaxType);
  }

  static void ppop(pSPLParser ctx) {
    SPL::ParserContext & pCtx = context(ctx);
    if (!pCtx._paraphrase.empty())
      pCtx._paraphrase.pop_back();
  }

  static SPL::AstNode * _e() {
    static SPL::AstNode * result = &SPL::AST::errorT::create();
    return result;
  }

  static SPL::ContextBase & currentContext(pSPLParser ctx) {
    SPL::ParserContext & pCtx = context(ctx);
    return pCtx.currentContext();
  }
  
  pANTLR3_UINT8 *getTokenNamesExtern() {
  	return getTokenNames();
  }
}

// ----- The compilation unit is the start symbol of the SPL grammar.
/**
@splProduction compilationUnit : splNamespace? useDirectives (compositeDef | functionDef | standAloneTypeDef)* EOF
@splModel
  <xs:complexType name="compilationUnitType">
    <xs:sequence> 
      <xs:element name="splDoc" type="splDocType" minOccurs="0"/>
      <xs:element name="splNamespace" type="splNamespaceType" minOccurs="0"/>
      <xs:element name="useDirectives" type="useDirectivesType" minOccurs="0"/>
      <xs:element name="definitions" type="definitionsType" minOccurs="0"/>
    </xs:sequence>
  </xs:complexType>
@splModel

There is no grammar production for this code model type.  In the code model the definitions have been grouped by type
@splModel
  <xs:complexType name="definitionsType">
    <xs:sequence>
      <xs:element name="typeDefinition" type="typeDefinitionType" minOccurs="0" maxOccurs="unbounded"/>
      <xs:element name="functionDefinition" type="functionDefinitionType" minOccurs="0" maxOccurs="unbounded"/>
      <xs:element name="compositeDefinition" type="compositeDefinitionType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
compilationUnit     @init { SPL::CompilationUnitContext cc(context(ctx), INPUT, *LT(1), COMPILATIONUNIT); }
                    : {cc << 1;} splNamespace? {cc << 2;} useDirectives {cc << 3;} (compositeDef | functionDef | standAloneTypeDef)* {cc << 4;}  EOF;

/**
@splProduction splNamespace : 'namespace' namespaceName ';'
@splModel
  <xs:complexType name="splNamespaceType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="splDoc" type="splDocType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
      </xs:extension> 
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
splNamespace        @init { SPL::NamespaceContext cc(context(ctx), INPUT, *LT(1)); }
                    : {cc << 1;} NAMESPACE namespaceName SEMI {cc << 2;};

/**
@splProduction namespaceName : identifier ('.' identifier)*
*/
namespaceName       @init { SPL::NamespaceNameContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;} (DOT identifier)* {cc << 3;};

/**
@splProduction useDirectives : useDirective*
@splModel
  <xs:complexType name="useDirectivesType">
    <xs:sequence>
      <xs:element name="useDirective" type="useDirectiveType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
useDirectives       @init { SPL::UseDirectivesContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} useDirective* {cc << 2;};
              

/**
@splProduction useDirective : 'use' namespaceName '::' ( star | identifier ) ';'
@splModel
  <xs:complexType name="useDirectiveType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="namespaceName" use="required" type="xs:string"/>
        <xs:attribute name="tail" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
useDirective        @init { SPL::UseDirectiveContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} USE namespaceName {cc << 2;} COLON_COLON ( star | identifier ) SEMI {cc << 3;};
                    
/**
@splProduction star : '*'
This production has no explicit representation in the splModel
*/
star                @init { SPL::StarContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} STAR {cc << 2;};

/**
@splProduction annotation : '@' identifier annotationTail
This production has no explicit representation in the splModel
*/
annotation          @init { SPL::AnnotationContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} '@' identifier {cc << 2;} (/* empty */ | LPAREN exprOrMapping_pc RPAREN {cc << 3;});

/**
@splProduction annotations : annotation*
This production has no explicit representation in the splModel
*/
annotations         @init { SPL::AnnotationsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} annotation* {cc << 2;};

/**
@splProduction compositeDef : compositeHead compositeBody
@splModel
  <xs:complexType name="compositeDefinitionType">
    <xs:complexContent>
      <xs:extension base="extendedSourceLocationType">
        <xs:sequence>       
          <xs:element name="compositeHead" type="compositeHeadType"/>
          <xs:element name="compositeBody" type="compositeBodyType"/>
        </xs:sequence>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
compositeDef        @init { SPL::CompositeDefContext cc(context(ctx), *LT(1), COMPOSITEDEF); }
                    : {cc << 1;} compositeHead {cc << 2;} compositeBody {cc << 3;};

// ----- Composite operators are defined at the top-level in a namespace.
/**
@splProduction compositeHead : annotations compositeModifiers compositeT identifier compositeInOut
@splModel
  <xs:complexType name="compositeHeadType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence> 
          <xs:element name="splDoc" type="splDocType" minOccurs="0"/>          
          <xs:element name="modifiers" type="compositeModifiersType" minOccurs="0"/>
          <xs:element name="inputs" type="compositeInputsType" minOccurs="0"/> 
          <xs:element name="outputs" type="compositeOutputsType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
compositeHead       @init{ SPL::CompositeHeadContext cc(context(ctx), INPUT, *LT(1), COMPOSITEHEAD); }
                    : {cc << 1;} annotations {cc << 2;} compositeModifiers {cc << 3;} compositeT {cc << 4;} identifier {cc << 5;} compositeInOut {cc << 6;};

/**
@splProduction  compositeModifiers : publicT?
@splModel
  <xs:complexType name="compositeModifiersType">
    <xs:sequence>       
      <xs:element name="modifier" type="compositeModifierType" minOccurs="1" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
  
  <xs:complexType name="compositeModifierType">
    <xs:attribute name="name" use="required" type="compositeModifierEnumType"/>
  </xs:complexType>
  
  <xs:simpleType name="compositeModifierEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="public"/>
    </xs:restriction>
  </xs:simpleType>
@splModel
*/
compositeModifiers  @init{ SPL::CompositeModifiersContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} publicT? {cc << 2;};

/**
@splProduction publicT : 'public'
This production has no explicit representation in the splModel
*/
publicT             @init{ SPL::PublicContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} PUBLIC {cc << 2;};

/**
@splProduction compositeInOut : ( | '(' ')' | '(' compositeOut ( ';' compositeIn  ')' | ')' ) | '(' compositeIn ( ';' compositeOut ')' | ')' ) )
This production has no explicit representation in the splModel
*/
compositeInOut      @init { SPL::CompositeInOutContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (/*empty*/
                                 | LPAREN RPAREN
                                 | LPAREN compositeOut ( SEMI {cc << 2;} compositeIn RPAREN | RPAREN)
                                 | LPAREN {cc << 2;} compositeIn ( SEMI {cc << 1;} compositeOut RPAREN | RPAREN)) {cc << 3;};
                                           
/**
@splProduction compositeOut : 'output' compositePort_pc
@splModel
  <xs:complexType name="compositeOutputsType">
    <xs:sequence>
      <xs:element name="oport" type="compositePortType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
compositeOut        @init { SPL::CompositeOutContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} OUTPUT compositePort_pc {cc << 2;};

/**
@splProduction compositeIn : 'input' compositePort_pc
@splModel
  <xs:complexType name="compositeInputsType">
    <xs:sequence>
      <xs:element name="iport" type="compositePortType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
compositeIn         @init { SPL::CompositeInContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} INPUTLIT compositePort_pc {cc << 2;};

/**
@splProduction compositePort_pc : compositePort (',' compositePort )*
This production has no explicit representation in the splModel
*/
compositePort_pc    : compositePort (COMMA compositePort)*;

/**
@splProduction compositePort : identifier | streamType identifier
@splModel
  <xs:complexType name="compositePortType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="name" use="required" type="xs:string"/>
        <xs:attribute name="index" use="required" type="xs:unsignedLong"/>
        <xs:attribute name="type" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
compositePort       @init { SPL::CompositePortContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( {cc << 2;} identifier | streamType {cc << 2;} identifier) {cc << 3;};

/**
@splProduction compositeBody : '{' compositeBodyClause* '}'
@splModel
  <xs:complexType name="compositeBodyType">
    <xs:sequence>    
      <xs:element name="types" type="compositeTypesType" minOccurs="0"/>
      <xs:element name="parameters" type="compositeParametersType" minOccurs="0"/>
      <xs:element name="graph" type="compositeGraphType" minOccurs="0"/>
      <xs:element name="configs" type="configsType" minOccurs="0"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
compositeBody       @init { SPL::CompositeBodyContext cc(context(ctx), *LT(1), COMPOSITEBODY); }
                    : {cc << 1;} LCURLY compositeBodyClause* {cc << 2;} rCurly {cc << 3;};

/**
@splProduction formalClause : 'param' compositeFormal+
@splModel
  <xs:complexType name="compositeParametersType">
    <xs:sequence>
      <xs:element name="parameter" type="compositeParameterType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
formalClause        @init { SPL::FormalClauseContext cc(context(ctx), *LT(1), FORMALCLAUSE); }
                    : {cc << 1;} PARAM compositeFormal+ {cc << 2;};

/**
@splProduction compositeFormal : expressionMode identifier compositeFormalInit
@splModel
  <xs:complexType name="compositeParameterType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="expressionMode" type="compositeParameterExpressionModeType"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
        <xs:attribute name="defaultValue" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
compositeFormal     @init { SPL::CompositeFormalContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} expressionMode {cc << 2;} identifier {cc << 3;} compositeFormalInit {cc << 4;};

/**
@splProduction rCurly : '}'
This production has no explicit representation in the splModel
*/
rCurly              @init { SPL::RCurlyContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} RCURLY {cc << 2;};

/**
@splProduction semi : ';'
This production has no explicit representation in the splModel
*/
semi               @init { SPL::SemiContext cc(context(ctx), *LT(1)); }
                   : {cc << 1;} SEMI {cc << 2;};

/**
@splProduction attributeT : 'attribute'
This production has no explicit representation in the splModel
*/
attributeT          @init { SPL::AttributeTContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ATTRIBUTE {cc << 2;};

/**
@splProduction expressionT : 'expression'
This production has no explicit representation in the splModel
*/
expressionT         @init { SPL::ExpressionTContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} EXPRESSION {cc << 2;};

/**
@splProduction functionT : 'function'
This production has no explicit representation in the splModel
*/
functionT           @init { SPL::FunctionTContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} FUNCTION {cc << 2;};

/**
@splProduction operatorT : 'operator'
This production has no explicit representation in the splModel
*/
operatorT           @init { SPL::OperatorTContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} OPERATOR {cc << 2;};

/**
@splProduction typeT : 'type'
This production has no explicit representation in the splModel
*/
typeT               @init { SPL::TypeTContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} TYPE {cc << 2;};

/**
@splProduction actualClause : 'param' opInvokeActual+
@splModel
  <xs:complexType name="operatorInvocationParametersType">
    <xs:sequence>
      <xs:element name="parameter" type="operatorInvocationParameterType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
actualClause        @init{ SPL::ActualClauseContext cc(context(ctx), *LT(1), OPINVOKEACTUAL); }
                    : {cc << 1;} PARAM opInvokeActual+ {cc << 2;};

/**
@splProduction compositeBodyClause : formalClause | typeClause | graphClause | configClause
*/
compositeBodyClause @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( formalClause | typeClause | graphClause | configClause) {cc << 2;}; //order and uniqueness enforced elsewhere

/**
@splProduction graphClause : 'graph' opInvoke+
@splModel
  <xs:complexType name="compositeGraphType">
    <xs:sequence>
      <xs:element name="operatorInvocation" type="operatorInvocationType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
graphClause         @init { SPL::GraphClauseContext cc(context(ctx), *LT(1), GRAPH); }
                    : {cc << 1;} GRAPH opInvoke+ {cc << 2;};

/**
@splProduction opInvokeBody : '{' opInvokeBodyClause* '}'
@splModel
  <xs:complexType name="operatorInvocationBodyType">
    <xs:sequence>
      <xs:element name="logic" type="operatorInvocationLogicType" minOccurs="0"/>
      <xs:element name="windows" type="operatorInvocationWindowsType" minOccurs="0"/>
      <xs:element name="parameters" type="operatorInvocationParametersType" minOccurs="0"/>
      <xs:element name="outputAssignments" type="operatorInvocationOutputAssignmentsType" minOccurs="0"/>
      <xs:element name="configs" type="configsType" minOccurs="0"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
opInvokeBody        @init { SPL::OpInvokeBodyContext cc(context(ctx), *LT(1), OPINVOKEBODY); }
                    : {cc << 1;} LCURLY opInvokeBodyClause* {cc << 2;} rCurly {cc << 3;};

/**
@splProduction opInvokeBodyClause : (logicClause | windowClause | actualClause | outputClause | configClause )
This production has no explicit representation in the splModel
*/
opInvokeBodyClause  @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( logicClause | windowClause | actualClause | outputClause | configClause) {cc << 2;}; //order and uniqueness enforced elsewhere

/**
@splProduction typeClause : 'type' typeDef+
@splModel
  <xs:complexType name="compositeTypesType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>    
          <xs:element name="type" type="typeDefinitionType" minOccurs="1" maxOccurs="unbounded"/>
        </xs:sequence>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
typeClause          @init { SPL::TypeClauseContext cc(context(ctx), *LT(1), TYPECLAUSE); }
                    : {cc << 1;} TYPE typeDef+ {cc << 2;};

/**
@splProduction windowClause : 'window' opInvokeWindow+
@splModel
  <xs:complexType name="operatorInvocationWindowsType">
    <xs:sequence>
      <xs:element name="window" type="operatorInvocationWindowType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
windowClause        @init { SPL::WindowClauseContext cc(context(ctx), *LT(1), OPINVOKEWINDOW); }
                    : {cc << 1;} WINDOW opInvokeWindow+ {cc << 2;};

/**
@splProduction configClause : 'config' config+
@splModel
  <xs:complexType name="configsType">
    <xs:sequence>
      <xs:element name="config" type="configType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
configClause        @init { SPL::ConfigClauseContext cc(context(ctx), *LT(1), CONFIG); }
                    : {cc << 1;} CONFIG config+ {cc << 2;};

/**
@splProduction opInvokeWindow : identifier ':' expr_pc ';'
@splModel
  <xs:complexType name="operatorInvocationWindowType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="evictionPolicy" type="windowPolicyType"/>
          <xs:element name="triggerPolicy" type="windowPolicyType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="windowType" use="required" type="windowTypeEnumType"/>
        <xs:attribute name="partitioned" use="required" type="xs:boolean"/>
        <xs:attribute name="portName" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>

  <xs:complexType name="windowPolicyType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="kind" use="required" type="windowPolicyKindEnumType"/>
        <xs:attribute name="size" type="xs:string"/>
        <xs:attribute name="attribute" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>

  <xs:simpleType name="windowPolicyKindEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="count"/>
      <xs:enumeration value="time"/>
      <xs:enumeration value="delta"/>
      <xs:enumeration value="punct"/>
    </xs:restriction>
  </xs:simpleType>

  <xs:simpleType name="windowTypeEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="tumbling"/>
      <xs:enumeration value="sliding"/>
    </xs:restriction>
  </xs:simpleType>
@splModel
*/
opInvokeWindow      @init { SPL::OpInvokeWindowContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;} COLON expr_pc SEMI {cc << 3;};

/**
@splProduction config : identifier ':' expr_pc ';'
@splModel
  <xs:complexType name="configType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="option" type="configOptionType" minOccurs="1" maxOccurs="unbounded"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>

  <xs:complexType name="configOptionType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="parameter" type="configValueParameterType" minOccurs="0" maxOccurs="unbounded"/>
        </xs:sequence>
        <xs:attribute name="value" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>

  <xs:complexType name="configValueParameterType">
    <xs:attribute name="value" use="required" type="xs:string"/>
  </xs:complexType>
@splModel
*/
config             @init { SPL::ConfigContext cc(context(ctx), *LT(1)); }
                   : {cc << 1;} identifier {cc << 2;} COLON expr_pc SEMI {cc << 3;};

/**
@splProduction opActual : ( (opActualName)=> opActualName | (opActualType)=> opActualType  | opActualExprs )
This production has no explicit representation in the splModel
*/
opActual            @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( (opActualName)=> opActualName | (opActualType)=> opActualType | opActualExprs) {cc << 2;};

/**
@splProduction compositeFormalInit : (';' | ':' opActual)
This production has no explicit representation in the splModel
*/
compositeFormalInit @init { SPL::CompositeFormalInitContext cc(context(ctx), *LT(1)); }
                   : {cc << 1;} ( SEMI | COLON opActual) {cc << 2;};

// ----- Streams are defined in a composite operator's graph clause.
/**
@splProduction opInvoke : opInvokeHead opInvokeBody
@splModel
  <xs:complexType name="operatorInvocationType">
    <xs:sequence>
      <xs:element name="operatorInvocationHead" type="operatorInvocationHeadType"/>
      <xs:element name="operatorInvocationBody" type="operatorInvocationBodyType"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
opInvoke            @init { SPL::OpInvokeContext cc(context(ctx), *LT(1), OPINVOKE); }
                    : {cc << 1;} opInvokeHead {cc << 2;} opInvokeBody {cc << 3;};

/**
@splProduction opInvokeHead : annotations ( '(' ')' asAlias | opOutput | opOutputs asAlias? ) '=' opName opInputs
@splModel
  <xs:complexType name="operatorInvocationHeadType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="splDoc" type="splDocType" minOccurs="0"/>
          <xs:element name="outputs" type="operatorInvocationOutputsType" minOccurs="0"/>
          <xs:element name="inputs" type="operatorInvocationInputsType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="operatorName" use="required" type="xs:string"/>
        <xs:attribute name="invocationAlias" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
opInvokeHead        @init { SPL::OpInvokeHeadContext cc(context(ctx), INPUT, *LT(1), OPINVOKEHEAD); }
                    : {cc << 1;} annotations {cc << 2;} ( LPAREN RPAREN asAlias | {cc << 3;} opOutput | {cc << 4;} opOutputs {cc << 5;} asAlias?) {cc << 6;} EQ opName {cc << 7;} opInputs {cc << 8;};

/**
@splProduction opActualExprs : expr_pc ';'
This production has no explicit representation in the splModel
*/
opActualExprs       @init { SPL::OpActualExprsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} expr_pc SEMI {cc << 2;};

/**
@splProduction opOutputs : '(' opOutput_ps ')'
@splModel
  <xs:complexType name="operatorInvocationOutputsType">
    <xs:sequence>
      <xs:element name="output" type="operatorInvocationOutputType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
opOutputs           @init { SPL::OpOutputsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LPAREN opOutput_ps RPAREN {cc << 2;};

/**
@splProduction asAlias : 'as' identifier
This production has no explicit representation in the splModel
*/
asAlias             @init { SPL::AsAliasContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} AS identifier {cc << 2;};

/**
@splProduction opName : ( (qualifiedName)=> qualifiedName | identifier)
This production has no explicit representation in the splModel
*/
opName              @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ((qualifiedName)=> qualifiedName | identifier) {cc << 2;};

/**
@splProduction qualifiedName : namespaceName '::' identifier
This production has no explicit representation in the splModel
*/
qualifiedName       @init{ SPL::QualifiedNameContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} namespaceName {cc << 2;} COLON_COLON identifier {cc << 3;};

/**
@splProduction opInputs '(' portInputs_ps? ')'
@splModel
  <xs:complexType name="operatorInvocationInputsType">
    <xs:sequence>
      <xs:element name="input" type="operatorInvocationInputType" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
opInputs            @init{ SPL::OpInputsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LPAREN portInputs_ps? RPAREN {cc << 2;};

/**
@splProduction opInvokeActual : identifier ':' opActual
@splModel
  <xs:complexType name="operatorInvocationParameterType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="value" type="expressionType" minOccurs="1" maxOccurs="unbounded"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>

  <xs:complexType name="expressionType">
    <xs:attribute name="expr" use="required" type="xs:string"/>
  </xs:complexType>
@splModel
*/
opInvokeActual      @init { SPL::OpInvokeActualContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;} COLON opActual {cc << 3;};

/**
@splProduction outputClause : 'output' opInvokeOutput+
@splModel
  <xs:complexType name="operatorInvocationOutputAssignmentsType">
    <xs:sequence>
      <xs:element name="outputAssignment" type="operatorInvocationOutputAssignmentType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
outputClause        @init { SPL::OutputClauseContext cc(context(ctx), *LT(1), OPINVOKEOUTPUT); }
                    : {cc << 1;} OUTPUT opInvokeOutput+ {cc << 2;};

/**
@splProduction opInvokeOutput : identifier ':' outputAssign_pc ';'
@splModel
  <xs:complexType name="operatorInvocationOutputAssignmentType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="attributeAssignment" type="operatorInvocationAttributeAssignmentType" minOccurs="1" maxOccurs="unbounded"/>
        </xs:sequence>
        <xs:attribute name="portName" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
opInvokeOutput      @init { SPL::OpInvokeOutputContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;} COLON outputAssign_pc SEMI {cc << 3;};

/*
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
add onWatermarkType like onPunctType to opInvokeLogic below
*/
/**
@splProduction opInvokeLogic : (opInvokeState | opInvokeOnTuple | opInvokeOnPunct | opInvokeOnProcess)
@splModel
  <xs:complexType name="operatorInvocationLogicType">
    <xs:sequence>
      <xs:element name="onProcess" type="onProcessType" minOccurs="0" maxOccurs="1"/>
      <xs:element name="onTuple" type="onTupleType" minOccurs="0" maxOccurs="unbounded"/>
      <xs:element name="onPunct" type="onPunctType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
    <xs:attribute name="hasState" use="required" type="xs:boolean"/>
  </xs:complexType>
@splModel
*/
opInvokeLogic       @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ({NEED_ID(state)}? opInvokeState | opInvokeOnTuple | opInvokeOnPunct | {NEED_ID(onProcess)}? opInvokeOnProcess) {cc << 2;} ;

/**
@splProduction logicClause : 'logic' opInvokeLogic+
*/
logicClause         @init { SPL::LogicClauseContext cc(context(ctx), *LT(1), LOGIC); }
                    : {cc << 1;} LOGIC opInvokeLogic+ {cc << 2;};

/**
@splProduction functionModifiers : functionModifier*
@splModel
  <xs:complexType name="functionModifiersType">
    <xs:sequence>       
      <xs:element name="modifier" type="functionModifierType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
functionModifiers   @init { SPL::FunctionModifiersContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} functionModifier* {cc << 2;};

/**
@splProduction functionModifier : (publicT | statefulT)
@splModel
  <xs:complexType name="functionModifierType">
    <xs:attribute name="name" use="required" type="functionModifierEnumType"/>
  </xs:complexType>

  <xs:simpleType name="functionModifierEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="public"/>
      <xs:enumeration value="stateful"/>
    </xs:restriction>
  </xs:simpleType>
@splModel
*/
functionModifier    @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;}  (publicT | statefulT) {cc << 2;} ;

/**
@splProduction statefulT : 'stateful'
This production has no explicit representation in the splModel
*/
statefulT           @init { SPL::StatefulContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} STATEFUL {cc << 2;};

/**
@splProduction typeFormals : ( | '<' typeFormal_pc '>')
This production has no explicit representation in the splModel
*/
typeFormals         @init { SPL::TypeFormalsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (/*empty*/ | LESS typeFormal_pc GREATER) {cc << 2;};

/**
@splProduction boundsFormals : ( | '[' id_pc ']')
This production has no explicit representation in the splModel
*/
boundsFormals       @init { SPL::BoundsFormalsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (/*empty*/ | LBRACKET id_pc RBRACKET) {cc << 2;};

/**
@splProduction protoFormals : '(' protoFormal_pc? ')'
This production has no explicit representation in the splModel
*/
protoFormals        @init { SPL::ProtoFormalsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LPAREN protoFormal_pc? RPAREN {cc << 2;};

/**
@splProduction functionFormal_pc : functionFormal (',' functionFormal )*
@splModel
  <xs:complexType name="functionParametersType">
    <xs:sequence>
      <xs:element name="parameter" type="functionParameterType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
functionFormal_pc   : functionFormal (COMMA functionFormal)*;

/**
@splProduction functionHead : functionModifiers returnType identifier '(' functionFormal_pc? ')'
@splModel
  <xs:complexType name="functionHeadType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="splDoc" type="splDocType" minOccurs="0"/>
          <xs:element name="modifiers" type="functionModifiersType" minOccurs="0"/>
          <xs:element name="parameters" type="functionParametersType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
        <xs:attribute name="returnType" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
functionHead        @init { SPL::FunctionHeadContext cc(context(ctx), INPUT, *LT(1)); }
                    : {cc << 1;} functionModifiers {cc << 2;} returnType {cc << 3;} identifier {cc << 4;} LPAREN functionFormal_pc? RPAREN {cc << 5;};

/**
@splProduction voidT : 'void'
This production has no explicit representation in the splModel
*/
voidT               @init { SPL::VoidContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} VOID {cc << 2;};

/**
@splProduction opOutput : streamType identifier asAlias?
@splModel
  <xs:complexType name="operatorInvocationOutputType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="streamName" use="required" type="xs:string"/>
        <xs:attribute name="index" use="required" type="xs:unsignedLong"/>
        <xs:attribute name="type" use="required" type="xs:string"/>
        <xs:attribute name="alias" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
opOutput            @init { SPL::OpOutputContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} streamType {cc << 2;} identifier {cc << 3;} asAlias? {cc << 4;};

/**
@splProduction portInputs : streamType? id_pc asAlias?
@splModel
  <xs:complexType name="operatorInvocationInputType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="istream" type="operatorInvocationInputStreamType" minOccurs="1" maxOccurs="unbounded"/>
        </xs:sequence>
        <xs:attribute name="index" use="required" type="xs:unsignedLong"/>
        <xs:attribute name="alias" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
portInputs          @init{ SPL::PortInputsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} streamType? {cc << 2;} id_pc {cc << 3;} asAlias? {cc << 4;};

keySTATE            returns[SPL::AstNode* r = NULL] 
                    @init{ SPL::KeySTATEContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;};

/**
@splProduction opInvokeState : keySTATE ':' ( localDecl | localDecls )
This production has no explicit representation in the splModel
*/
opInvokeState       @init{ SPL::OpInvokeStateContext cc(context(ctx), *LT(1), STATE); }
                    : {cc << 1;} keySTATE COLON {cc << 2;} (localDecl | localDecls) {cc << 3;};

keyONPROCESS        returns[SPL::AstNode* r = NULL] 
                    @init{ SPL::KeyONPROCESSContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;};

/**
@splProduction opInvokeOnProcess : keyOnProcess ':' stmt
@splModel
  <xs:complexType name="onProcessType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType"/>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
opInvokeOnProcess   @init{ SPL::OpInvokeOnProcessContext cc(context(ctx), *LT(1), ONPROCESS); }
                    : {cc << 1;} keyONPROCESS {cc << 2;} COLON stmt {cc << 3;};

/**
@splProduction stmtOther : (exprStmt | ifStmt | forStmt | whileStmt | breakStmt | continueStmt | returnStmt)
This production has no explicit representation in the splModel
*/
stmtOther           @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (exprStmt | ifStmt | forStmt | whileStmt | breakStmt | continueStmt | returnStmt) {cc << 2;} ;

/**
@splProduction streamType : 'stream' tupleArgs
@splModel
  <xs:complexType name="operatorInvocationInputStreamType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="name" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
streamType          @init { SPL::StreamTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} STREAM tupleArgs {cc << 2;};

/**
@splProduction opActualType : type ';'
This production has no explicit representation in the splModel
*/
opActualType        @init { SPL::OpActualTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} type SEMI {cc << 2;};

/**
@splProduction opActualName : typeNameContents ';'
This production has no explicit representation in the splModel
*/
opActualName        @init { SPL::OpActualNameContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} typeNameContents SEMI {cc << 2;};

/**
@splProduction opInvokeOnTuple : 'onTuple' identifier ':' stmt
@splModel
  <xs:complexType name="onTupleType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="portName" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
opInvokeOnTuple     @init{ SPL::OpInvokeOnTuple cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ONTUPLE identifier {cc << 2;} COLON stmt {cc << 3;};

/**
@splProduction opInvokeOnPunct : 'onPunct' identifier ':' stmt
@splModel
  <xs:complexType name="onPunctType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:attribute name="portName" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
opInvokeOnPunct     @init{ SPL::OpInvokeOnPunct cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ONPUNCT identifier {cc << 2;} COLON stmt {cc << 3;};

/*
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
add opInvokeOnWatermark like opInvokeOnPunct
*/

/**
@splProduction localDecls : '{' localDecl+ '}'
This production has no explicit representation in the splModel
*/
localDecls          @init{ SPL::LocalDeclsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LCURLY localDecl+ RCURLY {cc << 2;};

/**
@splProduction outputAssign : identifier opEqual expr
@splModel
  <xs:complexType name="operatorInvocationAttributeAssignmentType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="value" type="expressionType" minOccurs="1" maxOccurs="unbounded"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
        <xs:attribute name="outputFunction" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
outputAssign        @init { SPL::OutputAssignContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;} opEqual {cc << 3;} expr {cc << 4;};

/**
@splProduction opEqual : '='
This production has no explicit representation in the splModel
*/
opEqual             @init { SPL::OpEqualContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} EQ {cc << 2;};

// ----- Functions are defined at the top-level in a namespace.
/**
@splProduction functionDef : functionHead blockStmtNoScope
@splModel
  <xs:complexType name="functionDefinitionType">
    <xs:complexContent>
      <xs:extension base="extendedSourceLocationType">           
        <xs:sequence>
          <xs:element name="functionHead" type="functionHeadType"/>
          <xs:element name="functionBody" type="xs:string" minOccurs="0"/>
        </xs:sequence>
      </xs:extension>  
    </xs:complexContent>
  </xs:complexType> 
@splModel
*/
functionDef         @init { SPL::FunctionDefContext cc(context(ctx), *LT(1), FUNCTIONDEF); }
                    : {cc << 1;} functionHead {cc << 2;} blockStmtNoScope {cc << 3;};

/**
@splProduction functionFormal : formalModifiers type identifier
@splModel
  <xs:complexType name="functionParameterType">
    <xs:complexContent>
      <xs:extension base="sourceLocationType">
        <xs:sequence>
          <xs:element name="modifiers" type="functionParameterModifiersType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="type" use="required" type="xs:string"/>
        <xs:attribute name="name" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
functionFormal      @init { SPL::FunctionFormalContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} formalModifiers {cc << 2;} type {cc << 3;} identifier {cc << 4;};

/**
@splProduction formalModifiers : formalModifier?
@splModel
  <xs:complexType name="functionParameterModifiersType">
    <xs:sequence>
      <xs:element name="modifier" type="functionParameterModifierType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
formalModifiers     @init { SPL::FormalModifiersContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} formalModifier? {cc << 2;};

/**
@splProduction formalModifier mutableT
@splModel
  <xs:complexType name="functionParameterModifierType">
    <xs:attribute name="name" use="required" type="functionParameterModifierEnumType"/>
  </xs:complexType>
  
  <xs:simpleType name="functionParameterModifierEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="mutable"/>
    </xs:restriction> 
  </xs:simpleType>
@splModel
*/
formalModifier      @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} mutableT {cc << 2;};

/**
@splProduction mutableT : 'mutable'
This production has no explicit representation in the splModel
*/
mutableT            @init { SPL::MutableContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} MUTABLE {cc << 2;};

/**
@splProduction eqExpr_q : ( | '=' expr )
This production has no explicit representation in the splModel
*/
eqExpr_q            @init{ SPL::EqExprQContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (/*empty*/ | EQ expr {cc << 2;});

// ----- Imperative statements can appear in function bodies or the logic clause of operator invocations.
/**
@splProduction stmtOrType : (stmt | standAloneTypeDef)
This production has no explicit representation in the splModel
*/
stmtOrType          @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (stmt | standAloneTypeDef) {cc << 2;};

/**
@splProduction stmt : ( (localModifiers type identifier)=> localDecl | ('{')=> blockStmt | stmtOther)
This production has no explicit representation in the splModel
*/
stmt                @init{ SPL::StmtContext cc(context(ctx), *LT(1), STMT); }
                    : {cc << 1;} ( (localModifiers type identifier)=> localDecl | (LCURLY)=> blockStmt | stmtOther) {cc << 2; };

/**
@splProduction stmtNoScope : ( (localModifiers type identifier)=> localDecl | ('{')=> blockStmtNoScope | stmtOther )
This production has no explicit representation in the splModel
*/
stmtNoScope         @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( (localModifiers type identifier)=> localDecl | (LCURLY)=> blockStmtNoScope | stmtOther) {cc << 2;};

/**
@splProduction localDecl : localModifiers type localDeclItem_pc ';'
This production has no explicit representation in the splModel
*/
localDecl           @init{ SPL::LocalDeclContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} localModifiers {cc << 2;} type {cc << 3;} localDeclItem_pc SEMI {cc << 4;};

/**
@splProduction localModifiers : localModifier?
This production has no explicit representation in the splModel
*/
localModifiers      @init{ SPL::LocalModifiersContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} localModifier? {cc << 2;};

/**
@splProduction localModifier : mutableT
This production has no explicit representation in the splModel
*/
localModifier       @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} mutableT {cc << 2;} ;

/**
@splProduction localDeclItem : identifier eqExpr_q
This production has no explicit representation in the splModel
*/
localDeclItem       @init{ SPL::LocalDeclItemContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} identifier {cc << 2;} eqExpr_q {cc << 3;};

/**
@splProduction blockStmt : '{' stmtOrType* '}'
This production has no explicit representation in the splModel
*/
blockStmt           @init{ SPL::BlockStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LCURLY stmtOrType* RCURLY {cc << 2;};

/**
@splProduction blockStmtNoScope : '{' stmtOrType* '}'
This production has no explicit representation in the splModel
*/
blockStmtNoScope    @init{ SPL::BlockStmtNoScopeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LCURLY stmtOrType* RCURLY {cc << 2;};

/**
@splProduction exprStmt : expr ';'
This production has no explicit representation in the splModel
*/
exprStmt            @init{ SPL::ExprStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} expr {cc << 2;} SEMI {cc << 3;};

/**
@splProduction ifStmt : 'if' '(' expr ')' stmt ( ('else')=> elseClause | )
This production has no explicit representation in the splModel
*/
ifStmt              @init{ SPL::IfStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1; } IF LPAREN expr {cc << 2; } RPAREN stmt {cc << 3; } ((ELSE)=> elseClause | /*empty*/) {cc << 4;};

/**
@splProduction elseClause : 'else' stmt
This production has no explicit representation in the splModel
*/
elseClause          @init{ SPL::ElseClauseContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ELSE stmt {cc << 2;};

/**
@splProduction forStmt : 'for' forInit stmtNoScope
This production has no explicit representation in the splModel
*/
forStmt             @init{ SPL::ForStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} FOR forInit {cc << 2;} stmtNoScope {cc << 3;};

/**
@splProduction forInit : '(' type identifier 'in' expr ')'
This production has no explicit representation in the splModel
*/
forInit             @init{ SPL::ForInitContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LPAREN type {cc << 2;} identifier {cc << 3;} IN expr {cc << 4;} RPAREN {cc << 5;};

/**
@splProduction whileStmt : 'while' '(' expr ')' stmt
This production has no explicit representation in the splModel
*/
whileStmt           @init{ SPL::WhileStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} WHILE LPAREN expr {cc << 2;} RPAREN stmt {cc << 3;};

/**
@splProduction breakStmt : 'break' ';'
This production has no explicit representation in the splModel
*/
breakStmt           @init{ SPL::BreakStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} BREAK SEMI {cc << 2;};

/**
@splProduction continueStmt : 'continue' ';'
This production has no explicit representation in the splModel
*/
continueStmt        @init{ SPL::ContinueStmtContext cc(context(ctx), *LT(1)); }
                    :  {cc << 1;}CONTINUE SEMI {cc << 2;};

/**
@splProduction returnStmt : 'return' ( ';' | expr ';')
This production has no explicit representation in the splModel
*/
returnStmt          @init{ SPL::ReturnStmtContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} RETURN (SEMI | expr {cc << 2;} SEMI) {cc << 3;};

// ----- Expressions can appear in many places in the grammar.
/**
@splProduction exprOrMapping : expr (':' expr)?
This production has no explicit representation in the splModel
*/
exprOrMapping       @init{ SPL::ExprOrMappingContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} expr {cc << 2;} (COLON expr {cc << 3;})?;

/**
@splProduction expr : assignExpr
This production has no explicit representation in the splModel
*/
expr                @init{ SPL::ExprContext cc(context(ctx), *LT(1), EXPRESSION); }
                    : {cc << 1;} assignExpr {cc << 2;};

/**
@splProduction assignExpr : condExpr (assignOp  condExpr)*
This production has no explicit representation in the splModel
*/
assignExpr          @init{ SPL::AssignExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} condExpr {cc << 2;} ({cc << 3;} assignOp {cc << 4;} condExpr)* {cc << 5;};

/**
@splProduction condExpr : logicOrExpr ('?' condExpr ':' condExpr)?
This production has no explicit representation in the splModel
*/
condExpr            @init{ SPL::CondExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} logicOrExpr {cc << 2;} (QMARK {cc << 3;} condExpr COLON {cc << 4;} condExpr)? {cc << 5;};

/**
@splProduction logicOrExpr : logicAndExpr (barBar logicAndExpr)*
This production has no explicit representation in the splModel
*/
logicOrExpr         @init{ SPL::LogicOrExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;}  logicAndExpr {cc << 2;} ({cc << 3;} barBar {cc << 4;} logicAndExpr)* {cc << 5;};

/**
@splProduction barBar : '||'
This production has no explicit representation in the splModel
*/
barBar              @init{ SPL::BarBarContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} BAR_BAR {cc << 2;};

/**
@splProduction logicAndExpr : memberExpr (ampAmp memberExpr)*
This production has no explicit representation in the splModel
*/
logicAndExpr        @init{ SPL::LogicAndExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} memberExpr {cc << 2;} ({cc << 3;} ampAmp {cc << 4;} memberExpr)* {cc << 5;};

/**
@splProduction ampAmp : '&&'
This production has no explicit representation in the splModel
*/
ampAmp              @init{ SPL::AmpAmpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} AMP_AMP {cc << 2;};

/**
@splProduction memberExpr : bitOrExpr ('in' bitOrExpr)*
This production has no explicit representation in the splModel
*/
memberExpr          @init{ SPL::MemberExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} bitOrExpr {cc << 2;} ({cc << 3;} in {cc << 4;} bitOrExpr)* {cc << 5;};

/**
@splProduction in : 'in'
This production has no explicit representation in the splModel
*/
in                  @init{ SPL::InContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} IN {cc << 2;};

/**
@splProduction bitOrExpr : bitXorExpr (bitOrOp bitXorExpr)*
This production has no explicit representation in the splModel
*/
bitOrExpr           @init{ SPL::BitOrExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} bitXorExpr {cc << 2;} ({cc << 3;} bitOrOp {cc << 4;} bitXorExpr)* {cc << 5;};

/**
@splProduction bitXorExpr : bitAndExpr (bitXorOp bitAndExpr)*
This production has no explicit representation in the splModel
*/
bitXorExpr          @init{ SPL::BitXorExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} bitAndExpr {cc << 2;} ({cc << 3;} bitXorOp {cc << 4;} bitAndExpr)* {cc << 5;};

/**
@splProduction bitAndExpr : compareExpr (bitAndOp compareExpr)*
This production has no explicit representation in the splModel
*/
bitAndExpr          @init{ SPL::BitAndExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} compareExpr {cc << 2;} ({cc << 3;} bitAndOp {cc << 4;} compareExpr)* {cc << 5;};

/**
@splProduction compareExpr : shiftExpr (compareOp shiftExpr)*
This production has no explicit representation in the splModel
*/
compareExpr         @init{ SPL::CompareExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} shiftExpr {cc << 2;} ({cc << 3;} compareOp {cc << 4;} shiftExpr )* {cc << 5;};

/**
@splProduction shiftExpr : addExpr (shiftOp addExpr)*
This production has no explicit representation in the splModel
*/
shiftExpr           @init{ SPL::ShiftExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} addExpr {cc << 2;} ({cc << 3;} shiftOp {cc << 4;}  addExpr)* {cc << 5;};

/**
@splProduction addExpr : multExpr (addOp multExpr)*
This production has no explicit representation in the splModel
*/
addExpr             @init{ SPL::AddExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} multExpr {cc << 2;} ({cc << 3;} addOp {cc << 4;} multExpr)* {cc << 5;};

/**
@splProduction multExpr : prefixExpr (multOp prefixExpr)*
This production has no explicit representation in the splModel
*/
multExpr            @init{ SPL::MultExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} unwrapOrElseExpr {cc << 2;} ({cc << 3;} multOp {cc << 4;} unwrapOrElseExpr)* {cc << 5;};

unwrapOrElseExpr	@init{ SPL::UnwrapOrElseContext cc(context(ctx), *LT(1)); }
					: {cc << 1;} prefixExpr {cc << 2;} ({cc << 3;} QMARK_COLON {cc << 4 ;} prefixExpr {cc << 5; cc._foundUnwrap = true;})? {cc << 6;};

/**
@splProduction prefixExpr : (prefixOrUnaryMinus | (castExpr)=> castExpr | postfixExpr)
This production has no explicit representation in the splModel
*/
prefixExpr          @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( prefixOrUnaryMinus | (castExpr)=> castExpr | postfixExpr) {cc << 2;};

/**
@splProduction prefixOrUnaryMinus : prefixOp prefixExpr
This production has no explicit representation in the splModel
*/
prefixOrUnaryMinus  @init{ SPL::PrefixOrUnaryMinusContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} prefixOp {cc << 2;} prefixExpr {cc << 3;};

/**
@splProduction castExpr : '(' (typeLiteral | typeName) ')' prefixExpr
This production has no explicit representation in the splModel
*/
castExpr            @init{ SPL::CastExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LPAREN (typeLiteral | typeName) RPAREN {cc << 2;} prefixExpr {cc << 3;};

/**
@splProduction postfixExpr : primaryExpr ( '(' expr_pc? ')' | '::' identifier | '.' identifier | '[' subscript ']' | postfixOp )*
This production has no explicit representation in the splModel
*/
postfixExpr         @init{ SPL::PostfixExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} primaryExpr ( {cc << 2;} LPAREN expr_pc? {cc << 3;} RPAREN {cc << 10;}
                                             | {cc << 4;} COLON_COLON identifier {cc << 10;}  
                                             | {cc << 5;} DOT identifier {cc << 10;}
                                             | {cc << 6;} LBRACKET subscript RBRACKET {cc << 10;}
                                             | {cc << 7;} postfixOp {cc << 10;}
                                             | {cc << 8;} BANG {cc << 10;}
                                             | {cc << 9;} QMARK_QMARK {cc << 10;} 
                                             )*;

/**
@splProduction subscript : (':' expr_q | expr ( | ':' expr_q ))
This production has no explicit representation in the splModel
*/
subscript           @init{ SPL::SubscriptContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (COLON expr_q {cc << 2;} | {cc << 3;} expr {cc << 4;} ( /*empty*/ | {cc << 1;} COLON expr_q {cc << 5;}));

/**
@splProduction expr_q : ( | expr )
This production has no explicit representation in the splModel
*/
expr_q              @init{ SPL::ExprQContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (/*empty*/ |  expr {cc << 2;});

/**
@splProduction primaryExpr : (parenExpr | literal | identifier )
This production has no explicit representation in the splModel
*/
primaryExpr         @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (parenExpr | literal | identifier) {cc << 2;};

/**
@splProduction parenExpr : '(' expr ')'
This production has no explicit representation in the splModel
*/
parenExpr           @init{ SPL::ParenExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LPAREN expr RPAREN {cc << 2;}; 

/**
@splProduction typeLiteral : (primitiveType | compositeType | optionalType)
This production has no explicit representation in the splModel
*/
typeLiteral         @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (primitiveType | compositeType | optionalType) {cc << 2;};

/**
@splProduction assignOp : ( '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | rShiftEq | '&=' | '^=' | '|=' )
This production has no explicit representation in the splModel
*/
assignOp            @init{ SPL::AssignOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( EQ {cc << 2;} | PLUS_EQ {cc << 3;} | MINUS_EQ  {cc << 4;} | STAR_EQ   {cc << 5;} | SLASH_EQ  {cc << 6;} | MOD_EQ {cc << 7;}
                                 | LSHIFT_EQ {cc << 8;} | rShiftEq {cc << 9;} | AMP_EQ {cc << 10;} | HAT_EQ {cc << 11;} | BAR_EQ {cc << 12;});

/**
@splProduction bitOrOp : ( '|' | '.|' )
This production has no explicit representation in the splModel
*/
bitOrOp             @init{ SPL::BitOrOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( BAR {cc << 2;} | DOT_BAR {cc << 3;});

/**
@splProduction bitXorOp : ( '^' | '.^' )
This production has no explicit representation in the splModel
*/
bitXorOp            @init{ SPL::BitXorOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( HAT {cc << 2;} | DOT_HAT {cc << 3;});

/**
@splProduction bitAndOp : ( '&' | '.&' )
This production has no explicit representation in the splModel
*/
bitAndOp            @init{ SPL::BitAndOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( AMP {cc << 2;} | DOT_AMP {cc << 3;});

/**
@splProduction compareOp : ( '<' | '<=' | '>' | gEq | '!=' | '==' | '.<' | '.<=' | '.>' | '.>=' | '.!=' | '.==' )
This production has no explicit representation in the splModel
*/
compareOp           @init{ SPL::CompareOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( LESS {cc << 2;} | LEQ  {cc << 3;} | GREATER {cc << 4;} | gEq {cc << 5;}
                                 | NEQ {cc << 6;} | EQ_EQ {cc << 7;} | DOT_LESS {cc << 8;} | DOT_LEQ  {cc << 9;} | DOT_GREATER {cc << 10;}
                                 | DOT_GEQ {cc << 11;} | DOT_NEQ {cc << 12;} | DOT_EQ_EQ {cc << 13;});

/**
@splProduction shiftOp : ( '<<' | rShift | '.<<' | '.>>' )
This production has no explicit representation in the splModel
*/
shiftOp             @init{ SPL::ShiftOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( LSHIFT {cc << 2;} | rShift {cc << 3;} | DOT_LSHIFT {cc << 4;} | DOT_RSHIFT {cc << 5;});

/**
@splProduction addOp : ( '+' | '-' | '.+' | '.-' )
This production has no explicit representation in the splModel
*/
addOp               @init{ SPL::AddOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (PLUS {cc << 2;} | MINUS {cc << 3;} | DOT_PLUS  {cc << 4;} | DOT_MINUS {cc << 5;});

/**
@splProduction multOp : ( '*' | '/' | '%' | '.*' | './' | '.%' )
This production has no explicit representation in the splModel
*/
multOp              @init{ SPL::MultOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (STAR {cc << 2;} | SLASH {cc << 3;} | MOD {cc << 4;} | DOT_STAR  {cc << 5;} | DOT_SLASH {cc << 6;} | DOT_MOD {cc << 7;});

/**
@splProduction prefixOp : ( '!' | '-' | '~' | '++' | '--' )
This production has no explicit representation in the splModel
*/
prefixOp            @init{ SPL::PrefixOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( BANG {cc << 2;} | MINUS {cc << 3;} | TILDE {cc << 4;} | PLUS_PLUS {cc << 5;} | MINUS_MINUS {cc << 6;});

/**
@splProduction postfixOp : ( '++' | '--' )
This production has no explicit representation in the splModel
*/
postfixOp           @init{ SPL::PostfixOpContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (PLUS_PLUS {cc << 2;} | MINUS_MINUS {cc << 3;} );

// ----- Helper productions to circumvent parsing problems with nested generics such as list<list<int32>>.
/**
@splProduction gEq : ('>' '=')=> '>' '='
This production has no explicit representation in the splModel
*/
gEq                 @init{ SPL::GEqContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (GREATER EQ)=> a=GREATER b=EQ assertNoSpace[$a,$b] {cc << 2;};

/**
@splProduction rShift : ('>' '>')=> '>' '>'
This production has no explicit representation in the splModel
*/
rShift              @init{ SPL::RShiftContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (GREATER GREATER)=> a=GREATER b=GREATER assertNoSpace[$a,$b] {cc << 2;};

/**
@splProduction rShiftEq : ('>' '>' '=')=>  '>' '>' '='
This production has no explicit representation in the splModel
*/
rShiftEq            @init{ SPL::RShiftEqContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (GREATER GREATER EQ)=> a=GREATER b=GREATER c=EQ assertNoSpace[$a,$b] assertNoSpace[$b,$c] {cc << 2;};

// ----- Literals are the highest-precedence expressions denoting values.
/**
@splProduction literal : ( listLiteral | curlyLiteral | primitiveLiteral )
This production has no explicit representation in the splModel
*/
literal             @init{ SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (listLiteral | curlyLiteral | primitiveLiteral) {cc << 2;};

/**
@splProduction listLiteral : '[' expr_pc? ']'
This production has no explicit representation in the splModel
*/
listLiteral         @init{ SPL::ListLiteralContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LBRACKET expr_pc? RBRACKET {cc << 2;};

/**
@splProduction curlyLiteral : '{' exprOrMapping_pc? '}'
This production has no explicit representation in the splModel
*/
curlyLiteral        @init{ SPL::CurlyLiteralContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LCURLY exprOrMapping_pc? RCURLY {cc << 2;};

/**
@splProduction stringLiteral : STRING_LITERAL
This production has no explicit representation in the splModel
*/
stringLiteral       @init{ SPL::StringLiteralContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} STRING_LITERAL {cc << 2;};

/**
@splProduction xmlLiteral : XML_LITERAL
This production has no explicit representation in the splModel
*/
xmlLiteral          @init{ SPL::XMLLiteralContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} XML_LITERAL {cc << 2;};

/**
@splProduction primitiveLiteral : ( 'true' | 'false' | stringLiteral | xmlLiteral | FLOAT_LITERAL | INT_LITERAL | HEX_LITERAL | nullLiteral )
This production has no explicit representation in the splModel
*/
primitiveLiteral    @init{ SPL::PrimitiveLiteralContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( TRUE_LITERAL {cc << 2;} | FALSE_LITERAL {cc << 3;} | stringLiteral {cc << 4;} | xmlLiteral {cc << 5;} 
//                                 | FLOAT_LITERAL {cc << 6;} | INT_LITERAL {cc << 7;} | HEX_LITERAL {cc << 8;} | nullLiteral {cc << 9;});
                                 | FLOAT_LITERAL {cc << 6;} | INT_LITERAL {cc << 7;} | HEX_LITERAL {cc << 8;} | NULL_LITERAL {cc << 9;});

// ----- Types are defined in a composite operator's type clause, or in a block, or at the top-level.
/**
@splProduction standAloneTypeDef : 'type' identifier '=' typeDefTail ';'
This production has no explicit representation in the splModel
*/
standAloneTypeDef   @init { SPL::StandaloneTypeDefContext cc(context(ctx), INPUT, *LT(1)); }
                    : {cc << 1;} TYPE identifier {cc << 2;} EQ typeDefTail {cc << 3;} semi {cc << 4;};

/**
@splProduction typeDef : typeModifiers identifier '=' typeDefTail ';'
@splModel
  <xs:complexType name="typeDefinitionType">
    <xs:complexContent>
      <xs:extension base="extendedSourceLocationType">
        <xs:sequence>
          <xs:element name="splDoc" type="splDocType" minOccurs="0"/>
          <xs:element name="modifiers" type="typeModifiersType" minOccurs="0"/>
        </xs:sequence>
        <xs:attribute name="name" use="required" type="xs:string"/>
        <xs:attribute name="value" use="required" type="xs:string"/>
      </xs:extension>
    </xs:complexContent>
  </xs:complexType>
@splModel
*/
typeDef             @init { SPL::TypeDefContext cc(context(ctx), INPUT, *LT(1)); }
                    : {cc << 1;} typeModifiers {cc << 2;} identifier {cc << 3;} EQ typeDefTail {cc << 4;} semi {cc << 5;};

/**
@splProduction typeDefTail : ( (type ';')=> type | tupleBody )
*/
typeDefTail         @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( (type SEMI)=> type | tupleBody ) {cc << 2;};

/**
@splProduction typeModifiers : typeModifier*
@splModel
  <xs:complexType name="typeModifiersType">
    <xs:sequence>      
      <xs:element name="modifier" type="typeModifierType" minOccurs="0" maxOccurs="unbounded"/>
    </xs:sequence>
  </xs:complexType>
@splModel
*/
typeModifiers       @init { SPL::TypeModifiersContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} typeModifier* {cc << 2;};

/**
@splProduction typeModifier : staticT
@splModel
  <xs:complexType name="typeModifierType">
    <xs:attribute name="name" use="required" type="typeModifierEnumType"/>
  </xs:complexType>
@splModel
@splModel
  <xs:simpleType name="typeModifierEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="public"/>
      <xs:enumeration value="static"/>
    </xs:restriction>
  </xs:simpleType>
@splModel
*/
typeModifier        @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} staticT {cc << 2;};

/**
@splProduction staticT : 'static'
This production has no explicit representation in the splModel
*/
staticT             @init { SPL::StaticContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} STATIC {cc << 2;};
                    

/**
@splProduction expressionMode : (attributeT | expressionT ( | typeArgs1 ) | functionT | operatorT | typeT )
@splModel
  <xs:complexType name="compositeParameterExpressionModeType">
    <xs:attribute name="mode" use="required" type="compositeParameterExpressionModeEnumType"/>
    <xs:attribute name="type" type="xs:string"/>
  </xs:complexType>

   <xs:simpleType name="compositeParameterExpressionModeEnumType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="operator"/>
      <xs:enumeration value="function"/>
      <xs:enumeration value="attribute"/>
      <xs:enumeration value="expression"/>
      <xs:enumeration value="type"/>
    </xs:restriction>
  </xs:simpleType>
@splModel
*/
expressionMode      @init { SPL::ExpressionModeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( attributeT | expressionT  {cc << 2;} ( /*empty*/ | typeArgs1) | functionT | operatorT | typeT ) {cc << 3;};

/**
@splProduction returnType : (type | voidT)
This production has no explicit representation in the splModel
*/
returnType          @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (type | voidT) {cc << 2;};

/**
@splProduction type : ( typeName | primitiveType | compositeType )
This production has no explicit representation in the splModel
*/
type                @init { SPL::TypeContext cc(context(ctx), *LT(1), TYPE); }
                    : {cc << 1;} (typeName | primitiveType | compositeType | optionalType) {cc << 2;};

/**
@splProduction typeName : typeNameContents
This production has no explicit representation in the splModel
*/
typeName            @init { SPL::TypeNameContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} typeNameContents {cc << 2;};

/**
@splProduction typeNameContents : typeNameStart ('.' identifier)?
This production has no explicit representation in the splModel
*/
typeNameContents    @init { SPL::TypeNameContentsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} typeNameStart {cc << 2;} (DOT identifier {cc << 3;})?; 

/**
@splProduction typeNameStart : ( ( namespaceName '::')=> namespaceName '::' identifier | identifier )
This production has no explicit representation in the splModel
*/
typeNameStart       @init{ SPL::TypeNameStartContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ((namespaceName COLON_COLON)=> namespaceName {cc << 2;} COLON_COLON identifier {cc << 3;}
                                 | {cc << 2;} identifier {cc << 4;});

/**
@splProduction typeArgs1 : '<' type '>'
This production has no explicit representation in the splModel
*/
typeArgs1           @init { SPL::TypeArgs1Context cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LESS type GREATER {cc << 2;};

/**
@splProduction typeArgs2 : '<' type ',' type '>'
This production has no explicit representation in the splModel
*/
typeArgs2           @init { SPL::TypeArgs2Context cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LESS type {cc << 2;} COMMA type GREATER {cc << 3;}; 

/**
@splProduction typeDims : '[' expr ']'
This production has no explicit representation in the splModel
*/
typeDims            @init { SPL::TypeDimsContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LBRACKET expr RBRACKET {cc << 2;};

// ----- For type checking and debugging.
/**
@splProduction splType : typeLiteral EOF
This production has no explicit representation in the splModel
*/
splType             @init{SPL::SPLTypeContext cc(context(ctx), *LT(1), TYPE); }
                    : {cc << 1;} typeLiteral EOF {cc << 2;};

/**
@splProduction splExpr : expr EOF
This production has no explicit representation in the splModel
*/
splExpr             @init{SPL::SPLExprContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} expr EOF {cc << 2;};

// ----- Primitive types are types without other types as arguments.

/**
@splProduction primitiveType : ( enumType | stringType | xmlType | simplePrimitiveType )
This production has no explicit representation in the splModel
*/
primitiveType       @init{SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (enumType | stringType | xmlType | simplePrimitiveType) {cc << 2;};

/**
@splProduction xmlType : 'xml' xmlSchema?
This production has no explicit representation in the splModel
*/
xmlType             @init{SPL::XMLTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} XML {cc << 2;} (xmlSchema {cc << 3;})?;

/**
@splProduction xmlSchema : '<' stringLiteral '>'
This production has no explicit representation in the splModel
*/
xmlSchema           @init{SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LESS stringLiteral GREATER {cc << 2;};

/**
@splProduction enumType : 'enum' '{' id_pc '}'
This production has no explicit representation in the splModel
*/
enumType            @init{SPL::EnumTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ENUM LCURLY id_pc RCURLY {cc << 2;};

/**
@splProduction stringType : ( 'rstring' typeDims? | 'ustring' )
This production has no explicit representation in the splModel
*/
stringType          @init{SPL::StringTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (RSTRING {cc << 2;} (typeDims {cc << 3;})? | USTRING {cc << 4;});

/**
@splProduction simplePrimitiveType : simplePrimitiveTypeName
This production has no explicit representation in the splModel
*/
simplePrimitiveType @init{SPL::SimplePrimitiveTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} simplePrimitiveTypeName {cc << 2;};

/**
@splProduction simplePrimitiveTypeName : ( 'boolean' | 'int8' | 'int16' | 'int32' | 'int64' | 'uint8' | 'uint16' | 'uint32' | 'uint64' | 'float32' | 'decimal32' | 'decimal64' | 'decimal128' | 'complex32' | 'complex64' | 'timestamp' | 'blob')
This production has no explicit representation in the splModel
*/
simplePrimitiveTypeName @init{SPL::SimplePrimitiveTypeNameContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( BOOLEAN  {cc << 2;} | INT8 {cc << 3;} | INT16 {cc << 4;} | INT32 {cc << 5;} | INT64 {cc << 6;} | UINT8 {cc << 7;} | UINT16 {cc << 8;}
                                 | UINT32     {cc << 9;} | UINT64 {cc << 10;} | FLOAT32 {cc << 11;} | FLOAT64 {cc << 12;} | DECIMAL32  {cc << 13;} | DECIMAL64  {cc << 14;}
                                 | DECIMAL128 {cc << 15;} | COMPLEX32  {cc << 16;} | COMPLEX64  {cc << 17;} | TIMESTAMP  {cc << 18;} | BLOB {cc << 19;});

// ----- Composite types are type constructors for composing new types out of other types.

/**
@splProduction compositeType : ( tupleType | listType | setType | mapType )
This production has no explicit representation in the splModel
*/
compositeType       @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (tupleType | listType | setType | mapType) {cc << 2;};

/**
@splProduction listType : 'list'  typeArgs1 typeDims?
This production has no explicit representation in the splModel
*/
listType            @init { SPL::ListTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LIST typeArgs1 {cc << 2; } (typeDims {cc << 3;})?;  

/**
@splProduction setType : 'set' typeArgs1 typeDims?
This production has no explicit representation in the splModel
*/
setType             @init { SPL::SetTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} SET typeArgs1 {cc << 2; } (typeDims {cc << 3;})?;

/**
@splProduction mapType : 'map' typeArgs2 typeDims?
This production has no explicit representation in the splModel
*/
mapType             @init { SPL::MapTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} MAP typeArgs2 {cc << 2; } (typeDims {cc << 3;})?;

/**
@splProduction tupleType : 'tuple' tupleArgs
This production has no explicit representation in the splModel
*/
tupleType           @init { SPL::TupleTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} TUPLE tupleArgs {cc << 2;};

/**
@splProduction tupleArgs : '<' tupleBody '>'
This production has no explicit representation in the splModel
*/
tupleArgs           @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LESS tupleBody GREATER {cc << 2;};

/**
@splProduction tupleBody : ( (attributeDecl)=> attributeDecl_pc | tupleExtendee_pc )
This production has no explicit representation in the splModel
*/
tupleBody           @init { SPL::TupleBodyContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ( (attributeDecl)=> attributeDecl_pc {cc << 2;} | tupleExtendee_pc {cc << 3;});

/**
@splProduction optionalType : '<' type '>'
This production has no explicit representation in the splModel
*/
optionalType        @init { SPL::OptionalTypeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} OPTIONAL LESS (typeName | primitiveType | compositeType) GREATER {cc << 2;};

/**
@splProduction attributeDecl : type identifier
This production has no explicit representation in the splModel
*/
attributeDecl       @init { SPL::AttributeDeclContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} type {cc << 2;} identifier {cc << 3;};

/**
@splProduction tupleExtendee : (typeName | tupleType )
This production has no explicit representation in the splModel
*/
tupleExtendee       @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} (typeName | tupleType) {cc << 2;};

// ----- Function Prototype declarations come from XML files, not regular SPL files, hence the "EOF" at the end.
/**
@splProduction functionPrototype : typeFormals boundsFormals functionModifiers returnType identifier protoFormals EOF
This production has no explicit representation in the splModel
*/
functionPrototype   @init { SPL::FunctionPrototypeContext cc(context(ctx), *LT(1), FUNCTIONPROTOTYPE); }
                    : {cc << 1;} typeFormals {cc << 2;} boundsFormals {cc << 3;} functionModifiers {cc << 4;} returnType {cc << 5;} identifier {cc << 6;} protoFormals EOF {cc << 7;};

/**
@splProduction typeFormal : typeFormalConstraint identifier
This production has no explicit representation in the splModel
*/
typeFormal          @init { SPL::TypeFormalContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} typeFormalConstraint {cc << 2;} identifier {cc << 3;};

/**
@splProduction typeFormalConstraint : ( optionalT | compositeT | enumT | listT | mapT | setT | tupleT | xmlT | (semiKeyword["any"])=> identifier | (semiKeyword["collection"])=> identifier | (semiKeyword["complex"])=> identifier | (semiKeyword["complex"])=> identifier | (semiKeyword["decimal"])=> identifier | (semiKeyword["float"])=> identifier | (semiKeyword["floatingpoint"])=> identifier | (semiKeyword["integral"])=> identifier | (semiKeyword["integral"])=> identifier | (semiKeyword["numeric"])=> identifier | (semiKeyword["ordered"])=> | identifier | (semiKeyword["primitive"])=> identifier |(semiKeyword["string"])=> identifier )
This production has no explicit representation in the splModel
*/
typeFormalConstraint  @init { SPL::PassThroughContext cc(context(ctx), *LT(1)); }
                      : {cc << 1;} ( optionalT | compositeT | enumT | listT | mapT | setT | tupleT | xmlT
                                   | (semiKeyword["any"])=>           identifier
                                   | (semiKeyword["collection"])=>    identifier
                                   | (semiKeyword["complex"])=>       identifier
                                   | (semiKeyword["decimal"])=>       identifier
                                   | (semiKeyword["float"])=>         identifier
                                   | (semiKeyword["floatingpoint"])=> identifier
                                   | (semiKeyword["integral"])=>      identifier
                                   | (semiKeyword["numeric"])=>       identifier
                                   | (semiKeyword["ordered"])=>       identifier
                                   | (semiKeyword["primitive"])=>     identifier
                                   | (semiKeyword["string"])=>        identifier) {cc << 2;};




/**
@splProduction optionalT : 'optional'
This production has no explicit representation in the splModel
*/
optionalT           @init { SPL::OptionalContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} OPTIONAL {cc << 2;};

/**
@splProduction compositeT : 'composite'
This production has no explicit representation in the splModel
*/
compositeT          @init { SPL::CompositeContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} COMPOSITE {cc << 2;};

/**
@splProduction enumT : 'enum'
This production has no explicit representation in the splModel
*/
enumT               @init { SPL::EnumContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} ENUM {cc << 2;};

/**
@splProduction listT : 'list'
This production has no explicit representation in the splModel
*/
listT               @init { SPL::ListContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} LIST {cc << 2;};

/**
@splProduction mapT : 'map'
This production has no explicit representation in the splModel
*/
mapT                @init { SPL::MapContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} MAP {cc << 2;};

/**
@splProduction setT : 'set'
This production has no explicit representation in the splModel
*/
setT                @init { SPL::SetContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} SET {cc << 2;};

/**
@splProduction xmlT : 'xml'
This production has no explicit representation in the splModel
*/
xmlT                @init { SPL::XMLContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} XML {cc << 2;};

/**
@splProduction tupleT : 'tuple'
This production has no explicit representation in the splModel
*/
tupleT              @init { SPL::TupleContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} TUPLE {cc << 2;};

/**
@splProduction protoFormal : formalModifiers type identifier?
This production has no explicit representation in the splModel
*/
protoFormal         @init { SPL::ProtoFormalContext cc(context(ctx), *LT(1)); }
                    : {cc << 1;} formalModifiers {cc << 2;} type {cc << 3;} identifier? {cc << 4;};

// ----- lists (_ps = plus semicolon = +;) (_pc = plus comma = +,) (_pp = plus period = +.)
/**
@splProduction attributeDecl_pc : attributeDecl (',' attributeDecl )*
This production has no explicit representation in the splModel
*/
attributeDecl_pc    : attributeDecl (COMMA attributeDecl)*;

/**
@splProduction exprOrMapping_pc : exprOrMapping (',' exprOrMapping )*
This production has no explicit representation in the splModel
*/
exprOrMapping_pc    : exprOrMapping (COMMA exprOrMapping)*;

/**
@splProduction expr_pc : expr (',' expr )*
This production has no explicit representation in the splModel
*/
expr_pc             : expr (COMMA expr)*;

/**
@splProduction id_pc : identifier (',' identifier )*
This production has no explicit representation in the splModel
*/
id_pc              : identifier (COMMA identifier)*;

/**
@splProduction localDeclItem_pc : localDeclItem (',' localDeclItem )*
This production has no explicit representation in the splModel
*/
localDeclItem_pc    : localDeclItem (COMMA localDeclItem)*;

/**
@splProduction protoFormal_pc : protoFormal (',' protoFormal )*
This production has no explicit representation in the splModel
*/
protoFormal_pc      : protoFormal (COMMA protoFormal)*;

/**
@splProduction opOutput_ps : opOutput (';'  opOutput )
This production has no explicit representation in the splModel
*/
opOutput_ps         : opOutput (SEMI opOutput)*;

/**
@splProduction outputAssign_pc : outputAssign (',' outputAssign )*
This production has no explicit representation in the splModel
*/
outputAssign_pc     : outputAssign (COMMA outputAssign)*;

/**
@splProduction portInputs_ps : portInputs (';'  portInputs )*
This production has no explicit representation in the splModel
*/
portInputs_ps       : portInputs (SEMI portInputs)*;

/**
@splProduction tupleExtendee_pc : tupleExtendee (',' tupleExtendee )*
This production has no explicit representation in the splModel
*/
tupleExtendee_pc    : tupleExtendee (COMMA tupleExtendee)*;

/**
@splProduction typeFormal_pc : typeFormal (',' typeFormal )*
This production has no explicit representation in the splModel
*/
typeFormal_pc       : typeFormal (COMMA typeFormal)*;

/**
@splProduction identifier : ID
This production has no explicit representation in the splModel
*/
identifier          @init {SPL::IdentifierContext cc(context(ctx), *LT(1));}
                    : {cc << 1;} ID {cc << 2;};

// ----- tokens

/**
@splProduction ID : DOLLAR? LETTER (DIGIT | LETTER)*
This production has no explicit representation in the splModel
*/
ID                  : DOLLAR? LETTER (DIGIT | LETTER)*;

/**
@splProduction INT_LITERAL : ('0' | POSDIGIT DIGIT*) NUM_SUFFIX?
This production has no explicit representation in the splModel
*/
INT_LITERAL         : ('0' | POSDIGIT DIGIT*) NUM_SUFFIX?;

/**
@splProduction HEX_LITERAL : HEX_PRE HEXDIGIT* HEX_SUF_T?
This production has no explicit representation in the splModel
*/
HEX_LITERAL         : HEX_PRE HEXDIGIT* HEX_SUF_T?;

/**
@splProduction FLOAT_LITERAL : ('0' | POSDIGIT DIGIT*) '.' DIGIT* EXPONENT? NUM_SUFFIX? | '.' DIGIT+ EXPONENT? NUM_SUFFIX? | ('0' | POSDIGIT DIGIT*) EXPONENT NUM_SUFFIX?
This production has no explicit representation in the splModel
*/
FLOAT_LITERAL       : ('0' | POSDIGIT DIGIT*) DOT DIGIT* EXPONENT? NUM_SUFFIX?
                    | DOT DIGIT+ EXPONENT? NUM_SUFFIX?
                    | ('0' | POSDIGIT DIGIT*) EXPONENT NUM_SUFFIX?;

/**
@splProduction NUM_SUFFIX : NUM_SUF_T | NUM_SUF_B | NUM_SUF_T NUM_SUF_B | NUM_SUF_B NUM_SUF_T
This production has no explicit representation in the splModel
*/
fragment NUM_SUFFIX : NUM_SUF_T | NUM_SUF_B | NUM_SUF_T NUM_SUF_B | NUM_SUF_B NUM_SUF_T;

/**
@splProduction NUM_SUF_T : 's' | 'u' | 'f' | 'd'
This production has no explicit representation in the splModel
*/
fragment NUM_SUF_T  : 's' | 'u' | 'f' | 'd';

/**
@splProduction NUM_SUF_B : 'b' | 'h' | 'w' | 'l' | 'q'
This production has no explicit representation in the splModel
*/
fragment NUM_SUF_B  : 'b' | 'h' | 'w' | 'l' | 'q';

/**
@splProduction HEX_SUF_T : 's' | 'u'
This production has no explicit representation in the splModel
*/
fragment HEX_SUF_T  : 's' | 'u';

/**
@splProduction EXPONENT : ('e'|'E') ('+'|'-')? DIGIT+
This production has no explicit representation in the splModel
*/
fragment EXPONENT   : ('e'|'E') (PLUS|MINUS)? DIGIT+;

/**
@splProduction STRING_LITERAL : (DQUOTE (STRINGESC | STRINGCHAR | SQUOTE)* DQUOTE | SQUOTE (STRINGCHAR | DQUOTE)* SQUOTE) STR_SUF_T?
This production has no explicit representation in the splModel
*/
STRING_LITERAL      : (DQUOTE (STRINGESC | STRINGCHAR | SQUOTE)* DQUOTE | SQUOTE (STRINGESC | STRINGCHAR | DQUOTE)* SQUOTE)  STR_SUF_T?;

/**
@splProduction STR_SUF_T : 'r' | 'u'
This production has no explicit representation in the splModel
*/
fragment STR_SUF_T  : 'r' | 'u';

/**
@splProduction XML_LITERAL : (DQUOTE (STRINGESC | STRINGCHAR | SQUOTE)* DQUOTE | SQUOTE (STRINGCHAR | DQUOTE)* SQUOTE) 'x'
This production has no explicit representation in the splModel
*/
XML_LITERAL         : (DQUOTE (STRINGESC | STRINGCHAR | SQUOTE)* DQUOTE | SQUOTE (STRINGESC | STRINGCHAR | DQUOTE)* SQUOTE) 'x';

/**
@splProduction WHITE_SPACE : (' '|'\t'|'\n'|'\r')+
This production has no explicit representation in the splModel
*/
WHITE_SPACE         : (' '|'\t'|'\n'|'\r')+                                 { $channel = HIDDEN; };

/**
@splProduction ML_COMMENT : SLASH_STAR .* STAR_SLASH
This production has no explicit representation in the splModel
*/
ML_COMMENT          : SLASH_STAR (options {greedy=false;} : . )* STAR_SLASH { $channel = HIDDEN; };

/**
@splProduction SL_COMMENT : SLASH_SLASH (~('\n'|'\r'))* ('\n'|'\r')?
This production has no explicit representation in the splModel
*/
SL_COMMENT          : SLASH_SLASH (~('\n'|'\r'))* ('\n'|'\r')?              { $channel = HIDDEN; };

/**
@splProduction STRINGESC : '\\' ( DQUOTE | '\\' | '?' | SQUOTE | 'a' | 'b' | 'f' | 'n' | 'r' | 't' | 'v' | 'u' HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT | '0')
This production has no explicit representation in the splModel
*/
fragment STRINGESC  : '\\' (DQUOTE|'\\'|'?'|'\''|'a'|'b'|'f'|'n'|'r'|'t'|'v'|'u' HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT | '0');

/**
@splProduction STRINGCHAR : ~( '\\' | DQUOTE | SQUOTE)
This production has no explicit representation in the splModel
*/
fragment STRINGCHAR : ~('\\'|DQUOTE|SQUOTE);

/**
@splProduction LETTER : 'a'..'z' | 'A'..'Z' | '_'
This production has no explicit representation in the splModel
*/
fragment LETTER     : 'a'..'z' | 'A'..'Z' | '_';


/**
@splProduction DIGIT : '0'..'9'
This production has no explicit representation in the splModel
*/
fragment DIGIT      : '0'..'9';

/**
@splProduction POSDIGIT : '1'..'9'
This production has no explicit representation in the splModel
*/
fragment POSDIGIT   : '1'..'9';

/**
@splProduction HEX_PRE : '0x'
This production has no explicit representation in the splModel
*/
fragment HEX_PRE    : '0x';

/**
@splProduction HEXDIGIT : DIGIT | 'a'..'f' | 'A'..'F'
This production has no explicit representation in the splModel
*/
fragment HEXDIGIT   : DIGIT | 'a'..'f' | 'A'..'F';

// ----- Helper productions involving C++ code.
assertNoSpace       [pANTLR3_COMMON_TOKEN t1, pANTLR3_COMMON_TOKEN t2] 
                    : { $t1->line == $t2->line && $t1->getCharPositionInLine($t1) + 1 == $t2->getCharPositionInLine($t2) }? ;

semiKeyword         [char* w] 
                    : ID assertEquals[(char*)$ID.text->chars, w];

assertEquals        [char* s, char* t] 
                    : { 0 == strcmp(s, t) }?;
