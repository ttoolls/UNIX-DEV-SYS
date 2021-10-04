#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>
void (*print_message)(const char *);

bool load_library() {
  void *hdl = dlopen("./libPrint.so", RTLD_LAZY);
  if ( hdl == NULL )
    return false;
  print_message = (void (*)(const char*))dlsym(hdl,"print_message");
  if ( print_message == NULL )
    return false;
  return true;
}

int main() {
  if ( load_library() )
    print_message("4st sample");
  else
    printf("Cannot find library\n");
  return 0;
}
