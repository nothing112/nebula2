//------------------------------------------------------------------------------
/**
    @page NebulaToolsnsh nsh

    nsh

    <dl>
     <dt>-help</dt>
       <dd>show this help</dd>
     <dt>-startup</dt>
       <dd>run script and go into interactive mode</dd>
     <dt>-run</dt>
       <dd>run script and exit</dd>
     <dt>-scriptserver</dt>
       <dd>define an alternative script server class (default is ntclserver)</dd>
    </dl>

    (C) 2003 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "tools/ncmdlineargs.h"
#include "network/nhttpserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nnetwork);

//------------------------------------------------------------------------------
/**
    Main function.
*/
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg                = args.GetBoolArg("-help");
    const char* startupArg      = args.GetStringArg("-startup", 0);
    const char* runArg          = args.GetStringArg("-run", 0);
    const char* scriptServerArg = args.GetStringArg("-scriptserver", "ntclserver");

    if (helpArg)
    {
        printf("(C) 2003 RadonLabs GmbH\n"
               "nsh - Nebula2 shell\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                run script and go into interactive mode\n"
               "-run                    run script and exit\n"
               "-scriptserver           define an alternative script server class (default is ntclserver)\n");
        return 5;
    }

    // create minimal Nebula runtime
    nKernelServer kernelServer;
    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(nnetwork);

    nScriptServer* scriptServer = (nScriptServer*) kernelServer.New(scriptServerArg, "/sys/servers/script");
    if (0 == scriptServer)
    {
        n_printf("Could not create script server of class '%s'\n", scriptServerArg);
        return 10;
    }
    nHttpServer* httpServer = (nHttpServer*) kernelServer.New("nhttpserver", "/sys/servers/http");
    n_assert(httpServer);

    if (runArg)
    {
        const char* result;
        scriptServer->RunScript(runArg, result);
    }
    else
    {
        if (startupArg)
        {
            const char* result;
            scriptServer->RunScript(startupArg, result);
        }

        // interactively execute commands
        bool lineOk = true;
        scriptServer->SetFailOnError(false);
        while (!scriptServer->GetQuitRequested() && lineOk)
        {
            char line[1024];
            line[0] = '\0';

            // generate prompt string
            nString prompt = scriptServer->Prompt();
            printf("%s", prompt.Get());
            fflush(stdout);

            // get user input
            bool lineOk = (gets(line) > 0);
            if (strlen(line) > 0)
            {
                const char* result = 0;
                scriptServer->Run(line, result);
                if (result)
                {
                    printf("%s\n", result);
                }
            }
        }
    }
    httpServer->Release();
    scriptServer->Release();
    return 0;
}
