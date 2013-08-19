#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef __ZYLIB__
#include "zylib.h"
#endif


/*****************************************************************************/
/*  宏与常量定义  */
/*****************************************************************************/

extern const ADT undefined_object = &undefined_object;

extern const unsigned int inexistent_index = 0xFFFFFFFF;



/*****************************************************************************/
/*  时间处理  */
/*****************************************************************************/

STRING TimeToString(const struct tm* t)
{
  STRING _s;
  _s = CreateObject(char, 20);
  if(!strftime(_s, 20, "%Y-%m-%d %H:%M:%S", t))
  {
    DestroyObject(_s);
    return NULL;
  }
  return _s;
}
STRING CurrentTimeToString()
{
  time_t _t0 = time(NULL);
  struct tm* _t1 = localtime(&_t0);
  return TimeToString(_t1);
}



/*****************************************************************************/
/*  错误处理  */
/*****************************************************************************/

#define EXITCODE (-1)

void PrintErrorMessage(BOOL on, CSTRING fmt, ...)
{
  va_list _args;
  STRING _s;
  if(fmt)
  {
    _s = CurrentTimeToString();
    va_start(_args, fmt);
    fprintf(stderr, "[");
    fprintf(stderr, _s);
    fprintf(stderr, "] ");
    vfprintf(stderr, fmt, _args);
    fprintf(stderr, "\n");
    va_end(_args);
  }
  if(!on)
    exit(EXITCODE);
}


void PrintErrorMessageToFile(FILE* fp, BOOL on, CSTRING fmt, ...)
{
  va_list _args;
  STRING _s;
  if(fp && fmt)
  {
    _s = CurrentTimeToString();
    va_start(_args, fmt);
    fprintf(fp, "[");
    fprintf(fp, _s);
    fprintf(fp, "] ");
    vfprintf(fp, fmt, _args);
    fprintf(fp, "\n");
    va_end(_args);
  }
  if(!on)
    exit(EXITCODE);
}



/*****************************************************************************/
/*  动态内存分配与管理  */
/*****************************************************************************/



/*****************************************************************************/
/*  字符串功能  */
/*****************************************************************************/

int GetIntegerFromKeyboard()
{
  STRING _s;
  int _n;
  char _junk;
  while(TRUE)
  {
    _s = GetStringFromKeyboard();
    switch(sscanf(_s, " %d %c", &_n, &_junk))
    {
    case 1:
      DestroyObject(_s);
      return _n;
    case 2:
      printf("Unexpected character '%c'.\n", _junk);
      break;
    default:
      printf("Please input an integer.\n");
      break;
    }
    DestroyObject(_s);
    printf("Retry: ");
  }
}

double GetRealFromKeyboard()
{
  STRING _s;
  double _d;
  char _junk;
  while(TRUE)
  {
    _s = GetStringFromKeyboard();
    switch(sscanf(_s, " %lf %c", &_d, &_junk))
    {
    case 1:
      DestroyObject(_s);
      return _d;
    case 2:
      printf("Unexpected character '%c'.\n", _junk);
      break;
    default:
      printf("Please input an real number.\n");
      break;
    }
    DestroyObject(_s);
    printf("Retry: ");
  }
}

STRING GetStringFromKeyboard()
{
  fflush(stdin);
  return GetLineFromFile(stdin);
}

STRING GetLineFromFile(FILE* fp)
{
  STRING _s, _t;
  int _n, _c, _size;
  _n = 0;
  _size = BUFSIZ;
  _s = CreateObject(char, _size);
  while((_c = getc(fp)) != '\n' && _c != EOF)
  {
    if(_n >= _size - 1)
    {
      _size <<= 1;
      _t = CreateObject(char, _size);
      strncpy(_t, _s, _n);
      DestroyObject(_s);
      _s = _t;
    }
    _s[_n++] = (char)_c;
  }
  if(_n == 0 && _c == EOF)
  {
    DestroyObject(_s);
    return NULL;
  }
  _s[_n] = '\0';
  _t = CreateObject(char, _n + 1);
  strcpy(_t, _s);
  DestroyObject(_s);
  return _t;
}

STRING DuplicateString(STRING s)
{
  STRING _s;
  unsigned int _n, _i;
  _n = strlen(s);
  _s = CreateObject(char, _n + 1);
  /* while( *_s++ = *s++ ); */
  for(_i=0; _i<_n; _i++)
    _s[_i] = s[_i];
  _s[_n] = '\0';
  return _s;
}

STRING ConcatenateString( STRING s1, STRING s2 )
{
  STRING _s;
  unsigned int _n1, _n2;
  if( !s1 || !s2 )
    PrintErrorMessage( FALSE, "ConcatenateString: Illegal string parameter(s).\n" );
  _n1 = strlen( s1 );
  _n2 = strlen( s2 );
  _s = CreateObject( char, _n1 + _n2 + 1 );
  strcpy( _s, s1 );
  strcpy( _s + _n1, s2 );
  return _s;
}

int CompareString( STRING s1, STRING s2 )
{
  if( !s1 || !s2 )
    PrintErrorMessage( FALSE, "CompareString: Illegal string parameter(s).\n" );
  return strcmp( s1, s2 );
}

BOOL IsStringEqual(STRING s1, STRING s2)
{
  BOOL _r;
  if(!s1 || !s2)
    PrintErrorMessage(FALSE, "IsStringEqual: Illegal string parameter(s).\n");
  _r = strcmp(s1, s2) == 0;
  return _r;
}

BOOL IsStringEqualWithoutCase(STRING s1, STRING s2)
{
  STRING _s1, _s2;
  BOOL _r;
  if(!s1 || !s2)
    PrintErrorMessage(FALSE, "IsStringEqualWithoutCase: Illegal string parameter(s).\n");
  _s1 = DuplicateString(s1);
  TransformStringIntoUpperCase(_s1);
  _s2 = DuplicateString(s2);
  TransformStringIntoUpperCase(_s2);
  _r = strcmp(_s1, _s2) == 0;
  DestroyObject(_s1);
  DestroyObject(_s2);
  return _r;
}

unsigned int GetStringLength( STRING s )
{
  if(!s)
    PrintErrorMessage(FALSE, "GetStringLength: Illegal string parameter(s).\n");
  return strlen(s);
}

char GetIthChar(STRING s, unsigned int pos)
{
  unsigned int _n;
  if(!s)
    PrintErrorMessage(FALSE, "GetIthChar: Illegal string parameter(s).\n");
  _n = strlen(s);
  if(pos >= _n)
    PrintErrorMessage(FALSE, "GetIthChar: Index %d out of range.\n", pos );
  else
    return s[pos];
}

STRING GetSubString( STRING s, unsigned int pos, unsigned int n )
{
  unsigned int _n;
  if(!s)
    PrintErrorMessage(FALSE, "GetSubString: Illegal string parameter.\n");
  _n = strlen( s );
  if( pos >= _n )
    PrintErrorMessage(FALSE, "GetSubString: Index %d out of range.\n", pos );
  else
  {
    unsigned int _m = n < _n - pos ? n : _n - pos;
    STRING _s = CreateObject( char, _m + 1 );
    unsigned int _i;
    for( _i = 0; _i < _m; _i++ )
      _s[_i] = s[pos + _i];
    _s[_m] = '\0';
    return _s;
  }
}

STRING TransformStringIntoUpperCase(STRING s)
{
  unsigned int _n, _i = 0;
  if(!s)
    PrintErrorMessage(FALSE, "TransformStringIntoUpperCase: Illegal string parameter.\n");
  _n = strlen(s);
  for(_i=0; _i<_n; _i++)
    s[_i] = toupper(s[_i]);
  return s;
}


STRING TransformStringIntoLowerCase(STRING s)
{
  unsigned int _n, _i = 0;
  if(!s)
    PrintErrorMessage(FALSE, "TransformStringIntoLowerCase: Illegal string parameter.\n");
  _n = strlen(s);
  for(_i=0; _i<_n; _i++)
    s[_i] = tolower(s[_i]);
  return s;
}

STRING TransformCharIntoString( char c )
{
  STRING _s = CreateObject( char, 2 );
  _s[0] = c;
  _s[1] = '\0';
  return _s;
}

STRING TransformIntegerIntoString( int n )
{
  char _s[BUFSIZ];
  sprintf( _s, "%d", n );
  return DuplicateString(_s);
}

int TransformStringIntoInteger( STRING s )
{
  int _n;
  char _junk;
  if(!s)
    PrintErrorMessage(FALSE, "TransformStringIntoInteger: Illegal string parameter.\n");
  if( sscanf( s, " %d %c", &_n, &_junk ) != 1 )
    PrintErrorMessage(FALSE, "TransformStringIntoInteger: %s is not a number.\n", s );
  return _n;
}

STRING TransformRealIntoString( double d )
{
  char _s[BUFSIZ];
  sprintf( _s, "%G", d );
  return DuplicateString(_s);
}

double TransformStringIntoReal( STRING s )
{
  double _d;
  char _junk;
  if(!s)
    PrintErrorMessage(FALSE, "TransformStringIntoReal: Illegal string parameter.\n");
  if( sscanf( s, " %lg %c", &_d, &_junk ) != 1 )
    PrintErrorMessage(FALSE, "TransformStringIntoReal: %s is not a real number.\n", s );
  return _d;
}

unsigned int FindCharFirst( char key, STRING s )
{
  unsigned int _i;
  if(!s)
    PrintErrorMessage(FALSE, "FindCharFirst: Illegal string parameter.\n");
  for( _i = 0; s[_i] != '\0'; _i++ )
  {
    if( s[_i] == key )
      return _i;
  }
  return inexistent_index;
}

unsigned int FindCharNext( char key, STRING s, unsigned int pos )
{
  unsigned int _i;
  if(!s)
    PrintErrorMessage(FALSE, "FindCharNext: Illegal string parameter.\n");
  if( pos >= strlen(s) )
    return inexistent_index;
  for( _i = pos; s[_i] != '\0'; _i++ )
  {
    if( s[_i] == key )
      return _i;
  }
  return inexistent_index;
}

unsigned int FindSubStringFirst( STRING key, STRING s )
{
  STRING _s;
  if( !s || !key )
    PrintErrorMessage(FALSE, "FindSubStringFirst: Illegal string parameter.\n");
  _s = strstr( s, key );
  if( !_s )
    return inexistent_index;
  else
    return _s - s;
}

unsigned int FindSubStringNext( STRING key, STRING s, unsigned int pos )
{
  STRING _s;
  if( !s || !key )
    PrintErrorMessage(FALSE, "FindSubStringNext: Illegal string parameter.\n");
  if( pos >= strlen(s) )
    return inexistent_index;
  _s = strstr( s + pos, key );
  if( !_s )
    return inexistent_index;
  else
    return _s - s;
}



