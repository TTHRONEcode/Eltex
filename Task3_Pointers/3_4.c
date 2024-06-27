// 4. Напишите программу, которая ищет в введенной строке (с клавиатуры)
// введенную подстроку (с клавиатуры) и возвращает указатель на начало
// подстроки, если подстрока не найдена в указатель записывается NULL.
// В качестве строк использовать статические массивы.

#include <stdio.h>

#define N 255

int main() {

  char sentens[N] = {0}, word[N] = {0};
  char *ptr = NULL;

  int counter = 0, char_count = 0, start_num = 0;
  int i, j;

  printf("Enter a sentens of several words (max character - %i):\n", N);
  scanf("%[^\n]%*c", sentens);

  printf("Enter a word to find (max character - %i):\n", N);
  scanf("%[^\n]%*c", word);

  // TODO попробовать сделать через битовые операции
  for (i = 0; i < N; i++) {
    if (word[counter] == 0)
      break;

    if (word[i] != 0)
      char_count++;

    for (j = 0; j < 2; j++) {
      if (sentens[i] == word[counter]) {
        if (counter == 0)
          start_num = i;

        counter++;
        break;
      } else {
        counter = 0;
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