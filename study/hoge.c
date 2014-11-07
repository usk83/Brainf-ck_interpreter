#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char name[2];
    int i;
    puts("type in you name with small letters");
    scanf("%[^\n]", name);
    for(i=0; name[i]; i++)
    {
        if(name[i] >= 97 && name[i] <= 122)
        {
            printf("%c", name[i]-32);
        }
        else
        {
            printf("%c", name[i]);
        }
    }
    puts("");
    return 0;
}
