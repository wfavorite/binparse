#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "penum.h"
#include "strlib.h"


ENVP *parse_enum_pair(int *moved, char *estr);

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
   if (( estr[0] != 'e' ) ||
       ( estr[1] != 'n' ) ||
       ( estr[2] != 'u' ) ||
       ( estr[3] != 'm' ))
   {
      return(0);
   }

   /* Move ourselves off the tag */
   while ( (( *estr >= 'a' ) && ( *estr <= 'z' )) || 
           (( *estr >= 'A' ) && ( *estr <= 'Z' )) || 
           (( *estr >= '0' ) && ( *estr <= '9' )) ||
           (*estr == '-' ) || (*estr == '_' ) || (*estr == '.' ) )
   {
      estr++;
   }

   /* There may be white space */
   eat_ws(&estr);

   /* The next MUST be an '=' */
   if ( *estr != '=' )
   {
      return(0);
   }

   /* We are on an "=". Move off. */
   estr++;

   /* There may be white space */
   eat_ws(&estr);

   /* The next MUST be an '{' */
   if ( *estr != '{' )
   {
      return(0);
   }

   /* This line may not be valid... but we have tested (sniffed) enough
      to know that it likely is (or was *intended* to be). So let's
      consider this an enum line. */
   return(1);
}


/* ========================================================================= */
Enum *ParseEnum(char *estr)
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

   /* Walk off leading white space */
   eat_ws(&estr);

   /* Just because I got tired of the test framework throwing errors on empty strings */
   if ( *estr == 0 )
      return(NULL); /* Fail silently - This should be unreachable in normal ops! */

   /* The tag string must begin with an expected pattern */
   if (( estr[0] != 'e' ) ||
       ( estr[1] != 'n' ) ||
       ( estr[2] != 'u' ) ||
       ( estr[3] != 'm' ))
   {
      fprintf(stderr, "ERROR: Enum line begins with improper tag name.\n");
      return(NULL);
   }

   /* Copy off the tag */
   i = 0;
   while ( (( estr[i] >= 'a' ) && ( estr[i] <= 'z' )) || 
           (( estr[i] >= 'A' ) && ( estr[i] <= 'Z' )) || 
           (( estr[i] >= '0' ) && ( estr[i] <= '9' )) ||
           (estr[i] == '-' ) || (estr[i] == '_' ) || (estr[i] == '.' ) )
   {
      if ( i < MAX_TAG_LEN )
         enumtag[i] = estr[i];
      else
      {
         fprintf(stderr, "ERROR: Enum tag length on line STUB exceeded maximum length of %d.\n", MAX_TAG_LEN);
         return(NULL);
      }

      i++;
   }

   /* Make sure that the length of the tag is sufficient */
   if ( i < 5 )
   {
      /* The enum tag is too short */
      fprintf(stderr, "ERROR: The tag used for the enum on line STUB is too short.\n");
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
      fprintf(stderr, "ERROR: Unexpected characters in enum tag on line number STUB.\n");
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
      fprintf(stderr, "ERROR: Basic syntatical problem parsing line STUB.\n");
      return(NULL);
   }

   /* Walk off leading white space */
   eat_ws(&estr);

   /* Validate that we got an '=' */
   if ( *estr == '=' )
      estr++;
   else
   {
      fprintf(stderr, "ERROR: Missing equality for enum on line STUB.\n");
      return(NULL);
   }

   /* Walk off leading white space */
   eat_ws(&estr);

   /* Validate that we got an '{' */
   if ( *estr == '{' )
      estr++;
   else
   {
      fprintf(stderr, "ERROR: Missing left bracket for enum on line STUB.\n");
      return(NULL);
   }
   
   if (NULL == (eb = (Enum *)malloc(sizeof(Enum))))
   {
      return(NULL);
   }

   eb->raw = nc_mkstring(start);
   eb->tag = nc_mkstring(enumtag);
   eb->defval = NULL;
   eb->elist = NULL;

   move = 0;
   while ( NULL != ( nvp = parse_enum_pair(&move, estr) ) )
   {
      /* STUB: printf("DEBUG: Parsed an enum pair.\n"); */

      if ( nvp->next )
      {
         /* Check default before setting it */
         if ( eb->defval )
         {
            fprintf(stderr, "ERROR: Two default values were parsed from enum on line STUB.\n");
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

   /* STUB: fprintf(stderr, "parse_envp_name(%s);\n", str); */
      
   if ( NULL == str )
      return(NULL);

   if ( 0 == str[0] )
      return(NULL);

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

      /* STUB: The old
      if ( NULL == ( rv = malloc(i + 1) ) )
         return(NULL);

      strcpy(rv, name);
      */

      return(rv);
   }
   else
   {
      i = 0;
      while ((str[i] != ';') && (str[i] != ' ') && (str[i] != '}') && (str[i] != '\t'))
      {
         /* STUB: What is the risk we will run off the end of the string? */

         if ( i >= MAX_EVP_NAME_LEN )
            return(NULL);

         name[i] = str[i];
         i++;
      }

      name[i] = 0;

      rv = nc_mkstring(name);

      /* STUB: The old
      if ( NULL == ( rv = malloc(i + 1) ) )
         return(NULL);

      strcpy(rv, name);
      */

      return(rv);
   }

   return(NULL);
}

/* ========================================================================= */
/* STUB: This should have more meaningful messages. The message should contain
   either the enum number, or the string itself. Potentially both. */
ENVP *parse_enum_pair(int *moved, char *estr)
{
   ENVP *ep = NULL;
   /*char value[MAX_TAG_LEN]; / * This value is not entirely appropriate, but safe */
   EVType value;
   char *start = estr;

   if ( estr == NULL )
      return(ep);

   if ( estr[0] == 0 )
      return(ep);

   /* Walk off leading white space */
   while ((*estr == ' ')||(*estr == '\t'))
      estr++;

   /*
   if ( *estr == 'd' )
   {
   */
   if (( estr[0] == 'd' ) &&
       ( estr[1] == 'e' ) &&
       ( estr[2] == 'f' ) &&
       ( estr[3] == 'a' ) &&
       ( estr[4] == 'u' ) &&
       ( estr[5] == 'l' ) &&
       ( estr[6] == 't' ))
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
         fprintf(stderr, "ERROR: Problems parsing enum pair at \"%s\". Line STUB.\n", start);
         return(NULL);
      }
      
      ep = new_envp(ENVP_DEFAULT);
      
      ep->name = parse_envp_name(estr);
      
      /* STUB: printf("DEBUG: default --> %s\n", ep->name); */
      
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
         fprintf(stderr, "ERROR: Problems parsing \"%s\" at line STUB.\n", start);
         return(NULL);
      }

      ep = new_envp(ENVP_NORMAL);

      ep->name = parse_envp_name(estr);
      ep->value = value;

      /* STUB: printf("DEBUG: %d --> %s\n", ep->value, ep->name); */

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
int InsertEnum(RuleSet *rs, Enum *e)
{
   Enum *thise;

   assert(NULL != rs);
   assert(NULL != e);

   thise = rs->elist;
   while(thise)
   {
      if ( 0 == strcmp(e->tag, thise->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum naming collision. Two enums are called \"%s\".\n", e->tag);
         return(1);
      }

      thise = thise->next;
   }

   /* If we made it here, then we are unique (on the user-defined list) */
   e->next = rs->elist;
   rs->elist = e;

   return(0);
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
