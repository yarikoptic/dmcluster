#ifndef RUMBA_PROGRAM_H
#define RUMBA_PROGRAM_H

#include <rumba/arghandler.h>
#include <string>
#include <memory>
#include <iostream>

namespace RUMBA {

class Program
{
    Program(const Program&);
    Program& operator=(const Program&);
    int status;
    std::string usage;
    std::auto_ptr<RUMBA::ArgHandler> m_argh;

protected:

    RUMBA::ArgHandler * argh();
public:
    /* 
     * myArgs should be an array of arguments (NULL if default is OK), 
     * usg is a usage message 
     */
    Program (int argc, char ** argv, RUMBA::Argument* myArgs, 
            const char* usg = "" );
    virtual int run() ;

    /* override this member function to customize program behaviour */
    virtual int run_helper() ;

};

} /* namespace RUMBA */

#endif
