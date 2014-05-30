#include <stdio.h>
#include <unistd.h>

int add(int a, int b)
{
	return a + b;
}

int main(int argc, char* argv[])
{
	int i = 0;
	int j = 0;

	for(i = 0; i < argc; i++)
	{
		printf("arg : %s\n", argv[i]);
	}

	for(i = 0; i < 10; i ++)
	{
		j = i*10;
		printf("sum : %d\n", add(i, j));
	}

	return 0;
}


