#include "stdio.h"
#include "Resource.h"
#include <string.h>
int main(int argc, char** argv )
{
	printf("%s\n", Resource_GetText("LICENCE.md"));
	printf("%d\n", Resource_Has("LICENCE.md"));
	printf("%d\n", strlen(Resource_GetText("LICENCE.md")));
	return 0;
}