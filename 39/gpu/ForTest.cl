__kernel void test(__global unsigned long* message)
{
	// получаем текущий id.
	int gid = get_global_id(0);

	message[gid] += gid;
}
