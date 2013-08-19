#ifndef __LIST__
#define __LIST__

#ifndef __ZYLIB__
#include "zylib.h"
#endif

typedef struct __LINKED_LIST * LINKED_LIST;

typedef int ( * COMPARE_OBJECT )( CADT e1, CADT e2 );
typedef void ( * DESTROY_OBJECT )( ADT e );
typedef void ( * MANIPULATE_OBJECT )( ADT e, ADT tag );

LINKED_LIST LlCreate();
void LlDestroy( LINKED_LIST list, DESTROY_OBJECT destroy );
void LlClear( LINKED_LIST list, DESTROY_OBJECT destroy );
void LlAppend( LINKED_LIST list, ADT object );
void LlInsert( LINKED_LIST list, ADT object, unsigned int pos );
void LlDelete( LINKED_LIST list, unsigned int pos, DESTROY_OBJECT destroy );
void LlTraverse( LINKED_LIST list, MANIPULATE_OBJECT manipulate, ADT tag );
BOOL LlSearch( LINKED_LIST list, ADT object, COMPARE_OBJECT compare );
unsigned int LlGetCount( LINKED_LIST list );
BOOL LlIsEmpty( LINKED_LIST list );

#endif

