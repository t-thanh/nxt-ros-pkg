#include <nxt.h>
#include <sstream>
#include <boost/lexical_cast.hpp>

using namespace std;

int main(int argc, char **argv)
{
	Nxt *nxt = new Nxt();
	std::string msg;

	nxt->connectNxt((char*)"00:16:53:09:BD:4B");

	for(unsigned int i=0; i < 100; ++i)
	{
		 msg = std::string(boost::lexical_cast<std::string>(static_cast<int>(i)));
		 cout << "Sending..." << msg << endl;
		 sleep(1);
	}

	nxt->disconnectNxt();

	return 0;
}
