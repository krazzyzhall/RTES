#include <stdio.h>
#include <stdlib.h>

int main()
{
   char *str;
int i=0;
   /* Initial memory allocation */
   str = (char *) malloc(15);
   strcpy(str, "tutorialspoint");
   printf("String = %s,  Address = %u\n", str, str);

   /* Reallocating memory */
for(i=0;i<10;i++){
   str = (char *) realloc(str, i*25);
   strcat(str, ".com");
   printf("String = %s,  Address = %u\n", str, str);
}
   free(str);
   
   return(0);
}

