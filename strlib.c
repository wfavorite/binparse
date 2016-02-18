#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "strlib.h"

/* ========================================================================= */
char *mkstring(const char *input)
{
   char *str;
   unsigned long len;

   if ( NULL == input ) 
   {
      len = 4;
   }
   else
   {
      /* Round to a happy padding number */
      if ( 4 > (len = strlen(input) + 1) )
         len = 4;
   }

   if ( NULL == (str = malloc(len)) )
      return(NULL);

   if ( NULL == input )
      str[0] = 0;
   else
      strcpy(str, input);

   return(str);
}

/* ========================================================================= */
char *nc_mkstring(const char *input)
{
   char *str;

   if ( NULL == ( str = mkstring(input) ) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for a string.\n");
      exit(1);
   }

   return(str);
}

/* ========================================================================= */
char *mid_trunc(char *str, const char *input, unsigned long len)
{
   unsigned long ilen;
   unsigned long middle;
   unsigned long i, j;
   char defstr[4] = "---";
  
   assert(len > 8);

   /* Check input - is it valid? */
   if (( input == NULL ) || ( input[0] == 0) )
   {
      if ( NULL == str )
      {
         return(mkstring(defstr));
      }
      else
      {
         strcpy(str, defstr);
         return(str);
      }
   }
  
   ilen = strlen(input);

   /* Does the input need to be truncated? */
   if ( ilen <= len )
   {
      /* No need to truncate */
      if ( NULL == str )
         return(mkstring(input));
      else
      {
         strcpy(str, input);
         return(str);
      }
   }
   else
   {
      /* input needs to be truncated */

      if ( NULL == str )
      {
         /* Allocate a string - input was NULL */
         if ( NULL == (str = (char *)malloc(len + 1)) )
            return(NULL);
      }

      /* Math look wonky? Because I favor the trailing part */
      middle = (len / 2) - 3;

      /* Copy over the head of the string */
      i = 0;
      while ( i < middle )
      {
         str[i] = input[i];
         i++;
      }

      str[i++] = '.';
      str[i++] = '.';
      str[i++] = '.';

      j = ilen - (len / 2 );
    
      while ( i < len )
      {
         str[i] = input[j];
         i++;
         j++;
      }

      /* Terminate */
      str[i] = 0;
      return(str);
   }
}

/* ========================================================================= */
int hash_trunc(char *istr)
{
   int i;
   int qc = 0;   /* Number of ' before hash */
   int qct = 0;  /* Total number of ' in line */
   int qqc = 0;  /* Number of " before hash */
   int qqct = 0; /* Total number of " in line */
   int fh = 0;   /* location of first hash */

   /* NULL input - line not modified */
   if ( NULL == istr )
      return(0);

   /* Empty string - line not modified */
   if ( istr[0] == 0 )
      return(0);

   /* Line begins with hash - line truncated */
   if ( istr[0] == '#' )
   {
      istr[0] = 0;
      return(1);
   }

   i = 0;
   while( istr[i] != 0 )
   {
      switch(istr[i])
      {
      case '#':
         if ( fh == 0 )
            fh = i;
         break;
      case '\'':
         qct++;
         if ( fh == 0 )
            qc++;
         break;
      case '\"':
         qqct++;
         if ( fh == 0 )
            qqc++;
         break;
      }

      i++;
   }
      
   if (( qqc % 2 ) && ( 0 == qqct % 2 ))
   {
      /* Hash is in a quote */
      return(0);
   }

   /* STUB: This may be a bug! Single tick may be a "...'s" string */
   if (( qc % 2 ) && ( 0 == qct % 2 ))
   {
      /* Hash is in a (single) quote */
      return(0);
   }

   if ( fh > 0 )
   {
      istr[fh] = 0;
      return(1);
   }
  
   return(0);
}

/* ========================================================================= */
int ws_trunc(char *istr)
{
   int i;
   int rv = 0;

   /* NULL input - line not modified */
   if ( NULL == istr )
      return(0);

   /* Empty string - line not modified */
   if ( istr[0] == 0 )
      return(0);

   i = strlen(istr) - 1;

   while((istr[i] == ' ') || (istr[i] == '\t'))
   {
      istr[i] = 0;

      rv = 1;
      i--;

      if ( i < 0 )
         return(1);
   }

   return(rv);
}

/* ========================================================================= */
char *leadingwst(char *istr)
{
   if ( NULL == istr )
      return(istr);

   if ( istr[0] == 0 )
      return(istr);

   while((*istr == ' ') || (*istr == '\t'))
      istr++;

   return(istr);
}

/* ========================================================================= */
char *chomp(char *istr)
{
   unsigned long last;
  
   if (NULL == istr)
      return(NULL);

   if (istr[0] == 0)
      return(istr);

   last = strlen(istr) - 1;
   
   while((istr[last] == '\n')|| (istr[last] == '\r'))
   {
      istr[last--] = 0;
   }

   return(istr);
}

/* ========================================================================= */
int ishex(char *istr)
{
   int mspace = 0; /* WS encountered after we started parsing the number */
  
   /* Wade through any leading white space */
   while(( *istr == ' ' ) || ( *istr == '\t' ))
      istr++;

   if ( *istr != '0' )
      return(0);

   istr++;
  
   if (( *istr != 'x' ) && ( *istr != 'X' ))
      return(0);

   istr++;
  
   while( *istr != 0 )
   {
      switch( *istr )
      {
         /* Lame-O, but should be clear/one-statement */
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'a':
      case 'A':
      case 'b':
      case 'B':
      case 'c':
      case 'C':
      case 'd':
      case 'D':
      case 'e':
      case 'E':
      case 'f':
      case 'F':
         if ( mspace )
            return(0);
         break;
      case ' ':
      case '\t':
         mspace++;
         break;
      default:
         return(0);
         break;
      }

      istr++;
   }

   return(1);
}

/* ========================================================================= */
int ispnumeric(char *istr)
{
   /* Wade through any leading white space */
   while(( *istr == ' ' ) || ( *istr == '\t' ))
      istr++;

   while( *istr != 0 )
   {
      if ((*istr < '0')||(*istr > '9'))
         return(0);
   }
      
   return(1);
}

/* ========================================================================= */
char *nth_token_location(char *istr, int n)
{
   int i;
   int t;
   char qterm;
   int bcount; /* Boundary/Bracket count */

   /* If 0th token, then return the string itself */
   if ( n <= 0 )
      return(istr);

   i = 0;
   t = 0;
   while(istr[i] != 0 )
   {
      /* Skip leading white space */
      while((istr[i] == ' ') || (istr[i] == '\t'))
         i++;

      /* fprintf(stderr, "STUB DEBUG: TopOfLoop(%s, %d);\n", &istr[i], n); */

      if ((istr[i] != 0)&&(istr[i] != '\n')&&(istr[i] != '\r')) 
      {
         t++; /* We are on a new token */

         if ( n == t )
         {
            /* fprintf(stderr, "STUB DEBUG: Nth_token_location(%s, %d);\n", &istr[i], n); */
            return(&istr[i]);
         }

         if ( istr[i] == '(' )
         {
            i++; /* Step off the opening bracket */
            bcount = 1;

            while (( 0 != bcount ) && ( istr[i] != 0 ))
            {
               switch(istr[i])
               {
               case ')':
                  bcount--;
                  break;
               case '(':
                  bcount++;
                  break;
               }

               i++;
            }
         }
         else if (( istr[i] == '\"' ) || ( istr[i] == '\'' ))
         {
            qterm = istr[i];
	
            i++; /* Step over the starting quote */

            /* Step through the quoted token */
            while((istr[i] != qterm) && (istr[i] != 0))
               i++;
            
            /* Step off the end quote */
            if ( istr[i] == qterm )
               i++;
         }
         else
         {
            /* Step through the non-quoted token */
            while((istr[i] != ' ') && (istr[i] != '\t') && (istr[i] != 0 ))
               i++;
         }
      }
      
   }
   return(NULL);
}

/* ========================================================================= */
int copy_out_nth_token(char *tostr, unsigned long len, char *istr, int n)
{
   int i;
   int bcount;
   char *token;
   char qterm;

   if ( n <= 0 )
      return(1);
  
   if (NULL == (token = nth_token_location(istr, n)))
      return(1);

   if (( token[0] == '\"' ) || ( token[0] == '\'' ))
   {
      qterm = token[0];
      
      i = 0;    
      while((token[i + 1] != qterm) && (token[i + 1] != 0))
      {
         tostr[i] = token[i + 1];
         
         i++;
         
         if ( i >= len )
         {
            tostr[i] = 0;
            return(1);
         }
      }
      
      tostr[i] = 0;
   }
   else if ( token[0] == '(' )
   {
      i = 0;

      /* Copy out the leading bracket, and account for it. */
      bcount = 1;
      tostr[i] = token[i];
      i++;
      
      while (( 0 != bcount ) && ( token[i] != 0 ))
      {
         switch(token[i])
         {
         case ')':
            bcount--;
            break;
         case '(':
            bcount++;
            break;
         }
         
         tostr[i] = token[i];
         i++;
      }
      tostr[i] = 0;
   }
   else
   {
      i = 0;
      while((token[i] != ' ') && (token[i] != 0) && (token[i] != '\t'))
      {
         tostr[i] = token[i];
         
         i++;
         
         if ( i >= len )
         {
            tostr[i] = 0;
            return(1);
         }
      }
      
      tostr[i] = 0;
   }
   return(0);
}

/* ========================================================================= */
int isanynum(char *str)
{
   /* Eat up WS */
   while((*str == ' ')||(*str == '\t'))
      str++;

   if ( *str == '-' )
   {
      /* Negative decimal */
      str++;
    
      /* Eat up WS */
      while((*str == ' ')||(*str == '\t'))
         str++;

      /* Walk over the number */
      while((*str >= '0')&&(*str <= '9'))
         str++;

      /* Check to see what is here */
      if ((*str == 0)||(*str == ' ')||(*str == '\t'))
      {
         /* Properly terminated negative decimal */
         return(1);
      }
   }

   if ( *str == '0' )
   {
      str++;
      
      if (( *str == 'x' )||( *str == 'X' ))
      {
         /* Hex */
         str++;

         /* Ugh... Such hackery */
         /* The first char must be a numeric. (You gotta have at least one) */
         switch(*str)
         {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
         case 'a':
         case 'A':
         case 'b':
         case 'B':
         case 'c':
         case 'C':
         case 'd':
         case 'D':
         case 'e':
         case 'E':
         case 'f':
         case 'F':
            str++;
            break;
         default:
            return(0);
            break;
         }
         
         /* Now 0 or more hex chars */
         while((*str != 0) && (*str != ' ') && (*str != '\t'))
         {
            switch(*str)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'a':
            case 'A':
            case 'b':
            case 'B':
            case 'c':
            case 'C':
            case 'd':
            case 'D':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
               str++;
               break;
            default:
               return(0);
               break;
            }
         }

         /* Gotta be a terminating char to get here */
         return(1);
      }
   } /* if ( str[0] == '0' ) */
   
   if (( *str >= '0' )||( *str <= '9' ))
   {
      /* Dec */
      while(( *str != 0 ) && ( *str != ' ' ) && ( *str != '\t' ))
      {
         if (( *str < '0' ) || ( *str > '9' ))
            return(0);
         
         str++;
      }

      /* If we got here, then we hit a terminating char, and we know we
         have at least one numeric char. */
      return(1);
   }
   
   return(0);
}

/* ========================================================================= */
int str_to_uint32t(uint32_t *rval, char *str)
{
   uint32_t val = 0;
   int pccnt = 0; /* Parsed Character Count */
  
   /* Strip off leading white space */
   while ( ( *str == ' ' ) && ( *str == '\t' ) )
      str++;

   if ( *str == '0' )
   {
      str++;

      if (( *str == 'x' ) || ( *str == 'X' ))
      {
         /* Hex */
         str++;

         while(( *str != 0 ) && ( *str != ' ' ) && ( *str != '\t' ))
         {
            switch(*str)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               val *= 16;
               val += *str - '0';
               break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
               val *= 16;
               val += *str - ('a' + 10);
               break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
               val *= 16;
               val += *str - ('A' + 10);
               break;
            default:
               return(1);
               break;
            }

            /* If we got here, then we successfully parsed our char. Now count the parse. */
            pccnt++;
            if(pccnt > 16)
               return(1);

            str++;
         }

         /* Iffen we got here, then it is ok */
         *rval = val;
         return(0);
      }
   }
 
   if (( *str >= '0' )||( *str <= '9' ))
   {
      /* Dec */
      while(( *str != 0 ) && ( *str != ' ' ) && ( *str != '\t' ))
      {
         if (( *str >= '0' ) && ( *str <= '9' ))
         {
            val *= 10;
            if ( val > (UINT32_MAX - 10) )
               return(1);
            val += (*str - '0');
         }
         else
            return(1);

         str++;
      }

      /* Parsed correctly if here */
      *rval = val;
      return(0);
   }

   /* Parse attempts failed. */
   return(1);
}

/* ========================================================================= */
void eat_ws(char **shand)
{
   /* Without the local copy, the compiler complained
      that the value was never used. It was pass by
      reference, and the C compiler did not figure that
      out. */
   char *str;

   if ( NULL == (str = *shand) )
      return;

   /* Walk off leading white space */
   while ((*str == ' ')||(*str == '\t'))
      str++;

   *shand = str;
}

/* ========================================================================= */
int is_valid_tag_char(char x)
{
   if ( (( x >= 'a' ) && ( x <= 'z' )) ||
        (( x >= 'A' ) && ( x <= 'Z' )) || 
        (( x >= '0' ) && ( x <= '9' )) ||
        ( x == '-' ) || ( x == '_' ) || ( x == '.' ) )
   {
      return(1);
   }

   return(0);
}

/* ========================================================================= */
char *three_dot_trunc(char *fill, int fill_len, char *source)
{
   int i;

   /* Fail silently when used inappropriately. */
   if ( NULL == fill )
      return(fill);

   /* Handle empty input strings */
   if ( NULL == source )
   {
      fill[0] = 0;
      return(fill);
   }

   if ( 0 == source[0] )
   {
      fill[0] = 0;
      return(fill);
   }

   /* Just make this an error - really an assert. The question is how
      to handle it. Here we terminate the string and return it. It introduces
      a potential visual error, but not a crash (by returning NULL). */
   if (fill_len < 3)
   {
      fill[0] = 0; /* Gotta have a length of one */
      return(NULL);
   }

   i = 0;
   while ( ( source[i] != 0 ) && ( i < fill_len ) )
   {
      fill[i] = source[i];
      i++;
   }

   /* Terminate */
   fill[i] = 0;

   if ( i < fill_len )
      return(fill);

   fill[fill_len - 0] = 0;
   fill[fill_len - 1] = '.'; 
   fill[fill_len - 2] = '.'; 
   fill[fill_len - 3] = '.'; 

   return(fill);
}


/* ========================================================================= */
int conv_to_uc(char *cpstr)
{
   if ( NULL == cpstr )
      return(1);

   while ( *cpstr != 0 )
   {
      if (( *cpstr >= 'a' ) && ( *cpstr <= 'z' ))
         *cpstr -= 32;

      cpstr++;
   }

   return(0);
}

