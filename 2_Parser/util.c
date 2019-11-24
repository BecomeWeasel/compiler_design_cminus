/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
{
  switch (token)
  {
  case IF:
  //case THEN:
  case ELSE:
  case WHILE:
  case RETURN:
  case INT:
  case VOID:
    //case END:
    //case REPEAT:
    //case UNTIL:
    //case READ:
    //case WRITE:
    fprintf(listing,
            "reserved word: %s\n", tokenString);
    break;
  case ASSIGN:
    fprintf(listing, "=\n");
    break;
  case LT:
    fprintf(listing, "<\n");
    break;
  case LE:
    fprintf(listing, "<=\n");
    break;
  case GT:
    fprintf(listing, ">\n");
    break;
  case GE:
    fprintf(listing, ">=\n");
    break;
  case EQ:
    fprintf(listing, "==\n");
    break;
  case NE:
    fprintf(listing, "!=\n");
    break;
  case LPAREN:
    fprintf(listing, "(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\n");
    break;
  case LBRACE:
    fprintf(listing, "[\n");
    break;
  case RBRACE:
    fprintf(listing, "]\n");
    break;
  case LCURLY:
    fprintf(listing, "{\n");
    break;
  case RCURLY:
    fprintf(listing, "}\n");
    break;
  case COMMA:
    fprintf(listing, ",\n");
    break;
  case SEMI:
    fprintf(listing, ";\n");
    break;
  case PLUS:
    fprintf(listing, "+\n");
    break;
  case MINUS:
    fprintf(listing, "-\n");
    break;
  case TIMES:
    fprintf(listing, "*\n");
    break;
  case OVER:
    fprintf(listing, "/\n");
    break;
  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  case NUM:
    fprintf(listing,
            "NUM, val= %s\n", tokenString);
    break;
  case ID:
    fprintf(listing,
            "ID, name= %s\n", tokenString);
    break;
  case ERROR:
    fprintf(listing,
            "ERROR: %s\n", tokenString);
    break;
  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode *newDclNode(DclKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DclK;
    t->kind.dcl = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode *newTypNode(TypKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = TypeNK;
    t->kind.typ = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newPrmNode(ParamKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ParamK;
    t->kind.prm = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = malloc(n);
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  while (tree != NULL)
  {
    printSpaces();
    if (tree->nodekind == StmtK)
    {
      switch (tree->kind.stmt)
      {
      case CmpK:
        fprintf(listing, "Compound Statement\n");
        break;
      case ItrK:
        fprintf(listing, "While (condition)\n");
        break;
      case IfK:
        fprintf(listing, "If (condition) (body) \n");
        break;
      case IfElseK:
        fprintf(listing, "If (condition) (body) (else)\n");
        break;
      case RetK:
        fprintf(listing, "Return (source)\n");
        break;
      default:
        fprintf(listing, "Unknown StmtNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == DclK)
    {
      switch (tree->kind.dcl)
      {
      case VarK:
        fprintf(listing, "Variable Declaration: %s\n", tree->attr.name);
        break;
      case ArrVarK:
        fprintf(listing,
                "Array Variable Declaration(following length of array): %s %d\n",
                tree->attr.arr.name,
                tree->attr.arr.size);
        break;
      case FunK:
        fprintf(listing, "Function Declaration: %s ", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown DelcareNode Kind\n");
        break;
      }
    }
    else if (tree->nodekind == TypeNK)
    {
      switch (tree->kind.typ)
      {
      case TypK:
        fprintf(listing, "Type: ");
        switch (tree->attr.type)
        {
        case INT:
          fprintf(listing, "int\n");
          break;
        case VOID:
          fprintf(listing, "void\n");
          break;
        }
        break;
      default:
        fprintf(listing, "Unknown TypeNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ParamK)
    {
      switch (tree->kind.prm)
      {
      case NonArrPrmK:
        fprintf(listing, "Parameter: %s\n", tree->attr.name);
        break;
      case ArrPrmK:
        fprintf(listing, "Array Parameter: %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ParamNode kind\n");
        break;
      }
    }
    else if (tree->nodekind == ExpK)
    {
      switch (tree->kind.exp)
      {
      case AssK:
        fprintf(listing, "Assign to: (destination) (source)\n");
        break;
      case IdK:
        fprintf(listing, "ID: %s\n", tree->attr.name);
        break;
      case ArrIdK:
        fprintf(listing, "Array ID\n");
        break;
      case OpK:
        fprintf(listing, "Op: ");
        printToken(tree->attr.op, "\0");
        break;
      case ConstK:
        fprintf(listing, "Const: %d\n", tree->attr.val);
        break;
      case CallK:
        fprintf(listing, "Function call : %s ,with arguments below\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}