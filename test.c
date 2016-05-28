//hello, git world!
// add a sentence here!
#include <stdio.h>
#include "test.h"

int main()
{
	int x = 100;
	int y = 9;

	printf("hello, git world!");
	int c = max(100, 9);
	printf("c = %d\n", c);

	return 0;
}

void max(int a, int b)
{
	return a>b? a: b;
}

void min(int a, int b)
{
	return a<b? a: b;
}

void print(int a, int b)
{
	return 0;
}