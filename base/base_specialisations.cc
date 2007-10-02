#include <map>
#include <list>
#include <string>
#include <rumba/splodge.h>
template class std::list<std::string>;
template class std::list<void*>;	// for dlopen()
template class std::map<std::string,std::string>;
template class std::map<std::string,RUMBA::Splodge>;

// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
