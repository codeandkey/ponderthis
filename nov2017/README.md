## Ponder This November 2017

This is my solution to IBM's November challenge.

### Compilation

To compile the test suite (computes averages for x,y,z to verify the probabilities are 0.5, 0.5, and 3.5 respectively),
execute `make` in this directory. The programs are quite basic and have no dependencies other than libc, so they should
compile on most machines.

### Solution expression

It uses a total of 108 characters (not including k=, d=, parentheses etc) and 4 intermediate variables to compute the solution to the problem.

Solution:

	# assuming a C-like syntax (so, ^ is an XOR operation, this solution does not require exponentiation)
	
	s = b+c;
	k = (a+5)/7;
	d = (4+c-a)/5+((b-a)*(c-b)+5)/6;
	f = 1^((d/2)*(k^1)*((b^7)/3));
	
	# total intermediate cost: 3+5+19+15 = 42
	
	x = f-(d&1)*((((a-1)%4)+6)/7)
	y = f-(d/2)*k*(c/5)
	z = 1+(d/2)*(1+k*((15-(a+s))%5)+(k^1)*(s%5))+k*(d&1)*(1+(a+c)%5)
	
	# total expression cost: 15 + 11 + 40 = 66
	# total cost of solution: 42 + 66 = 108 characters

This solution uses only addition, subtraction, multiplication, integer division, integer modulo, binary xor, and binary AND.
