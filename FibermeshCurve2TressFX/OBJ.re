
#include "stdafx.h"
#include <quaternion.h>
using namespace irr::core;

typedef unsigned char uchar;
typedef unsigned int uint;

#define	YYCTYPE		uchar
#define	YYCURSOR	cursor
#define	YYLIMIT		s->lim
#define	YYMARKER	s->ptr

#define	RET(i)	{s->cur = cursor; return i;}
#define	RETT(i, typeid)	{s->cur = cursor; s->type = typeid; return i;}


class CMYWriter
{
public:
  FILE *fp, *fpTmp;
  int iCntStrand;
  int iMaxVerts;
  
  CMYWriter(FILE* fpWrite) : fp(fpWrite)
  {
    iCntStrand = 0;
    iMaxVerts = 0;
    fpTmp = MyCreateTempFile();
  }
  
  FILE *MyCreateTempFile()
  {
    CString strPath;
    wchar_t *tmpPath = strPath.GetBufferSetLength(_MAX_PATH+2);
    if(!GetTempPath(_MAX_PATH, tmpPath))return NULL;
    CString strFileName;
    wchar_t *tmpFileName = strFileName.GetBufferSetLength(_MAX_PATH+2);
    if(!GetTempFileName(tmpPath, _T("tmpF2C"), 0, tmpFileName))return NULL;
    strPath.ReleaseBuffer();
    FILE *f = _wfopen(tmpFileName, _T("w+"));
    strFileName.ReleaseBuffer();
    return f;
  }
  ~CMYWriter()
  {
    if(fpTmp!=NULL)fclose(fpTmp);
  }
  
  void AddStrand(CAtlArray<vector3df*> *pVertex)
  {
    if(fpTmp==NULL)return;
    size_t numVerts = pVertex->GetCount();
    if(iMaxVerts < numVerts)iMaxVerts = numVerts;
    
    fprintf(fpTmp, "strand %d numVerts %d texcoord 0.000000 000000\n", iCntStrand, numVerts);
    for(int i=0;i<numVerts;i++)
    {
      vector3df &v = (*((*pVertex)[i]));
      //v*=100.0;
      fprintf(fpTmp, "%f, %f, %f\n", v.X, v.Y, v.Z);
    }
    iCntStrand++;
  }
  void Write()
  {
    if(fpTmp==NULL)return;
    fprintf(fp, "version 2.0\n");
    fprintf(fp, "scale 1.0\n");
    fprintf(fp, "rotation 0 0 0\n");
    fprintf(fp, "translation 0 0 0\n");
    fprintf(fp, "bothEndsImmovable 0\n");
    fprintf(fp, "maxNumVerticesInStrand %d\n", iMaxVerts);
    fprintf(fp, "numFollowHairsPerGuideHair 4\n");
    fprintf(fp, "maxRadiusAroundGuideHair 1.5\n");
    fprintf(fp, "numStrands %d\n", iCntStrand);
    fprintf(fp, "is sorted 1\n");
    fseek(fpTmp, 0, SEEK_SET);
    unsigned char *buf = (unsigned char *)malloc(1024);
    if(buf==NULL)return;
    size_t iReaded = 0;
    while(1)
    {
      iReaded = fread(buf, 1, 1024, fpTmp);
      if(iReaded>0)fwrite(buf, 1, iReaded, fp);
      if(iReaded<1024)break;
    }
  }
};

typedef struct PassData
{
  union {
    __int64 i;
    double dbl;
    CString *str;
  };
} PassData;

typedef struct Scanner
{
    int			fd;
    uchar		*bot, *tok, *ptr, *cur, *pos, *lim, *top, *eof;
    uint		line;
    PassData val;
    bool bLineFirst;
} Scanner;

/*enum {
 TAG_g, TAG_l, TAG_v, 
 TAG_Eof};*/


#include "OBJParser.h"
#include "OBJParser.c"


CString ReadStr(uchar *token, uint len)
{
  CStringA tmpA;
  tmpA.SetString((const char *)token, len);
  CStringW tmpB = CA2W(tmpA, CP_UTF8);
  tmpB.Replace(L"%", L"%%");
  CString retStr;
  retStr.Format(tmpB);
  return retStr;
}

BOOL fill(int n) { return FALSE; }

int scan(Scanner *s){
	uchar *cursor = s->cur;
std:
	s->tok = cursor;
/*!re2c

re2c:define:YYFILL:naked = 1;
re2c:define:YYFILL@len = #;
re2c:define:YYFILL = "if(!fill(#)) { return 0; }";
re2c:yyfill:enable = 1;

any	= [\000-\xFF];
CH = [\x21-\x7e\xA1-\xDF];
O	= [0-7];
D	= [0-9];
L	= [a-zA-Z_];
H	= [a-fA-F0-9];
LINEEND = ([\n]|[\r]|[\r][\n]);
SP  = [ \t\v\f]+;
SP2 = [ \t\v\f]*;
ESC	= [\\] ([abfnrtv?'"\\] | "x" H+ | O+);
*/

if(s->bLineFirst==false)goto normal;
//lineFirst:
/*!re2c
	"#"			{ goto comment2; }
	
  SP2 "g" SP	{ s->bLineFirst=false; RET(TAG_g); }
  SP2 "l" SP	{ s->bLineFirst=false; RET(TAG_l); }
  SP2 "v" SP	{ s->bLineFirst=false; RET(TAG_v); }
  
	SP		{ if(cursor == s->eof)RET(0); goto std; }

	LINEEND {
    if(cursor == s->eof)RET(0);
    s->pos = cursor; s->line++;
    s->bLineFirst = true;
    goto std;
  }

	any {
    ATLTRACE("unexpected character: %c\n", *s->tok);
    goto std;
  }
*/

normal:
/*!re2c
	"#"			{ goto comment2; }
	
	[+-]? [ \t]* ((D+) [\.] (D*) | (D*) [\.] (D+))	{ uchar *start = s->tok;
				bool bMinus = false;
				if(*start == '-')bMinus = true;
				while(!((*start>='0' && *start<='9') || *start=='.'))start++;
				CStringA tmp;
				tmp.SetString((const char *)(start), cursor - start);
				s->val.dbl = bMinus ? -atof(tmp) : atof(tmp);
				RET(DBLVAL); }
	
	[+-]? [ \t]* (D+)	{ uchar *start = s->tok;
				bool bMinus = false;
				if(*start == '-')bMinus = true;
				while(*start<'0' || *start>'9')start++;
				CStringA tmp;
				tmp.SetString((const char *)(start), cursor - start);
				s->val.i = bMinus ? -_atoi64(tmp) : _atoi64(tmp);
				RET(INTVAL); }
	
	CH+
				{ s->val.str = new CString(); *(s->val.str) = ReadStr(s->tok, cursor - s->tok); RET(STRVAL); }
	
	[\x1a]			{ RET(0);/*EOF*/ }


	SP		{ if(cursor == s->eof)RET(0); goto std; }

	LINEEND {
    if(cursor == s->eof)RET(0);
    s->pos = cursor; s->line++;
    s->bLineFirst = true;
    goto std;
  }

	any {
    ATLTRACE("unexpected character: %c\n", *s->tok);
    goto std;
  }
*/

comment:
/*!re2c
	"*/"  { if(cursor == s->eof || cursor+1 == s->eof)RET(0); goto std; }
	"\n"  { if(cursor == s->eof) RET(0); s->tok = s->pos = cursor; s->line++;s->bLineFirst = true; goto comment; }
  any   { if(cursor == s->eof)RET(0); goto comment; }
*/
comment2:
/*!re2c
	"\n" { if(cursor == s->eof) RET(0); s->tok = s->pos = cursor; s->line++;s->bLineFirst = true; goto std; }
  any  { if(cursor == s->eof)RET(0); goto comment2; }
*/
}