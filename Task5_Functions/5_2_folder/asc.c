#include <stdio.h>

int main ()
{ 
  char nu[26];

  for(int k=0;k<26;k++)   
  { 
    nu [k] = 0x55;

    switch (k){
    case 20:
      nu [k] = 0xeb;
      break;
    case 21:
      nu [k] = 0x51;
      break;
    }     
  }  

  FILE *fp = fopen("./data.txt", "w");
  fwrite(nu, sizeof (nu[0]), sizeof(nu), fp);
  fclose(fp);

  return 0;  
}
