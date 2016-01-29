#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pmath.h"
#include "strlib.h"

/* Use this to toggle */
/* #define PRINT_DEBUG  */

#ifdef PRINT_DEBUG
#define DEBUG(FMTSTR, ...); fprintf(stderr, FMTSTR, ##__VA_ARGS__);
#else
#define DEBUG(x, ...) /* x */
#endif 

/* ---- Allocators ---- */
Expression *new_expression(Entity *l_ent, int operation, Entity *r_ent);
Entity *new_entity(char *raw);
/* ---- Parse functions ---- */
Expression *parse_expr(int *moverv, char *str);
Entity *parse_token(int *move, char *str);
int parse_numeric(long *val, char *str);
int parse_tag(char *tag, char *str);
/* ---- Validate functions (subject to removal */
int validate_token(char *str);
int validate_numeric(char *str);
int validate_expr(char *str);
int validate_tag(char *str);


/* ========================================================================= */
int validate_expr(char *str)
{
   char *start = str;
   int move;

   DEBUG("Validating expression: \"%s\".\n", str);

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str != '(' )
      return(0);

   str++; /* Move off ( char */

   /* Walk off leading white space */
   eat_ws(&str);

   if ( 0 == ( move = validate_token(str) ) )
   {
      /* fprintf(stderr, "ERROR: Failed to parse token \"%s\".\n", str); */
      /* Errors are printed at point of failure */
      return(0);
   }

   str += move;

   /* Walk off leading white space */
   eat_ws(&str);

   /* STUB: perhaps make this a if statement */
   /* STUB: DEBUG("Validating operator: \"%s\"\n", str); */
   switch ( *str )
   {
   case '+':
   case '-':
   case '*':
   case '/':
      str++;
      break;
   default:
      fprintf(stderr, "ERROR: Operation function not parsed at \"%s\" in line STUB.\n", str);
      return(0);
      break;
   }

   /* Walk off leading white space */
   eat_ws(&str);

   if ( 0 == ( move = validate_token(str) ) )
   {
      fprintf(stderr, "ERROR: Parsing failed at token \"%s\" line STUB.\n", str);
      return(0);
   }

   str += move;

   /* Walk off trailing/leading white space */
   eat_ws(&str);

   if ( *str == ')' )
   {
      str++;

      return((int)(str - start));
   }

   /* STUB: DEBUG("  validate_expr() ---> %d\n", (int)(str - start)); */
   return(0);
}

/* ========================================================================= */
Expression *parse_expr(int *moverv, char *str)
{
   Expression *exp;

   /* Parts required for the Expression */
   Entity *l_ent;
   Entity *r_ent;
   int operation = EXPOP_NOP;
   int lineno;
   char *start = str;
   int move;

   lineno = 7; /* STUB: Hard-coded */

   DEBUG("Parsing expression: \"%s\".\n", str);

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str != '(' )
   {
      fprintf(stderr, "ERROR: Failed to recognize the beginning of expression \"%s\" at line %d.\n", str, lineno);
      return(NULL);
   }

   str++; /* Move off ( char */

   /* Walk off leading white space */
   eat_ws(&str);

   /* Parse the first token */
   if ( NULL == ( l_ent = parse_token(&move, str) ) )
   {
      /* parse_token() never leaves without an error */
      /* STUB: printf("ERROR: Parsing failed at token \"%s\" line STUB.\n", str); */
      return(NULL);
   }

   /* The first token parse was good. Move off that location. */
   str += move;

   /* Walk off leading white space */
   eat_ws(&str);

   /* Get the operator */
   DEBUG("Validating operator: \"%s\"\n", str);
   switch ( *str )
   {
   case '+':
      operation = EXPOP_ADD;
      str++;
      break;
   case '-':
      operation = EXPOP_SUB;
      str++;
      break;
   case '*':
      operation = EXPOP_MUL;
      str++;
      break;
   case '/':
      operation = EXPOP_DIV;
      str++;
      break;
   default:
      printf("ERROR: Operation function not parsed at \"%s\" line %d.\n", str, lineno);
      return(NULL);
      break;
   }

   /* Walk off leading white space */
   eat_ws(&str);

   if ( 0 == ( r_ent = parse_token(&move, str) ) )
   {
      /* parse_token() never leaves without an error */
      /* printf("Parsing failed at token \"%s\".\n", str); */
      return(NULL);
   }

   str += move;

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == ')' )
   {
      str++;

      if ( NULL == ( exp = new_expression(l_ent, operation, r_ent) ) )
      {
         /* Print error at point of error */
         return(NULL);
      }

      /* This needs to be saved and returned passbyvalue */
      *moverv = (int)(str - start);

      return(exp);
   }

   /* Fall through to error. We may lose the l_ent and r_ent malloc()s,
      but that is not a problem because will exit on the parse failure. */
   fprintf(stderr, "ERROR: Failed to find terminating condition for expr \"%s\" on line %d.\n",
           start, lineno);
   return(NULL);
}

/* ========================================================================= */
int validate_tag(char *str)
{
   char *start = str;

   DEBUG("Validating as tag: \"%s\".\n", str);

   /* Walk off leading white space */
   eat_ws(&str);

   /* Leading char cannot be a numeric */
   if (( *str >= '0' ) && ( *str <= '9' ))
   {
      /* This code can be reachable! This function is used to "probe"
         a token to see if it validates properly. */
      /* printf(" The tag \"%s\" contains a numeric.\n", str); */
      return(0);
   }

   while (( *str != ' ' ) && ( *str != '\t' ) && ( *str != 0 ) && ( *str != ')' ))
   {
      /* I broke this conditional out so it is not so terse.
         Hopefully this is a payoff for the kruftiness. */
      if (( *str >= '0' ) && ( *str <= '9' ))
      {
         str++;
      }
      else if (( *str >= 'a' ) && ( *str <= 'z' ))
      {
         str++;
      }
      else if (( *str >= 'A' ) && ( *str <= 'Z' ))
      {
         str++;
      }
      else
      {
         /* printf(" Tag \"%s\" has an invalid character.\n", str); */
         return(0);
      }
   }
   
   DEBUG("  validate_tag() ---> %d\n", (int)(str - start));
   return((int)(str - start));
}

/* ========================================================================= */
int validate_numeric(char *str)
{
   char *start = str;
   int isdec = 0;

   /* DEBUG("Validating as numeric: \"%s\".\n", str); */

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '-' )
   {
      isdec = 1;
      str++; /* Move off the '-' char */

      /* Walk off leading white space. Note: This could be an if(), but
         my fear is the extreme edge case of two spaces from the neg to the
         actual number. Technically any amount of white space should be fair
         but I am being somewhat limiting here. */
      eat_ws(&str);

      while (( *str >= '0' ) && ( *str <= '9' ))
         str++;

      return((int)(str - start));
   }

   if ( *str == '0' )
   {
      /* Leading zero of a numeric or hex notation */
      str++;
      if (( *str == 'x' ) || ( *str == 'X' ))
      {
         /* Move off the x in 0x */
         str++;

         while (( *str != ' ' ) && ( *str != '\t' ) && ( *str != 0 ) && ( *str != ')' ))
         {
            if (( *str >= '0' ) && ( *str <= '9' ))
            {
               str++;
            }
            else if (( *str >= 'a' ) && ( *str <= 'f' ))
            {
               str++;
            }
            else if (( *str >= 'A' ) && ( *str <= 'F' ))
            {
               str++;
            }
            else
            {
               return(0);
            }
         }
         
         return((int)(str - start));
      }

      /* STUB: This is not required - Then remove it * /
      if (( *str >= '0' ) && ( *str <= '9' ))
      {
         while (( *str != ' ' ) && ( *str != '\t' ) && ( *str != 0 ) && ( *str != ')' ))
         {
            if (( *str >= '0' ) && ( *str <= '9' ))
            {
               str++;
            }
            else
            {
               return(0);
            }
         }
         
         return((int)(str - start));
      }
      */
   }

   if (( *str >= '0' ) && ( *str <= '9' ))
   {
      while (( *str >= '0' ) && ( *str <= '9' ))
      {
         /* DEBUG("%c", *str); */
         str++;
      }

      /* DEBUG("]\n"); */

      eat_ws(&str);

      /* DEBUG("switch(%d - %c)\n", (int)*str, *str); */

      switch(*str)
      {
      case 0:
      case ')':
      case '+':
      case '-':
      case '*':
      case '/':
         /* All valid chars */
         /* Do NOT move the string pointer forward */
         return((int)(str - start));
         break;
      default:
         /* Anything else is invalid */
         /* fprintf(stderr, "ERROR: Numeric value at \"%s\" contained illegal chars.\n", start); */
         /* No error message - fail silently */
         return(0);
         break;
      }

   }

   return(0);
}

/* ========================================================================= */
Entity *parse_token(int *move, char *str)
{
   Entity *newe = NULL;
   Expression *nexp = NULL;
   long num; /* Used in parse numeric */
   char *start = str;
   char tag[MAX_TAG_LEN];
   int lineno;

   lineno = 7; /* STUB: Hard-coded */

   DEBUG("Parsing token: \"%s\".\n", str);

   /* Set move to show that we have not parsed anything */
   *move = 0;

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '{' )
   {
      fprintf(stderr, "ERROR: Unsupported inline enum \"%s\" in line %d.\n", str, lineno);
      return(NULL);
   }
   else if ( *str == '(' )
   {
      if ( NULL == (nexp = parse_expr(move, str)) )
      {
         /* STUB: This likely does not need to exist */
         printf("ERROR: Problems parsing mathematical expression \"%s\" line %d.\n", str, lineno);
         return(NULL);
      }

      /* Create the entity - we have a valid expression */
      if ( NULL == ( newe = new_entity(str) ) )
      {
         /* Errors are printed at the point of failure. */
         return(NULL);
      }

      newe->type = ETYPE_MEXPR;
      newe->u.math = nexp;

      str += *move;

      return(newe);
   }
   else
   {
      *move = 0;

      if ( 0 != (*move = parse_numeric(&num, str)) )
      {
         DEBUG("DEBUG --- parse_numeric(%ld, %s) ---> %d\n", num, str, *move);

         str += *move;

         /* Walk off leading white space */
         eat_ws(&str);

         if (NULL == (newe = new_entity(start)))
         {
            /* new_entity() will print the error */
            return(NULL);
         }

         *move = (int)(str - start);
         newe->type = ETYPE_VALUE;
         newe->u.value = num;

         return(newe);
      }


      if ( 0 != (*move = parse_tag(tag, str)) )
      {
         DEBUG("DEBUG --- parse_tag(%s, %s) ---> %d\n", tag, str, *move);

         str += *move;

         /* Walk off leading white space */
         eat_ws(&str);

         if (NULL == (newe = new_entity(start)))
         {
            /* new_entity() will print the error */
            return(NULL);
         }

         *move = (int)(str - start);
         newe->type = ETYPE_TAGCP;
         newe->u.tag = (void *)nc_mkstring(tag);

         return(newe);
      }

      printf("ERROR: Problems parsing token \"%s\" line STUB.\n", str);
      return(NULL);
   }
   
   /* Unreachable */
   *move = 0;
   printf("ERROR: Please contact author because this code is broken.\n");
   return(NULL);
}

/* ========================================================================= */
Expression *new_expression(Entity *l_ent, int operation, Entity *r_ent)
{
   Expression *e;

   if (( NULL == l_ent ) || ( NULL == l_ent ) || ( NULL == l_ent ))
   {
      /* This is an "internal" error and should probably be an assert */
      fprintf(stderr, "ERROR: Invalid input when creating an expression.\n");
      return(NULL);
   }
      
   if ( NULL == ( e = (Expression *)malloc(sizeof(Expression)) ) )
   {
      fprintf(stderr, "ERROR: Failed to allocate memory for an expression.\n");
      return(NULL);
   }

   e->left      = l_ent;
   e->right     = r_ent;
   e->operation = operation;

   return(e);
}

/* ========================================================================= */
int validate_token(char *str)
{
   char *start = str;
   int move;

   /* Insure that we got something. The is the base of the parse. Check here. */
   if ( NULL == str )
      return(0);
      
   DEBUG("Validating token: \"%s\".\n", str);

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '{' )
   {
      fprintf(stderr, "ERROR: Unsupported inline enum \"%s\" in line STUB.\n", str);
      return(0);
   }
   else if ( *str == '(' )
   {
      if ( 0 == (move = validate_expr(str)) )
      {
         fprintf(stderr, "ERROR: Problems parsing expression \"%s\".\n", str);
         return(0);
      }

      str += move;
   }
   else
   {
      move = 0;

      if ( move == 0 )
         move = validate_numeric(str);

      if ( move == 0 )
         move = validate_tag(str);

      if ( move == 0 )
      {
         printf("ERROR: Unable to parse token \"%s\".\n", str);
         return(0);
      }
      else
         str += move;
   }
   
   /* Walk off trailing white space */
   eat_ws(&str);

   DEBUG("  validate_token() ---> %d\n", (int)(str - start));
   return((int)(str - start));
}

/* ========================================================================= */
Entity *new_entity(char *raw)
{
   Entity *e;

   if ( NULL == ( e = (Entity *)malloc(sizeof(Entity)) ) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for entity.\n");
      return(NULL);
   }

   e->raw = nc_mkstring(raw);
   e->type = ETYPE_NOTYP;
   e->u.tag = NULL; /* Null out the union */

   return(e);
}

/* ========================================================================= */
int parse_numeric(long *val, char *str)
{
   char *start = str;
   int isdec = 0;
   int isneg = 0;
   int lineno;

   lineno = 7; /* STUB: Hard-coded */

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '-' )
   {
      isdec = 1;
      isneg = 1;
      str++; /* Move off the '-' char */

      /* Walk off leading white space. Note: This could be an if(), but
         my fear is the extreme edge case of two spaces from the neg to the
         actual number. Technically any amount of white space should be fair
         but I am being somewhat limiting here. */
      eat_ws(&str);

      *val = 0;
      while (( *str >= '0' ) && ( *str <= '9' ))
      {
         *val *= 10;
         *val += ( *str - '0' );
         str++;
      }

      /* Apply the negative sign */
      if ( isneg )
         *val *= -1;

      eat_ws(&str);

      switch ( *str )
      {
      case 0:
      case ')':
      case '+':
      case '-':
      case '*':
      case '/':
         return((int)(str - start));
         break;
      default:
         fprintf(stderr, "ERROR: Problems parsing negative decimal \"%s\" at line %d.\n", start, lineno);
         return(0);
         break;
      }
   }

   if ( *str == '0' )
   {
      *val = 0;

      /* Leading zero of a numeric or hex notation */
      str++;
      if (( *str == 'x' ) || ( *str == 'X' ))
      {
         str++;

         while (( *str != ' ' ) && ( *str != '\t' ) && ( *str != 0 ) && ( *str != ')' ))
         {
            if (( *str >= '0' ) && ( *str <= '9' ))
            {
               *val *= 16;
               *val += (*str - '0');
               str++;
            }
            else if (( *str >= 'a' ) && ( *str <= 'f' ))
            {
               *val *= 16;
               *val += ((*str - 'a') + 10);
               str++;
            }
            else if (( *str >= 'A' ) && ( *str <= 'F' ))
            {
               *val *= 16;
               *val += ((*str - 'A') + 10);
               str++;
            }
            else
            {
               *val = 0;
               fprintf(stderr, "ERROR: Invalid characters in numeric \"%s\" line %d.\n", start, lineno);
               return(0);
            }
         }
         
         return((int)(str - start));
      }

      /* See if the leading zero was the *only* numerid (therefore zero) */
      if (( *str == ' ' ) || ( *str == '\t' ) || ( *str == 0 ) || ( *str == ')' ))
      {
         /* This is a leading zero - that IS zero */
         *val = 0;
         return((int)(str - start));
      }

      /* If it is a numeric with a leading zero, then fall through to next block */
   }

   if (( *str >= '0' ) && ( *str <= '9' ))
   {
      *val = 0;
      while (( *str >= '0' ) && ( *str <= '9' ))
      {
         *val *= 10;
         *val += ( *str - '0' );
         str++;
      }

      /* TCHNICALLY... if we got this far, we are ok. The numeric is complete
         once we hit white space. The one error condition is if the number is
         followed (immediately - without a space) by any invalid char. For example:
         ...3h      <----- Invalid 3h is not a number, nor a tag, etc...
         ...3+      <----- Could be a mathematical expression
         ...3 4     <----- Is invalid (because 4 has no context), but would NOT
                           be flagged as wrong here.
      */

      switch ( *str )
      {
      case ' ':
      case '\t':
         eat_ws(&str);
         return((int)(str - start));
         break;
      case 0:
      case ')':
      case '+':
      case '-':
      case '*':
      case '/':
         /* All valid chars */
         /* Do NOT move the string pointer forward */
         return((int)(str - start));
         break;
      default:
         /* Anything else is invalid */
         fprintf(stderr, "ERROR: Numeric value \"%s\" contained illegal chars on line %d.\n", start, lineno);
         return(0);
         break;
      }
   }

   return(0);
}

/* ========================================================================= */
int parse_tag(char *tag, char *str)
{
   char *start = str;

   /* Walk off leading white space */
   eat_ws(&str);

   while ( (( *str >= 'a' ) && ( *str <= 'z' )) ||
           (( *str >= 'A' ) && ( *str <= 'Z' )) ||
           (( *str >= '0' ) && ( *str <= '9' )) ||
           ( *str == '_' ) ||
           ( *str == '-' ) ||
           ( *str == '.' ))
   {
      *tag = *str;
      tag++;
      str++;
   }

   *tag = 0;

   return((int)(str - start));
}

/* ========================================================================= */
void DBG_dump_expression(int r, Expression *e)
{
   int i;

   i = 0;
   while ( i++ < r ) { printf(" "); }
   printf("Expression op = ");
   switch ( e->operation )
   {
   case EXPOP_ADD:
      printf("+\n");
      break;
   case EXPOP_SUB:
      printf("-\n");
      break;
   case EXPOP_MUL:
      printf("*\n");
      break;
   case EXPOP_DIV:
      printf("/");
      break;
   case EXPOP_NOP:
   default:
      printf("unknown\n");
      break;
   }

   i = 0;
   while ( i++ < r ) { printf(" "); }
   printf("Left entity:\n");
   DBG_dump_entity(r + 2, e->left);

   i = 0;
   while ( i++ < r ) { printf(" "); }
   printf("Right entity:\n");
   DBG_dump_entity(r + 2, e->right);
   
}

/* ========================================================================= */
void DBG_dump_entity(int r, Entity *e)
{
   int i;

   i = 0;
   while ( i++ < r ) { printf(" "); }
   printf("Entity is a ");
   switch ( e->type )
   {
   case ETYPE_VALUE:
      printf("number.\n");
      i = 0;
      while ( i++ < r + 2 ) { printf(" "); }
      printf("Value = %ld\n", e->u.value);
      break;
   case ETYPE_MEXPR:
      printf("mathematical expression.\n");
      DBG_dump_expression(r + 2, e->u.math);
      break;
   case ETYPE_TAGCP:
      printf("tag (unresolved).\n");
      i = 0;
      while ( i++ < r + 2 ) { printf(" "); }
      printf("Tag = %s\n", (char *)e->u.tag);
      break;
   case ETYPE_TAGRS:
      printf("tag (resolved).\n");
      i = 0;
      while ( i++ < r + 2 ) { printf(" "); }
      printf("Tag = ???\n");
      break;
   default:
      printf("unknown.\n");
      break;      
   }
}

/* ========================================================================= */
Entity *ParseEntity(char *str)
{
   Entity *e;
#ifdef STUB_REMOVE
   int rv;
#endif
   int move = 0;

   /* assert()-ish initial check */
   if ( NULL == str )
      return(NULL);

   if ( *str == 0 )
      return(NULL);

#ifdef STUB_REMOVE
   /* STUB: Validate it first? */
   if ( 0 == ( rv = validate_token(str) ) )
   {
      printf("Failed to parse entity at \"%s\".\n", str);
      return(NULL);
   }

   DEBUG("Validated good. >> Moved %d chars.\n", rv);
#endif

   if ( NULL == ( e = parse_token(&move, str) ) )
   {
      printf("ERROR: Unable to parse the token \"%s\".\n", str);
      return(NULL);
   }

   return(e);
}
