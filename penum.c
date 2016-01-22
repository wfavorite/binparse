#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "penum.h"
#include "support.h"


ENVP *parse_enum_pair(int *moved, char *estr);



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

   eb->raw = mkstring(start);
   eb->tag = mkstring(enumtag);
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
#define ENVP_NORMAL  0
#define ENVP_DEFAULT 1
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
   /* STUB: Should we {}else{FAIL}? */

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
      /* rv = mkstring(name); */
      if ( NULL == ( rv = malloc(i + 1) ) )
         return(NULL);

      strcpy(rv, name);

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
      /* rv = mkstring(name); */
      if ( NULL == ( rv = malloc(i + 1) ) )
         return(NULL);

      strcpy(rv, name);

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
