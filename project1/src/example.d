// test comment
class Program {
	
	int i; boolean result;
	int extra, more;
	int array[5], bar[2], x;
	boolean ok; int junk, more_junk[4];
	
	//"This is a string.";
	
	int add(int a, int b) 
	{
		ok = true;
		return a + b;
	}
	
	int foo()
	{
		junk = -1;
		return 0;
	}
	
	int main()
	{
		for i = 0, 5
		{
			array[i] = 2 * i;
		}
		
		i = foo();
		b = -3 * bar[i] / 6 + (array[1] - add(2, 3));
		
		if (ok)
		{
			b = -1;
		}
		else
		{
			b = 0;
		}
		
		if (!ok)
		{
			b = -2;
		}
		
		return b;
	}
}
