#include <rumba/program.h>

RUMBA::ArgHandler * RUMBA::Program::argh() 
{ 
    return m_argh.operator-> (); 
}

RUMBA::Program::Program (int argc, char ** argv, RUMBA::Argument* myArgs, 
            const char* usg  ): m_argh(0), status(0), usage(usg)
{
    try {
        std::auto_ptr<RUMBA::ArgHandler> tmp (new RUMBA::ArgHandler(argc,argv,myArgs));
        m_argh = tmp;
    }
    catch (RUMBA::Exception & e)
    {
        std::cerr << e.error() << std::endl;
        std::cerr << usage << std::endl;
        status = 1;
    }
}

int RUMBA::Program::run() 
{
    if (status)
        return status; // dont try to run if we've already had an error

    if (m_argh->arg("help"))
    {
        std::cerr << usage << std::endl;
        return 0;
    }
    try {
        return run_helper();
    }
    catch (RUMBA::Exception & e)
    {
        std::cerr << e.error() << std::endl;
        return 1;
    }
}


int RUMBA::Program::run_helper() { return 0; }

// vim: set ts=4 sw=4 expandtab:
// Local Variables:
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
