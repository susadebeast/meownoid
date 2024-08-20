int bound(int min, int val, int max)
{
	if(val > max)
		val = max;
	if(val < min)
		val = min;
	return val;
}