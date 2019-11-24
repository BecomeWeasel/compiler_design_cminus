/****************************************************/
/* File: cminus.y                                   */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedArraySizeNumber; /* assignemnt statement used */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

%}

%token IF THEN ELSE WHILE RETURN INT VOID
%token ID NUM 
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY SEMI COMMA
%token ERROR

%nonassoc RPAREN 
%nonassoc ELSE


%% /* Grammar for TINY */

program     : declaration_list
                 { savedTree = $1;} 
            ;
declaration_list : declaration_list declaration {
  YYSTYPE t=$1;
  if(t!=NULL){
    while(t->sibling!=NULL)
      t = t->sibling;
    t->sibling=$2;
    $$=$1;
  }else $$=$2;
}| declaration { $$=$1; };
declaration : var_declaration {$$=$1;} |func_declaration {$$=$1;};
id : ID {
  savedName=copyString(tokenString); 
  savedLineNo=lineno;
  };
num : NUM {
  savedArraySizeNumber=atoi(tokenString);
  savedLineNo=lineno;
  };
var_declaration :type_specifier id SEMI
{
  $$=newDclNode(VarK);
  $$->child[0]=$1;
  $$->attr.name=savedName;
  $$->lineno=lineno;
}
| type_specifier id LBRACE num RBRACE SEMI
{
  $$=newDclNode(ArrVarK);
  $$->child[0]=$1;
  $$->lineno=lineno;
  $$->attr.arr.name=savedName;
  $$->attr.arr.size=savedArraySizeNumber;
};
type_specifier : INT
{
  $$=newTypNode(TypK);
  $$->attr.type=INT;
}
| VOID
{
  $$=newTypNode(TypK);
  $$->attr.type=VOID;
};
func_declaration : type_specifier id {
  $$=newDclNode(FunK);
  $$->attr.name=savedName;
  $$->lineno=lineno;
} LPAREN params RPAREN compound_stmt{
  $$=$3;
  $$->child[0]=$1; //ret type
  $$->child[1]=$5; // parameters
  $$->child[2]=$7; // function statment body
}
params : param_list {$$=$1;}|
  VOID {
    $$=newTypNode(TypK);
    $$->attr.type=VOID;
};
param_list : param_list COMMA param
  {
    YYSTYPE t=$1;
    if(t!=NULL)
    {
      while(t->sibling!=NULL)
        t=t->sibling;
      t->sibling=$3;
      $$=$1;
    }
    else $$=$3;
  }
  | param {$$=$1;};
param :type_specifier id
{
  $$=newPrmNode(NonArrPrmK);
  $$->child[0]=$1;
  $$->attr.name=savedName;
}|type_specifier id LBRACE RBRACE
{
  $$=newPrmNode(ArrPrmK);
  $$->child[0]=$1;
  $$->attr.name=savedName;
};
compound_stmt :LCURLY local_declarations statement_list RCURLY
{
  $$=newStmtNode(CmpK);
  $$->child[0]=$2;
  $$->child[1]=$3;
};
local_declarations :local_declarations var_declaration
{
  YYSTYPE t=$1;
  if(t!=NULL){
    while(t->sibling!=NULL)
      t=t->sibling;
    t->sibling=$2;
    $$=$1;
  }
  else {
    $$=$2;
  }
} | {$$=NULL;};
statement_list : statement_list statement
{
  YYSTYPE t=$1;
  if(t!=NULL){
    while(t->sibling!=NULL)
      t=t->sibling;
    t->sibling=$2;
    $$=$1;   
  }
  else {$$=$2;}
} | { $$=NULL; };
statement : expression_stmt {$$=$1;}
|compound_stmt{ $$=$1;}
|selection_stmt {$$=$1;}
|iteration_stmt {$$=$1;}
|return_stmt {$$=$1;};
expression_stmt :expression SEMI{$$=$1;}|
SEMI { $$=NULL; };
selection_stmt :IF LPAREN expression RPAREN statement
{
  $$=newStmtNode(IfK);
  $$->child[0]=$3; //if condition
  $$->child[1]=$5; // if statment;
  $$->child[2]=NULL;
}
| IF LPAREN expression RPAREN statement ELSE statement
{
  $$=newStmtNode(IfElseK);
  $$->child[0]=$3; //if condition
  $$->child[1]=$5; // if statment;
  $$->child[2]=$7; // else statment;
};
iteration_stmt :WHILE LPAREN expression RPAREN statement
{
  $$=newStmtNode(ItrK);
  $$->child[0]=$3;
  $$->child[1]=$5;
};
return_stmt : RETURN SEMI
{
  $$=newStmtNode(RetK);
  $$->child[0]=NULL;
}
|RETURN expression SEMI
{
  $$=newStmtNode(RetK);
  $$->child[0]=$2;
};
expression : var ASSIGN expression
{
  $$=newExpNode(AssK);
  $$->child[0]=$1;
  $$->child[1]=$3;
}
| simple_expression { $$=$1;};
var : id 
{
  $$=newExpNode(IdK);
  $$->attr.name=savedName;
}
| id 
{
  $$=newExpNode(ArrIdK);
  $$->attr.name=savedName;
} LBRACE expression RBRACE
{
  $$->child[0]=$3;
};
simple_expression : additive_expression EQ additive_expression
{ $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->attr.op = EQ;
}
| additive_expression NE additive_expression
{ $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->attr.op = NE;
}
| additive_expression LT additive_expression
{ $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->attr.op = LT;
}
| additive_expression LE additive_expression
{ $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->attr.op = LE;
}
| additive_expression GT additive_expression
{ $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->attr.op = GT;
}
| additive_expression GE additive_expression
{ $$ = newExpNode(OpK);
  $$->child[0] = $1;
  $$->child[1] = $3;
  $$->attr.op = GE;
}| additive_expression{ $$ = $1; };
additive_expression : additive_expression PLUS term
{
  $$=newExpNode(OpK);
  $$->child[0]=$1;
  $$->child[1]=$3;
  $$->attr.op=PLUS;
}
| additive_expression MINUS term
{
  $$=newExpNode(OpK);
  $$->child[0]=$1;
  $$->child[1]=$3;
  $$->attr.op=MINUS;
}
| term {$$=$1;};
term : term TIMES factor
{
  $$=newExpNode(OpK);
  $$->child[0]=$1;
  $$->child[1]=$3;
  $$->attr.op=TIMES;
}
| term OVER factor
{
  $$=newExpNode(OpK);
  $$->child[0]=$1;
  $$->child[1]=$3;
  $$->attr.op=OVER;
}
| factor {$$=$1;};
factor : LPAREN expression RPAREN {
  $$=$2;
}
| var {$$=$1;}
| call {$$=$1;}
| NUM
{
  $$=newExpNode(ConstK);
  $$->attr.val=atoi(tokenString);
};
call : id 
{
  $$=newExpNode(CallK);
  $$->attr.name=savedName;
} LPAREN args RPAREN
{
  $$=$2;
  $$->child[0]=$4;
};
args : arg_list {$$=$1;} | {$$=NULL;};
arg_list : arg_list COMMA expression
{
  YYSTYPE t=$1;
  if(t!=NULL){
    while(t->sibling!=NULL)
      t=t->sibling;
    t->sibling=$3;
    $$=$1;
  }else $$=$3;
}
| expression {$$=$1;};
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

