#include <rumba/exemplar.h>
#include <iostream>
// name is only used for debugging
Exemplar::Exemplar(const char* 
#ifdef DEBUG
		name
#endif
) 
{ 
#ifdef DEBUG
	std::cerr << "Calling Exemplar(" << name << ")\n";
#endif
} 
// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
