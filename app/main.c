#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int i = 0;

	char string[100];

	for(i = 0; i < 10; i ++)
	{
		printf("String : ");
		scanf("%s", string);
		printf("%s\n", string);
		printf("mung mung\n");
		sleep(1);
	}

	return 0;
}
