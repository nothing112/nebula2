#ifndef FSM_STATE_H
#define FSM_STATE_H
//------------------------------------------------------------------------------
/**
    @class FSM::State

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/ptr.h"
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace FSM
{
class State : public Foundation::RefCounted
{
    DeclareRtti;

public:
    /// constructor
    State(const nString& name);
    /// destructor
    virtual ~State();

    /// Called when state is active
    virtual void Trigger();

    /// Called when the state is entered
    virtual void OnEnter();
    /// Called when the state is left
    virtual void OnLeave();

    /// Get state name
    const nString& GetName() const;

protected:
    nString name;
};

} // namespace FSM
//------------------------------------------------------------------------------
#endif
