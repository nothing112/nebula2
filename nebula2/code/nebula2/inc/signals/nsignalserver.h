#ifndef N_SIGNALSERVER_H
#define N_SIGNALSERVER_H
//------------------------------------------------------------------------------
/**
    @file nsignalemitter.h
    @class nSignalEmitter
    @ingroup NebulaSignals

    The nSignalServer currently serves a single purpose only:

   * Provides a Trigger method to be called from the main event
     loop to handle asynchronous signal emission.  See nSignalEmitter's
     PostSignal and PostSignalWithAccumulator methods.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nobject.h"
#include "kernel/nref.h"
#include "util/nlist.h"

//------------------------------------------------------------------------------
const int N_SIGNALSERVER_MAX_SIGNALS = 512;

//------------------------------------------------------------------------------
class nCmd;
class nSignal;

//------------------------------------------------------------------------------
class nSignalServer
{
public:
    /// constructor
    nSignalServer();
    /// destructor
    ~nSignalServer();
    /// get instance pointer
    static nSignalServer* Instance();
    /// trigger signal server
    void Trigger(nTime t);

    /// Post a signals and commands for later execution by the signal server
    bool PostCmd(nTime t, nObject * emitter, nCmd * cmd);

protected:

    /// nPostedSignal type contains all information needed by a signal server
    /// for delayed execution of signals
    struct nPostedSignal : nNode
    {
        nRef<nObject> emitter;
        nCmd * cmd;
        nTime t;
    };

    // static variables
    static nSignalServer* Singleton;

    // list of the signals to emit sorted by time
    nList postedSignals;
    nList freeSignals;
    nPostedSignal signals[N_SIGNALSERVER_MAX_SIGNALS];
};

//------------------------------------------------------------------------------
inline
nSignalServer::nSignalServer()
{
    n_assert(0 == Singleton);
    Singleton = this;

    for(int i = 0;i < N_SIGNALSERVER_MAX_SIGNALS;i++)
    {
        this->freeSignals.AddTail(&this->signals[i]);
    }
}

//------------------------------------------------------------------------------
inline
nSignalServer::~nSignalServer()
{
    nSignalServer::nPostedSignal * aSignal = static_cast<nSignalServer::nPostedSignal *> (this->postedSignals.GetHead());
    while (aSignal)
    {
        // release posted signal
        aSignal->cmd->GetProto()->RelCmd(aSignal->cmd);
        aSignal->emitter.invalidate();
        aSignal->cmd = 0;
        aSignal->t = 0;
        aSignal->Remove();

        aSignal = static_cast<nSignalServer::nPostedSignal *> (this->postedSignals.GetHead());
    }

    aSignal = static_cast<nSignalServer::nPostedSignal *> (this->freeSignals.GetHead());
    while (aSignal)
    {
        aSignal->Remove();
        aSignal = static_cast<nSignalServer::nPostedSignal *> (this->freeSignals.GetHead());
    }
}

//------------------------------------------------------------------------------
inline
nSignalServer*
nSignalServer::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
