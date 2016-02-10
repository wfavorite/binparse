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
Expression *parse_expr(int *moverv, char *str, int lineno);
Entity *parse_token(int *move, char *str, int lineno);
int parse_numeric(long *val, char *str, int lineno);
int parse_tag(char *tag, char *str);

/* ========================================================================= */
Expression *parse_expr(int *moverv, char *str, int lineno)
{
   Expression *exp;

   /* Parts required for the Expression */
   Entity *l_ent;
   Entity *r_ent;
   int operation = EXPOP_NOP;
   char *start = str;
   int move;
   char errmsg[24];

   DEBUG("Parsing expression: \"%s\".\n", str);

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str != '(' )
   {
      /* This should be sniffed. It should never fire. (You can't get here, because this is pre-checked */
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Failed to recognize the beginning of expression \"%s\" at line %d.\n", three_dot_trunc(errmsg, 16, str), lineno);
      return(NULL);
   }

   str++; /* Move off ( char */

   /* Walk off leading white space */
   eat_ws(&str);

   /* Parse the first token */
   if ( NULL == ( l_ent = parse_token(&move, str, lineno) ) )
   {
      /* parse_token() never leaves without an error */
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
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Operation function not parsed at \"%s\" line %d.\n", three_dot_trunc(errmsg, 16, start), lineno);
      return(NULL);
      break;
   }

   /* Walk off leading white space */
   eat_ws(&str);

   if ( 0 == ( r_ent = parse_token(&move, str, lineno) ) )
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
   fprintf(stderr, "-------------------------------------------------------------------------------\n");
   fprintf(stderr, "Failed to find terminating condition for expr \"%s\" on line %d.\n",
           three_dot_trunc(errmsg, 16, str), lineno);
   return(NULL);
}


/* ========================================================================= */
Entity *parse_token(int *move, char *str, int lineno)
{
   Entity *newe = NULL;
   Expression *nexp = NULL;
   long num; /* Used in parse numeric */
   char *start = str;
   char tag[MAX_TAG_LEN];
   int moverv;
   char errmsg[24];

   DEBUG("Parsing token: \"%s\".\n", str);

   /* Set move to show that we have not parsed anything */
   *move = 0;

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '{' )
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Unsupported inline enum \"%s\" in line %d.\n", three_dot_trunc(errmsg, 24, str), lineno);
      return(NULL);
   }
   else if ( *str == '(' )
   {
      if ( NULL == (nexp = parse_expr(move, str, lineno)) )
      {
         /* Errors are in parse_expr() */
         /*
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Problems parsing mathematical expression \"%s\" line %d.\n", three_dot_trunc(errmsg, 18, str), lineno);
         */
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
      moverv = 0;
      

      if ( 0 != (moverv = parse_numeric(&num, str, lineno)) )
      {
         if ( moverv == -1 )
         {
            /* Was an error. Message was printed. Exit quietly. */
            return(NULL);
         }

         *move = moverv;

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


      if ( 0 != (moverv = parse_tag(tag, str)) )
      {
         if ( moverv == -1 )
         {
            /* Was an error. Message was printed. Exit quietly. */
            return(NULL);
         }

         *move = moverv;

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

      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Problems parsing token \"%s\" line %d.\n", three_dot_trunc(errmsg, 24, str), lineno);
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
int parse_numeric(long *val, char *str, int lineno)
{
   char *start = str;
   int isneg = 0;

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '-' )
   {
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
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Problems parsing negative decimal \"%s\" at line %d.\n", start, lineno);
         return(-1);
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
               fprintf(stderr, "-------------------------------------------------------------------------------\n");
               fprintf(stderr, "Invalid character \"%c\" in hex \"%s\" line %d.\n", *str, start, lineno);
               return(-1);
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
         /* Anything else is invalid - Don't begin a tag with a numeric char */
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Numeric value \"%s\" contained illegal char \"%c\" on line %d.\n", start, *str, lineno);
         return(-1);
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

   if (( *str != ' ' ) && ( *str != '\t' ) && ( *str != ')' ) && ( *str != 0 ))
      return(0);

   *tag = 0;

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
   case ETYPE_TAGPP:
      printf("tag (resolved to parse point).\n");
      i = 0;
      while ( i++ < r + 2 ) { printf(" "); }
      printf("Tag = ???\n");
      break;
   case ETYPE_TAGET:
      printf("tag (resolved to explicit tag).\n");
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
Entity *ParseEntity(char *str, int lineno)
{
   Entity *e;
   int move = 0;

   /* assert()-ish initial check */
   if ( NULL == str )
      return(NULL);

   if ( *str == 0 )
      return(NULL);

   if ( NULL == ( e = parse_token(&move, str, lineno) ) )
   {
      /* Error handled in parse_token */
      /* printf("ERROR: Unable to parse the token \"%s\" line number %d.\n", str, lineno); */
      return(NULL);
   }

   return(e);
}




