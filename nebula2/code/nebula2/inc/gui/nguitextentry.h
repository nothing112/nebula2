#ifndef N_GUITEXTENTRY_H
#define N_GUITEXTENTRY_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextEntry
    @ingroup NebulaGuiSystem

    A text entry widget.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"
#include "util/neditline.h"

//------------------------------------------------------------------------------
class nGuiTextEntry : public nGuiTextLabel
{
public:
    /// constructor
    nGuiTextEntry();
    /// destructor
    virtual ~nGuiTextEntry();
    /// render the widget
    virtual bool Render();
    /// set the text as string
    virtual void SetText(const char* text);
    /// get the text as string
    virtual const char* GetText() const;
    /// set max string length
    void SetMaxLength(int l);
    /// get max string length
    int GetMaxLength() const;
    /// set the cursor brush
    void SetCursorBrush(const char* name);
    /// get the cursor brush
    const char* GetCursorBrush() const;
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle character code entry
    virtual void OnChar(uchar charCode);
    /// handle key down event
    virtual bool OnKeyDown(nKey key);
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// activate the text entry widget (same as clicking mouse over widget)
    void SetActive(bool b);
    /// currently active?
    bool GetActive() const;
    /// set optional "empty" replacement text
    void SetEmptyText(const char* text);
    /// get optional "empty" replacement text
    const char* GetEmptyText() const;

protected:
    /// check if text edit line contains empty text and replace with provided empty replacement text
    void CheckEmptyText();

    bool mouseOver;
    bool active;
    bool firstFrameActive;
    nEditLine* lineEditor;
    nString emptyText;
    nString cursorBrush;
};

//------------------------------------------------------------------------------
/**
    Return true if widget is currently active.
*/
inline
bool
nGuiTextEntry::GetActive() const
{
    return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextEntry::SetCursorBrush(const char* name)
{
    this->cursorBrush = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiTextEntry::GetCursorBrush() const
{
    return this->cursorBrush.IsEmpty() ? 0 : this->cursorBrush.Get();
}

//------------------------------------------------------------------------------
#endif

