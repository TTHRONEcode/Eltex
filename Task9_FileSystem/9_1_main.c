#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(){
  char string_to_write[20] = "String from file";
  char buffer_string[20] = {0};

  int my_file;

  creat("./output.txt", S_IRWXU);
  my_file = open("./output.txt", O_WRONLY);
  write (my_file, string_to_write, sizeof(string_to_write));
  close(my_file);

  my_file = open("./output.txt", O_RDONLY);
  read(my_file, buffer_string, sizeof(string_to_write));
  close(my_file);

  for (int i = 19; i >= 0; i--) {
    printf("%c", buffer_string[i]);
  }

  printf("\n");

  return 0;
}
