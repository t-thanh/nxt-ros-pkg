#include <nxt.h>

int main(int argc, char **argv)
{
	Nxt *nxt = new Nxt();

	nxt->findAndConnectNxt();

	//nxt->connectNxt("00:23:6C:BD:37:55");

	return 0;
}
