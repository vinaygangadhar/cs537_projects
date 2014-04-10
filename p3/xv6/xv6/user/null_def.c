// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int* ptr = 0;

	printf(1, "First address: 0x%x\n", *ptr);
	return 0;
}

