#include <stdio.h>

#ifndef __ZYLIB__
#include "zylib.h"
#endif

#ifndef __LIST__
#include "list.h"
#endif

#include <pthread.h>
#include <semaphore.h>

#define BEFORE_READ(list); \
  pthread_mutex_lock (&(list)->rr_mutex);\
  (list)->reader_count++;\
  if((list)->reader_count == 1) pthread_mutex_lock(&(list)->ww_rw_mutex);\
  pthread_mutex_unlock (&(list)->rr_mutex);

#define END_READ(list); \
  pthread_mutex_lock (&(list)->rr_mutex);\
  (list)->reader_count--;\
  if((list)->reader_count == 0) pthread_mutex_unlock(&(list)->ww_rw_mutex);\
  pthread_mutex_unlock (&(list)->rr_mutex);

#define BEFORE_WRITE(list); \
  pthread_mutex_lock (&(list)->ww_rw_mutex);\
  int old_cancel_state;\
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &old_cancel_state);

#define END_WRITE(list); \
  pthread_setcancelstate (old_cancel_state, NULL);\
  pthread_mutex_unlock (&(list)->ww_rw_mutex);


typedef struct __NODE * NODE;
struct __LINKED_LIST
{ 
  unsigned int count; 
  NODE head, tail;
  unsigned int reader_count;//读者计数器
  pthread_mutex_t ww_rw_mutex;//写写&&读写互斥
  pthread_mutex_t rr_mutex;//读读互斥
};
struct __NODE{ ADT data; NODE next; };

LINKED_LIST LlCreate()
{
  LINKED_LIST p = NewObject( struct __LINKED_LIST );
  p->count = 0;
  p->head = NULL;
  p->tail = NULL;
  //初始化
  p->reader_count = 0;
  pthread_mutex_init(&(p->ww_rw_mutex),NULL);
  pthread_mutex_init(&(p->rr_mutex),NULL);
  return p;
}

void LlDestroy( LINKED_LIST list, DESTROY_OBJECT destroy )
{
  if( list ){  LlClear( list, destroy );  DestroyObject( list );  }
}

void LlClear( LINKED_LIST list, DESTROY_OBJECT destroy )
{
  BEFORE_WRITE(list);
  if( !list )  PrintErrorMessage( FALSE, "LlClear: Parameter illegal." );
  while( list->head )
  {
    NODE t = list->head;
    list->head = t->next;
    if( destroy )  ( *destroy )( t->data );
    DestroyObject( t );
    list->count--;
  }
  list->tail = NULL;
  END_WRITE(list);
}

void LlAppend( LINKED_LIST list, ADT object )
{
  BEFORE_WRITE(list);
  NODE t = NewObject( struct __NODE );
  if( !list || !object )  PrintErrorMessage( FALSE, "LlAppend: Parameter illegal." );
  t->data = object;
  t->next = NULL;
  if( !list->head )  // singly linked list with no elements
  {
    list->head = t;
    list->tail = t;
  }
  else
  {
    list->tail->next = t;
    list->tail = t;
  }
  list->count++;
  END_WRITE(list);
}

void LlInsert( LINKED_LIST list, ADT object, unsigned int pos )
{
  BEFORE_WRITE(list);
  if( !list || !object )  PrintErrorMessage( FALSE, "LlInsert: Parameter illegal." );
  if( pos < list->count ){
    NODE t = NewObject( struct __NODE );
    t->data = object;
    t->next = NULL;
    if( pos == 0 ){
      t->next = list->head;
      list->head = t;
    }
    else{
      unsigned int i;
      NODE u = list->head;
      for( i = 0; i < pos - 1; ++i )  u = u->next;
      t->next = u->next;
      u->next = t;
    }
    list->count++;
    END_WRITE(list);
  }
  else
  {
    END_WRITE(list);
    LlAppend( list, object );
  }
}

void LlDelete( LINKED_LIST list, unsigned int pos, DESTROY_OBJECT destroy )
{
  BEFORE_WRITE(list);
  if( !list )  PrintErrorMessage( FALSE, "LlDelete: Parameter illegal." );
  if( list->count == 0 )
  {
    END_WRITE(list);
    return;
  }
  if( pos == 0 ){
    NODE t = list->head;
    list->head = t->next;
    if( !t->next )  list->tail = NULL;
    if( destroy )  ( *destroy )( t->data );
    DestroyObject( t );
    list->count--;
  }
  else if( pos < list->count ){
    unsigned int i;
    NODE u = list->head, t;
    for( i = 0; i < pos - 1; ++i )  u = u->next;
    t = u->next;
    u->next = t->next;
    if( !t->next )  list->tail = u;
    if( destroy )  ( *destroy )( t->data );
    DestroyObject( t );
    list->count--;
  }
  END_WRITE(list);
}

void LlTraverse( LINKED_LIST list, MANIPULATE_OBJECT manipulate, ADT tag )
{
  BEFORE_WRITE(list);
  NODE t = list->head;
  if( !list )  PrintErrorMessage( FALSE, "LlTraverse: Parameter illegal." );
  while( t ){
    if( manipulate )  ( *manipulate )(t->data, tag );
    t = t->next;
  }
  END_WRITE(list);
}

BOOL LlSearch( LINKED_LIST list, ADT object, COMPARE_OBJECT compare )
{
  BEFORE_READ(list);
  NODE t = list->head;
  if( !list || !object || !compare )  PrintErrorMessage( FALSE, "LlSearch: Parameter illegal." );
  while( t ){
    if( ( *compare )( t->data, object ) )
    {
      END_READ(list);
      return TRUE;
    }
    t = t->next;
  }
  END_READ(list);
  return FALSE;
}

unsigned int LlGetCount( LINKED_LIST list )
{
  BEFORE_READ(list);
  if( !list )  PrintErrorMessage( FALSE, "LlGetCount: Parameter illegal." );
  END_READ(list);
  return list->count;
}

BOOL LlIsEmpty( LINKED_LIST list )
{
  BEFORE_READ(list);
  if( !list )  PrintErrorMessage( FALSE, "LlIsEmpty: Parameter illegal." );
  END_READ(list);
  return list->count == 0;
}

