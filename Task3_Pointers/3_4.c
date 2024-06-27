// 4. Напишите программу, которая ищет в введенной строке (с клавиатуры)
// введенную подстроку (с клавиатуры) и возвращает указатель на начало
// подстроки, если подстрока не найдена в указатель записывается NULL.
// В качестве строк использовать статические массивы.

#include <stdio.h>

char sentens[255], word[255];
char *ptr;

int counter, char_count, start_num;

int main() {

  printf("Enter a sentens of several words (max character - 255):\n");
  scanf("%[^\n]%*c", sentens);

  printf("Enter a word to find (max character - 255):\n");
  scanf("%[^\n]%*c", word);

  for (int i = 0; i < 255; i++) {
    if (word[counter] == 0)
      break;

    if (word[i] != 0)
      char_count++;

    if (sentens[i] == word[counter]) {
      if (counter == 0)
        start_num = i;

      counter++;
    } else {
      counter = 0;

      if (sentens[i] == word[counter]) {
        if (counter == 0)
          start_num = i;

        counter++;
      }
    }
  }

  if (counter == char_count) {
    ptr = &sentens[start_num];
    printf("The word was found.\n"
           "The pointer address is set to word's address: "
           "%p.\nStart index: %i\n",
           ptr, start_num);
  } else {
    ptr = NULL;
    printf("The word was NOT found. The pointer address is set to NULL\n");
  }

  return 0;
}