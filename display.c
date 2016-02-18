#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "display.h"

/* ========================================================================= */
int print_pp_enum(ParsePoint *pp, Options *o)
{
   ENVP *this_envp;

   this_envp = pp->use_enum->elist;
   while ( this_envp )
   {
      if ( pp->rdata == this_envp->value )
      {
         printf("%s\n", this_envp->name);
         return(0);
      }

      this_envp = this_envp->next;
   }
   
   /* Not found */
   printf("%s\n", pp->use_enum->defval->name);
   return(0);
}


/* ========================================================================= */
int print_fl_string(ParsePoint *pp, Options *o)
{
   BPInt i;
   char *cp;

   cp = pp->data;

   /* If hex, then the proper leading value */
   switch ( o->eDumpHex )
   {
   case OUTPUT_HEX_UC:
      printf("0X");
      break;
   case OUTPUT_HEX_LC:
      printf("0x");
      break;
   }

   /* Print out the string */
   i = 0;
   while( i < pp->rSize )
   {
      switch ( o->eDumpHex )
      {
      case OUTPUT_DEC:
         if ( isprint(cp[i]) )
            printf("%c", cp[i]);
         else
            printf(".");
         break;
      case OUTPUT_HEX_UC:
         printf("%02X", (unsigned char)cp[i]);
         break;
      case OUTPUT_HEX_LC:
         printf("%02x", (unsigned char)cp[i]);
         break;
      }
      
      i++;
   }

   printf("\n");
   fflush(stdout);

   return(0);
}

/* ========================================================================= */
int print_zt_string(ParsePoint *pp, Options *o)
{
   BPInt i;
   char *cp;

   cp = pp->data;

   /* If hex, then the proper leading value */
   switch ( o->eDumpHex )
   {
   case OUTPUT_HEX_UC:
      printf("0X");
      break;
   case OUTPUT_HEX_LC:
      printf("0x");
      break;
   }

   /* Print out the string */
   i = 0;
   while( 0 != cp[i] )
   {
      switch ( o->eDumpHex )
      {
      case OUTPUT_DEC:
         if ( isprint(cp[i]) )
            printf("%c", cp[i]);
         else
            printf(".");
         break;
      case OUTPUT_HEX_UC:
         printf("%02X", (unsigned char)cp[i]);
         break;
      case OUTPUT_HEX_LC:
         printf("%02x", (unsigned char)cp[i]);
         break;
      }
      
      i++;
   }

   printf("\n");

   return(0);
}

/* ========================================================================= */
int print_datatype(ParsePoint *pp, Options *o)
{
   /* Get the string types out of the way */
   if ( pp->dt == DT_ZTSTR )
      return(print_zt_string(pp, o));

   if ( pp->dt == DT_FLSTR )
      return(print_fl_string(pp, o));

   if ( pp->use_enum )
      return(print_pp_enum(pp, o));

   if ( OUTPUT_DEC == o->eDumpHex )
   {
      switch ( pp->dt )
      {
      case DT_CHAR:
         printf("%c\n", (char)(*(char *)pp->data));
         break;
      case DT_UINT8:
         printf("%u\n", (uint8_t)(*(uint8_t *)pp->data));
         break;
      case DT_INT8:
         printf("%d\n", (int8_t)(*(int8_t *)pp->data));
         break;
      case DT_UINT16:
         printf("%u\n", (uint16_t)(*(uint16_t *)pp->data));
         break;
      case DT_INT16:
         printf("%d\n", (int16_t)(*(int16_t *)pp->data));
         break;
      case DT_UINT32:
         printf("%u\n", (uint32_t)(*(uint32_t *)pp->data));
         break;
      case DT_INT32:
         printf("%d\n", (int32_t)(*(int32_t *)pp->data));
         break;
      case DT_UINT64:
         printf("%lu\n", (uint64_t)(*(uint64_t *)pp->data));
         break;
      case DT_INT64:
         printf("%ld\n", (int64_t)(*(int64_t *)pp->data));
         break;
      case DT_NULL:
      default:
         /* This will end badly - error on a output line half way through printing
            data. This is more of an assert()-like error, so feel free to print LFs
            agressively to insure that we are clear of the data. */
         printf("\n");
         fflush(stdout);
         
         fprintf(stderr, "ERROR: Data type for \"%s\" was not properly set.\n", pp->tag);
         return(1);
      }
      return(0);
   }

   if ( OUTPUT_HEX_UC == o->eDumpHex )
   {
      switch ( pp->dt )
      {
      case DT_CHAR:
      case DT_UINT8:
      case DT_INT8:
         printf("0X%02X\n", (uint8_t)(*(uint8_t *)pp->data));
         break;
      case DT_UINT16:
      case DT_INT16:
         printf("0X%04X\n", (uint16_t)(*(uint16_t *)pp->data));
         break;
      case DT_UINT32:
      case DT_INT32:
         printf("0X%08X\n", (uint32_t)(*(uint32_t *)pp->data));
         break;
      case DT_UINT64:
      case DT_INT64:
         printf("0X%016lX\n", (uint64_t)(*(uint64_t *)pp->data));
         break;
      case DT_NULL:
      default:
         /* See notes on similar section above */
         printf("\n");
         fflush(stdout);
         
         fprintf(stderr, "ERROR: Data type for \"%s\" was not properly set.\n", pp->tag);
         return(1);
      }
      return(0);
   }

   if ( OUTPUT_HEX_LC == o->eDumpHex )
   {
      switch ( pp->dt )
      {
      case DT_CHAR:
      case DT_UINT8:
      case DT_INT8:
         printf("0x%02x\n", (uint8_t)(*(uint8_t *)pp->data));
         break;
      case DT_UINT16:
      case DT_INT16:
         printf("0x%04x\n", (uint16_t)(*(uint16_t *)pp->data));
         break;
      case DT_UINT32:
      case DT_INT32:
         printf("0x%08x\n", (uint32_t)(*(uint32_t *)pp->data));
         break;
      case DT_UINT64:
      case DT_INT64:
         printf("0x%016lx\n", (uint64_t)(*(uint64_t *)pp->data));
         break;
      case DT_NULL:
      default:
         /* See notes on similar section above */
         printf("\n");
         fflush(stdout);
         
         fprintf(stderr, "ERROR: Data type for \"%s\" was not properly set.\n", pp->tag);
         return(1);
      }
      return(0);
   }

   /* Something was not handled */
   printf("\n");
   return(1);
}

/* ========================================================================= */
int print_parsepoint(RuleSet *rs, ParsePoint *pp, Options *o)
{
   char *lhs;
   char FMT[16];
   
   /* If we don't belong, then just pop out */
   if ( 0 == pp->print_result )
      return(0);
   
   /* Check for some error conditions */
   if ( DT_NULL == pp->dt )
   {
      fprintf(stderr, "ERROR: Data type for \"%s\" was not properly set.\n", pp->tag);
      return(1);
   }

   /* Conditionally show the label */
   if ( o->bShowLabel )
   {
      if ( o->bTagVal )
      {
         /* Do not put the %c into this string. User may pass %, this would
            then break output. */
         lhs = pp->tag;
         sprintf(FMT, "%%-%ds %%c ", MAX_TAG_LEN);
      }
      else
      {
         lhs = pp->label;
         sprintf(FMT, "%%-%ds %%c ", rs->maxlabel);
      }

      /* Now actually print it to stdout */
      printf(FMT, lhs, o->cFields, pp->rdata);
   }

   /* Farm the rest of the complexity out some more */
   return(print_datatype(pp, o));
}

/* ========================================================================= */
int DumpResults(RuleSet *rs, Options *o)
{
   ParsePoint *thispp;

   thispp = rs->pplist;
   while ( thispp )
   {
      print_parsepoint(rs, thispp, o);
      
      thispp = thispp->next;
   }
   
   fflush(stdout);
   return(0);
}
