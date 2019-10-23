#include <string.h>

static char* __strtok_r(register char* s, const register char* delim, char** lasts, int skip_leading_delim) {
  register char* spanp;
	register int c, sc;
	char* tok;

	if (s == NULL && (s = *lasts) == NULL) return (NULL);

cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc) {
			if (skip_leading_delim) {
				goto cont;
			}	else {
				*lasts = s;
				s[-1] = 0;
				return (s - 1);
			}
		}
	}

	if (c == 0) {
		*lasts = NULL;
		return (NULL);
	}
	tok = s - 1;

	for (;;) {
		c = *s++;
		spanp = (char*)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0) s = NULL;
				else s[-1] = 0;
				*lasts = s;
				return (tok);
			}
		} while (sc != 0);
	}
  return NULL;
}

void* memset(void* buffptr, int value, size_t size) {
	unsigned char* buff = (unsigned char*)buffptr;
	for (size_t i = 0; i < size; i++) buff[i] = (unsigned char)value;
	return buffptr;
}

void* memcpy(void* dest, const void* src, size_t n) {
  unsigned char* destbuff = (unsigned char*)dest;
  const unsigned char* srcbuff = (const unsigned char*)src;
  for (size_t i = 0; i < n; i++) destbuff[i] = srcbuff[i];
  return dest;
}

int strcmp(const char* s1, const char* s2) {
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return (*(unsigned char*)s1) - (*(unsigned char*)s2);
}

char* strcpy(char* dest, const char* src) {
  char* s = dest;
  while (*dest++ = *src++);
  return s;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

char* strncpy(char* dest, const char* src, size_t n) {
  char* dscan = dest;
  const char* sscan = src;
  while (n > 0) {
    n--;
    if ((*dscan++ = *sscan++) == '\0') break;
  }
  while (n-- > 0) *dscan++ = '\0';
  return dest;
}

char* strtok(char* str, const char* delim) {
  static char* last = NULL;
  return __strtok_r(str, delim, &last, 1);
}
