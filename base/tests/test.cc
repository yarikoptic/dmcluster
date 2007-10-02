#include <rumba/fft.h>

using namespace RUMBA;

int main()
{
	
	if (RUMBA::test_real_fft())
		std::cerr << "Real fft works" << std::endl;

	try {
		srand(time(0));
		for (int i =0; i < 10; ++i )
			RUMBA::test_dct(i+1);

		for (int i =0; i < 10; ++i )
			RUMBA::test_generic_fft(i+1);

	}
	catch (const char*s )
	{
		std::cerr << "Exception: " << s << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

}
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
