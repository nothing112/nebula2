//------------------------------------------------------------------------------
//  nguilabel_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguilabel.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiLabel, "nguiwidget");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguilabel

    @cppclass
    nGuiLabel
    
    @superclass
    nguiwidget
    
    @classinfo
    Base class for text widgets.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiLabel::nGuiLabel() :
    mouseOver(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiLabel::~nGuiLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    OnMouseMoved() simply sets the mouseOver flag, if a highlight resource
    is set, rendering will then render the highlighted state of the
    label.
*/
bool
nGuiLabel::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->mouseOver = true;
    }
    else
    {
        this->mouseOver = false;
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    This simply renders the label using the default bitmap resource.
*/
bool
nGuiLabel::Render()
{
    if (this->IsShown())
    {
        if (this->mouseOver && this->GetHighlightBrush())
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->highlightBrush);
        }
        else
        {
            nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);
        }
        return true;
    }
    return false;
}
