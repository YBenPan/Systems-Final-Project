#include <string.h>

#include "strcmds.h"

void chop_newline(char *s) {
  size_t ln = strlen(s) - 1;
  if (*s && ln > 0 && s[ln] == '\n'){
    s[ln] = '\0';
    ln--;
    if(ln > 0 && s[ln] == '\r'){
      // windows line endings, potentially
      s[ln] = '\0';
    }
  }
}