#ifndef DATAPOINT_H
#define DATAPOINT_H

typedef struct EnumPair
{
   void *value;
   char *label;
} EnumPair;

typedef struct EnumList
{
   EnumPair *plist;
   char *deflabel;
} EnumList;

typedef struct ParsePoint
{
   unsigned long offset;
   unsigned long size;    /* This is really too big, but it is consistent */
   char *label;
   void *data;
} ParsePoint;

ParsePoint *NewParsePoint(unsigned long offsetin,
                          unsigned long sizein,
                          char *labelin);

int CopyInData(ParsePoint *pp, void *data);
#endif
