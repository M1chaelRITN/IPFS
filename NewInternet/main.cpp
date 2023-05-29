#include "../Core/framework.h"

#include <iostream>
#include <string>
#include <exception>
using namespace std;

int main(int argc, char ** argv)
{
    Parameters parameters(argc, argv);
    if (parameters.Failed())
    {
        cout << parameters.GetUsage() << endl;
        return 1;
    }

    try
    {
        if (parameters.Method() == "network")
        {
            NetManager netManager;
            netManager.PrintNetInfo();
        }
        else
        {
            Repository repository;
            repository.Run(parameters);
        }
    }
    catch (const char * s)
    {
        cerr << s << endl;
    }
	catch (string & s)
    {
        cerr << s << endl;
    }
    catch (exception & ex)
    {
        cerr << ex.what() << endl;
    }
    catch (...)
    {
        cerr << "unhandled exception" << endl;
    }

    return 0;
}
