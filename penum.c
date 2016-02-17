#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "penum.h"
#include "strlib.h"


ENVP *parse_enum_pair(int *moved, char *estr, int lineno);

/* ========================================================================= */
int IsEnumLine(char *estr)
{
   /* Handle the enpty string case(s) */
   if ( NULL == estr )
      return(0);

   if ( 0 == estr[0] ) /* This is a bit redundant */
      return(0);
   
   /* This would be handled by the calling func, but make sure. */
   eat_ws(&estr);

   /* The tag string must begin with an expected pattern */

   /* The enum line must begin with an expected pattern */
   if ( estr != strstr(estr, "setenum") )
      return(0);

   /* Move off the directive */
   estr += 7;

   /* We gotta be on whitespace */
   if (( *estr != ' ' ) && ( *estr != '\t' ))
   {
      return(0);
   }

   /* STOP HERE!
      Why? Because extensive sniffing will just validate the entire line,
      see the syntax error, and report it as a skipped line. We want loose
      checking here so we drop into the heavier / well (error) messaged
      section of the parsing. This gives the user better messaging on a
      parsing failure. */
   return(1);
}


/* ========================================================================= */
Enum *ParseEnum(char *estr, int lineno)
{
   char enumtag[MAX_TAG_LEN + 1];
   int i;
   int lb;  /* Count of left brackets                                        */
   int rb;  /* Count of right brackets                                       */
   int eq;  /* Count of '=' characters                                       */
   int sq;  /* Count of single quote characters                              */
   int dq;  /* Count of double quote characters                              */
   
   Enum *eb;
   ENVP *nvp;
   int move;
   char *start = estr;
   char errmsg[12];

   /* Walk off leading white space */
   eat_ws(&estr);

   /* Just because I got tired of the test framework throwing errors on
      empty strings. Calling functions insure this is never an empty string. */
   if ( *estr == 0 )
      return(NULL); /* Fail silently - This should be unreachable in 
                       normal operations! */

   /* The enum line must begin with an expected pattern */
   if ( estr != strstr(estr, "setenum") )
   {
      /* The rule: All user-space errors get the "---" line treatment. They
         are syntax errors. Memory failures and what not get the more typical
         error message syntax. This particular item is both because it was
         sniffed for this string before it was sent here. Even the message is
         nonsense. This is really about establishing the error message type
         to be emulated - even though it is impossible to reach. */
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "setenum line begins with wrong setenum directive on line %d.\n", lineno);
      return(NULL);
   }

   /* Move off the directive */
   estr += 7;

      /* We gotta be on whitespace */
   if (( *estr != ' ' ) && ( *estr != '\t' ))
   {
      /* Technically... this is where the naive approach to line parsing falls apart.
         Here you cannot tell a reserved word from a reserved word fragment in a word.
         Had you pulled the line apart into individual tokens to be parsed, then you
         could do more *sane* pattern matching. */
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Syntax error on setenum directive parsing at \"%s\". Line %d.\n", three_dot_trunc(errmsg, 12, estr), lineno);
      return(NULL);

   }

   /* Now move off that whitespace */
   eat_ws(&estr);

   /* Copy off the tag */
   i = 0;
   while ( is_valid_tag_char(estr[i]) )
   {
      if ( i < MAX_TAG_LEN )
         enumtag[i] = estr[i];
      else
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum tag length on line %d exceeded maximum length of %d.\n", lineno, MAX_TAG_LEN);
         return(NULL);
      }

      i++;
   }

   /* Make sure that the length of the tag is sufficient */
   if ( i < 1 )
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Unable to parse enum tag on line %d.\n", lineno);
      return(NULL);
   }

   /* Make sure that we are on the expected character */
   if (( estr[i] == ' ' ) || ( estr[i] == '\t' ) || ( estr[i] == '=' ))
   {
      /* Terminate the tag. All is well */
      enumtag[i] = 0;
   }
   else
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Unexpected characters in enum tag on line number %d.\n", lineno);
      return(NULL);
   }

   /* Move our pointer up */
   estr += i;

   /* Check for expected characters (in remainder of string) */
   eq = 0;
   lb = 0;
   rb = 0;
   sq = 0;
   dq = 0;
   i = 0;
   while ( estr[i] != 0 )
   {
      if ( estr[i] == '{' )
         lb++;

      if ( estr[i] == '=' )
         eq++;

      if ( estr[i] == '}' )
         rb++;

      if ( estr[i] == '\"' )
         dq++;

      if ( estr[i] == '\'' )
         sq++;

      if ( estr[i] == '#' )
         break;

      i++;
   }

   if ( ( lb != 1 ) || ( rb != 1 ) || ( eq != 1 ) || ( dq % 2 != 0 ) || ( sq % 2 != 0 ) )
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Basic syntatical problem parsing line %d. Missing expected symbol elements.\n", lineno);
      return(NULL);
   }

   /* Walk off leading white space */
   eat_ws(&estr);

   /* Validate that we got an '=' */
   if ( *estr == '=' )
      estr++;
   else
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Missing equality \"=\" for enum on line %d.\n", lineno);
      return(NULL);
   }

   /* Walk off leading white space */
   eat_ws(&estr);

   /* Validate that we got an '{' */
   if ( *estr == '{' )
      estr++;
   else
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Missing left bracket for enum on line %d.\n", lineno);
      return(NULL);
   }
   
   if (NULL == (eb = (Enum *)malloc(sizeof(Enum))))
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for Enum structure.\n");
      return(NULL);
   }

   eb->raw = nc_mkstring(start);
   eb->tag = nc_mkstring(enumtag);
   eb->defval = NULL;
   eb->elist = NULL;

   move = 0;
   while ( NULL != ( nvp = parse_enum_pair(&move, estr, lineno) ) )
   {
      if ( nvp->next )
      {
         /* Check default before setting it */
         if ( eb->defval )
         {
            fprintf(stderr, "-------------------------------------------------------------------------------\n");
            fprintf(stderr, "Two default values were parsed from enum on line %d.\n", lineno);
            return(NULL);
         }

         eb->defval = nvp;
      }
      else
      {
         nvp->next = eb->elist;
         eb->elist = nvp;
      }

      estr += move;
   }

   return(eb);
}

/* ========================================================================= */
#define ENVP_NORMAL  0  /* This is a normal name/value pair                  */
#define ENVP_DEFAULT 1  /* This name value pair has the default value        */
ENVP *new_envp(int type)
{
   ENVP *ep;

   if ( NULL != ( ep = (ENVP *)malloc(sizeof(ENVP)) ) )
   {
      ep->value = 0;
      ep->name = NULL;
      ep->next = NULL;

      if ( type == ENVP_DEFAULT )
         ep->next = ep;
      else
         ep->next = NULL;
   }
   else
   {
      fprintf(stderr, "ERROR: Failed to allocate memory for enum structure.\n");
      /* Fall through to error (return NULL) */
   }

   return(ep);
}

/* ========================================================================= */
char *parse_envp_name(char *str)
{
   int i;
   char name[MAX_EVP_NAME_LEN + 1];
   char *rv;
      
   if ( NULL == str )
      return(NULL);

   if ( 0 == str[0] )
      return(NULL);

   /* STUB: Resolve this. Can I safely leave it? *Should* it be removed? */
   /* ---- Yank start ---- */
   /* Chomp off leading WS if it exists */
   eat_ws(&str);
      
   /* Should *not* start with a ':'. Earlier versions were a bit non-
      uniform in this respect. This stays mostly about legacy. This
      should not be required going forward. I have put comments where
      this block of code could be yanked. */
   if ( *str == ':' )
      str++;
   /* ---- Yank end ---- */

   /* Chomp off leading WS if it exists */
   eat_ws(&str);
      
   if ( *str == '\"' )
   {
      str++;

      i = 0;
      while(str[i] != '\"')
      {
         if ( i >= MAX_EVP_NAME_LEN )
            return(NULL);

         name[i] = str[i];
         i++;
      }

      name[i] = 0;

      rv = nc_mkstring(name);

      return(rv);
   }
   else
   {
      i = 0;
      while ((str[i] != ';') && (str[i] != ' ') && (str[i] != '}') && (str[i] != '\t'))
      {
         if ( i >= MAX_EVP_NAME_LEN )
            return(NULL);

         name[i] = str[i];
         i++;
      }

      name[i] = 0;

      rv = nc_mkstring(name);

      return(rv);
   }

   return(NULL);
}

/* STUB: WTF is this? */
   /*char value[MAX_TAG_LEN]; / * This value is not entirely appropriate, but safe */


/* ========================================================================= */
ENVP *parse_enum_pair(int *moved, char *estr, int lineno)
{
   ENVP *ep = NULL;
   BPInt value;
   char *start = estr;
   char errstr[16];
   
   if ( estr == NULL )
      return(ep);

   if ( estr[0] == 0 )
      return(ep);

   /* Walk off leading white space */
   eat_ws(&estr);

   if ( estr == strstr(estr, "default") )
   {
      /* I don't particularly like the above test, but it is valid. The
         word is case sensitive, and I did not intend to support WS here
         but I do. */
         
      estr += 7; /* Move off of default. Now on char after 'default' */

      eat_ws(&estr); /* Should not exist, but might */

      if ( *estr == ':' )
         estr++;
      else
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Problems parsing enum pair at \"%s\". Line %d.\n", three_dot_trunc(errstr, 16, start), lineno);
         return(NULL);
      }
      
      ep = new_envp(ENVP_DEFAULT);
      
      ep->name = parse_envp_name(estr);
      
      while ( ( *estr != ';' ) && ( *estr != '}' ) && ( *estr != 0 ) )
         estr++;
      
      if ( *estr == ';' )
         estr++;

      *moved = (int)(estr - start);

      return(ep);
   }


   if ( ( *estr >= '0' ) && ( *estr <= '9' ) )
   {
      value = 0;
      while ( ( *estr >= '0' ) && ( *estr <= '9' ) )
      {
         value *= 10;
         value += (*estr - '0');

         estr++;
      }

      /* Walk through leading WS */
      eat_ws(&estr);

      if ( *estr == ':' )
         estr++;
      else
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Problems parsing \"%s\". Line %d.\n", three_dot_trunc(errstr, 16, start), lineno);
         return(NULL);
      }

      ep = new_envp(ENVP_NORMAL);

      ep->name = parse_envp_name(estr);
      ep->value = value;

      while ( ( *estr != ';' ) && ( *estr != '}' ) && ( *estr != 0 ) )
         estr++;

      if ( *estr == ';' )
         estr++;

      *moved = (int)(estr - start);

      return(ep);
   }

   return(ep);
}

/* ========================================================================= */
Enum *new_enum(char *tag, char *raw)
{
   Enum *e;

   if (NULL == (e = (Enum *)malloc(sizeof(Enum))))
   {
      fprintf(stderr, "ERROR: Failed to allocate memory for base enum structure.\n");
      return(NULL);
   }

   if ( NULL != tag )
      e->tag = nc_mkstring(tag);
   else
      e->tag = NULL;

   if ( NULL != raw )
      e->raw = nc_mkstring(raw);
   else
      e->raw = NULL;

   e->defval = NULL;  /* Pointer to default value ENVP */
   e->elist = NULL;   /* Linked list of non-default ENVPs */
   e->next = NULL;    /* Linked list of Enums (a greater thing (not a "sub-"thing)) */

   return(e);
}

/* ========================================================================= */
int ApplyBuiltins(RuleSet *rs)
{
   Enum *e;

   assert(NULL != rs); /* This should never be NULL */

   /* There are three builtins (at this time). They are largely copies of
      each other (variants of each other). I put comments on the first 
      builtin, and just code for the other two. */


   /*** enumBool = True/False ***/
   if ( NULL == ( e = new_enum("enumBool", "enumBool = { 0:False; default:True; }") ) )
      return(1);

   if ( NULL == ( e->defval = new_envp(ENVP_DEFAULT) ) )
      return(1); /* Error msg is at point of failure. */
   /* don't set the value for a default value ENVP */
   e->defval->name = nc_mkstring("True");

   if ( NULL == ( e->elist = new_envp(ENVP_NORMAL) ) )
      return(1); /* Error msg is at point of failure. */
   e->elist->value = 0;
   e->elist->name = nc_mkstring("False");

   /* Into the linked list */
   e->next = rs->belist;
   rs->belist = e;

   /*** enumBOOL = TRUE/FALSE ***/
   if ( NULL == ( e = new_enum("enumBOOL", "enumBOOL = { 0:FALSE; default:TRUE; }") ) )
      return(1);

   if ( NULL == ( e->defval = new_envp(ENVP_DEFAULT) ) )
      return(1);
   e->defval->name = nc_mkstring("TRUE");

   if ( NULL == ( e->elist = new_envp(ENVP_NORMAL) ) )
      return(1);
   e->elist->value = 0;
   e->elist->name = nc_mkstring("FALSE");

   e->next = rs->belist;
   rs->belist = e;

   /*** enumbool = true/false ***/
   if ( NULL == ( e = new_enum("enumbool", "enumbool = { 0:false; default:true; }") ) )
      return(1);

   if ( NULL == ( e->defval = new_envp(ENVP_DEFAULT) ) )
      return(1);
   e->defval->name = nc_mkstring("true");

   if ( NULL == ( e->elist = new_envp(ENVP_NORMAL) ) )
      return(1);
   e->elist->value = 0;
   e->elist->name = nc_mkstring("false");

   e->next = rs->belist;
   rs->belist = e;

   return(0);
}
