#ifndef PROPERTIES_SIMPLEGRAPHICSPROPERTY_H
#define PROPERTIES_SIMPLEGRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    This is a simple graphics property which adds visibility to a game 
    entity. 
  
    (C) 2006 Radon Labs GmbH
*/
#include "properties/abstractgraphicsproperty.h"

//------------------------------------------------------------------------------
namespace Graphics
{
    class Entity;
};

namespace Properties
{
class SimpleGraphicsProperty : public AbstractGraphicsProperty
{
    DeclareRtti;
	DeclareFactory(SimpleGraphicsProperty);

public:
    /// constructor
    SimpleGraphicsProperty();
    /// destructor
    virtual ~SimpleGraphicsProperty();
    
    /// get the graphics entity
    Graphics::Entity* GetGraphicsEntity() const;
    
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    /// return true if message is accepted
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);
protected:
    /// setup graphics entity
    void SetupGraphics();
    /// cleanup graphics entity
    void CleanupGraphics();

    Ptr<Graphics::Entity> graphicsEntity;
};

RegisterFactory(SimpleGraphicsProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif