#pragma once

#define PD_SLIST_HEAD(name, type) struct name { type* slh_first; }
#define PD_SLIST_HEAD_INIT(head) { NULL }
#define PD_SLIST_ENTRY(type) struct { type* sle_next; }
#define PD_SLIST_EMPTY(head) ((head)->slh_first == NULL)
#define PD_SLIST_FIRST(head) ((head)->slh_first)
#define PD_SLIST_FOREACH(var, head, field) for ((var) = PD_SLIST_FIRST(head); (var); (var) = PD_SLIST_NEXT((var), field))
#define PD_SLIST_INIT(head) do { PD_SLIST_FIRST((head)) = NULL; } while (0)
#define PD_SLIST_NEXT(elem, field) ((elem)->field.sle_next)
#define PD_SLIST_REMOVE_HEAD(head, field) do { PD_SLIST_FIRST((head)) = PD_SLIST_NEXT(PD_SLIST_FIRST(head), field); } while (0)

#define PD_SLIST_INSERT_AFTER(slelem, elem, field) do { \
  PD_SLIST_NEXT((elem), field) = PD_SLIST_NEXT((slelem), field); \
  PD_SLIST_NEXT((slemem), field) = (elem); \
} while (0)

#define PD_SLIST_INSERT_HEAD(head, elem, field) do { \
  PD_SLIST_NEXT((elem), field) = PD_SLIST_FIRST((head)); \
  PD_SLIST_FIRST((head)) = (elem); \
} while (0)

#define PD_SLIST_REMOVE(head, elem, type, field) do { \
  if (PD_SLIST_FIRST((head)) == ((elem))) { PD_SLIST_REMOVE_HEAD((head), field); } else { \
    type* currlm = PD_SLIST_FIRST((head)); \
    while (PD_SLIST_NEXT(currlm, field) != (elem)) currlm = PD_SLIST_NEXT(currlm, field); \
    PD_SLIST_NEXT(currlm, field) = PD_SLIST_NEXT(PD_SLIST_NEXT(currlm, field), field); \
  } \
} while (0)
