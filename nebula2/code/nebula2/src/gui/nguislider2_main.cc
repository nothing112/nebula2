//------------------------------------------------------------------------------
//  nguislider2_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguislider2.h"
#include "gui/nguibutton.h"
#include "gui/nguilabel.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiSlider2, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiSlider2::nGuiSlider2() :
    overallSize(10.0f),
    visibleStart(0.0f),
    visibleSize(1.0f),
    rangeDirty(true),
    knobNegEdgeLayoutIndex(0),
    knobPosEdgeLayoutIndex(0),
    dragging(false),
    horizontal(false),
    dragVisibleStart(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSlider2::~nGuiSlider2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnShow()
{
    nGuiFormLayout::OnShow();

    if (nGuiServer::Instance()->BrushExists("sliderbg"))
    {
        this->SetDefaultBrush("sliderbg");
    }

    kernelServer->PushCwd(this);

    // get some brush sizes
    if (this->horizontal)
    {
        this->arrowBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("arrowleft_n");
        this->SetMinSize(vector2(2.0f * this->arrowBtnSize.x, this->arrowBtnSize.y));
        this->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
    }
    else
    {
        this->arrowBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("arrowup_n");
        this->SetMinSize(vector2(this->arrowBtnSize.x, 2.0f * this->arrowBtnSize.y));
        this->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
    }

    // create negative arrow button
    nGuiButton* negBtn = (nGuiButton*) kernelServer->New("nguibutton", "NegButton");
    n_assert(negBtn);
    negBtn->SetMinSize(this->arrowBtnSize);
    negBtn->SetMaxSize(this->arrowBtnSize);
    if (this->horizontal)
    {
        negBtn->SetDefaultBrush("arrowleft_n");
        negBtn->SetPressedBrush("arrowleft_p");
        negBtn->SetHighlightBrush("arrowleft_h");
    }
    else
    {
        negBtn->SetDefaultBrush("arrowup_n");
        negBtn->SetPressedBrush("arrowup_p");
        negBtn->SetHighlightBrush("arrowup_h");
    }
    this->AttachForm(negBtn, Top, 0.0f);
    this->AttachForm(negBtn, Left, 0.0f);
    negBtn->OnShow();
    this->refNegButton = negBtn;

    // create down arrow button
    nGuiButton* posBtn = (nGuiButton*) kernelServer->New("nguibutton", "PosButton");
    n_assert(posBtn);
    posBtn->SetMinSize(this->arrowBtnSize);
    posBtn->SetMaxSize(this->arrowBtnSize);
    if (this->horizontal)
    {
        posBtn->SetDefaultBrush("arrowright_n");
        posBtn->SetPressedBrush("arrowright_p");
        posBtn->SetHighlightBrush("arrowright_h");
        this->AttachForm(posBtn, Top, 0.0f);
        this->AttachForm(posBtn, Right, 0.0f);
    }
    else
    {
        posBtn->SetDefaultBrush("arrowdown_n");
        posBtn->SetPressedBrush("arrowdown_p");
        posBtn->SetHighlightBrush("arrowdown_h");
        this->AttachForm(posBtn, Bottom, 0.0f);
        this->AttachForm(posBtn, Left, 0.0f);
    }
    posBtn->OnShow();
    this->refPosButton = posBtn;

    // The "slidertrack" brush, if present, lies *between* the arrow buttons.
    if (nGuiServer::Instance()->BrushExists("slidertrack"))
    {
        nGuiLabel* backgroundLabel = (nGuiLabel*) kernelServer->New("nguilabel", "BackgroundLabel");
        n_assert(backgroundLabel);
        backgroundLabel->SetDefaultBrush("slidertrack");
        if (this->horizontal)
        {
            backgroundLabel->SetMinSize(vector2(0, this->arrowBtnSize.y));
            backgroundLabel->SetMaxSize(vector2(1.0f - 2*this->arrowBtnSize.x, this->arrowBtnSize.y));
            this->AttachForm(backgroundLabel, Top, 0.0f);
            this->AttachWidget(backgroundLabel, Left, negBtn, 0);
            this->AttachWidget(backgroundLabel, Right, posBtn, 0);
        }
        else
        {
            backgroundLabel->SetMinSize(vector2(this->arrowBtnSize.x, 0));
            backgroundLabel->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f - 2*this->arrowBtnSize.y));
            this->AttachForm(backgroundLabel, Left, 0.0f);
            this->AttachWidget(backgroundLabel, Top, negBtn, 0);
            this->AttachWidget(backgroundLabel, Bottom, posBtn, 0);
        }
        backgroundLabel->OnShow();
        this->refBgLabel = backgroundLabel;
    }

    // create the slider knob
    nGuiButton* knob = (nGuiButton*) kernelServer->New("nguibutton", "Knob");
    n_assert(knob);
    if (this->horizontal)
    {
        knob->SetDefaultBrush("sliderknobhori_n");
        knob->SetPressedBrush("sliderknobhori_p");
        knob->SetHighlightBrush("sliderknobhori_h");
        knob->SetMinSize(vector2(0.0f, this->arrowBtnSize.y));
        knob->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
        this->knobNegEdgeLayoutIndex = this->AttachPos(knob, Left, 0.0f);
        this->knobPosEdgeLayoutIndex = this->AttachPos(knob, Right, 0.0f);
        this->AttachForm(knob, Top, 0.0f);
        this->AttachForm(knob, Bottom, 0.0f);
    }
    else
    {
        knob->SetDefaultBrush("sliderknobvert_n");
        knob->SetPressedBrush("sliderknobvert_p");
        knob->SetHighlightBrush("sliderknobvert_h");
        knob->SetMinSize(vector2(this->arrowBtnSize.x, 0.0f));
        knob->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
        this->knobNegEdgeLayoutIndex = this->AttachPos(knob, Top, 0.0f);
        this->knobPosEdgeLayoutIndex = this->AttachPos(knob, Bottom, 0.0f);
        this->AttachForm(knob, Left, 0.0f);
        this->AttachForm(knob, Right, 0.0f);
    }
    knob->OnShow();
    this->refKnob = knob;
    
    // these buttons are used to detect mouse presses in the gutter/track
	// they are usually invisible
    nGuiButton* negTrack = (nGuiButton*) kernelServer->New("nguibutton", "NegTrack");
    n_assert(negTrack);
    if (this->horizontal)
    {
        negTrack->SetMinSize(vector2(0.0f, this->arrowBtnSize.y));
        negTrack->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
        this->AttachForm(negTrack, Top, 0.0f);
        this->AttachWidget(negTrack, Left, negBtn, 0);
        this->AttachWidget(negTrack, Right, knob, 0);
    }
    else
    {
        negTrack->SetMinSize(vector2(this->arrowBtnSize.x, 0.0f));
        negTrack->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
        this->AttachForm(negTrack, Left, 0.0f);
        this->AttachWidget(negTrack, Top, negBtn, 0);
        this->AttachWidget(negTrack, Bottom, knob, 0);
    }
    this->refNegTrack = negTrack;
    
    nGuiButton* posTrack = (nGuiButton*) kernelServer->New("nguibutton", "PosTrack");
    n_assert(posTrack);
    if (this->horizontal)
    {
        posTrack->SetMinSize(vector2(0.0f, this->arrowBtnSize.y));
        posTrack->SetMaxSize(vector2(1.0f, this->arrowBtnSize.y));
        this->AttachForm(posTrack, Top, 0.0f);
        this->AttachWidget(posTrack, Left, knob, 0);
        this->AttachWidget(posTrack, Right, posBtn, 0);
    }
    else
    {
        posTrack->SetMinSize(vector2(this->arrowBtnSize.x, 0.0f));
        posTrack->SetMaxSize(vector2(this->arrowBtnSize.x, 1.0f));
        this->AttachForm(posTrack, Left, 0.0f);
        this->AttachWidget(posTrack, Top, knob, 0);
        this->AttachWidget(posTrack, Bottom, posBtn, 0);
    }
    this->refPosTrack = posTrack;

    kernelServer->PopCwd();     // pop this
    
    this->UpdateLayout(this->rect);

    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->ClearAttachRules();
    if (this->refBgLabel.isvalid())
    {
        this->refBgLabel->Release();
        n_assert(!this->refBgLabel.isvalid());
    }
    if (this->refNegButton.isvalid())
    {
        this->refNegButton->Release();
        n_assert(!this->refNegButton.isvalid());
    }
    if (this->refPosButton.isvalid())
    {
        this->refPosButton->Release();
        n_assert(!this->refPosButton.isvalid());
    }
    if (this->refKnob.isvalid())
    {
        this->refKnob->Release();
        n_assert(!this->refKnob.isvalid());
    }

    // call parent class
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnRectChange(const rectangle& newRect)
{
    if (this->IsShown())
    {
        this->UpdateKnobLayout(newRect);
    }
    nGuiFormLayout::OnRectChange(newRect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlider2::OnFrame()
{
    // need to change knob layout?
    if (this->IsShown() && this->rangeDirty)
    {
        n_assert(this->overallSize > 0);
        this->rangeDirty = false;
        this->UpdateKnobLayout(this->rect);
        this->UpdateLayout(this->rect);
    }
    nGuiFormLayout::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Updates the layout attachment offsets of the knob. Must be called
    when any of the range values or the widget's size changes.
*/
void
nGuiSlider2::UpdateKnobLayout(const rectangle& newSliderRect)
{
    n_assert(this->shown);

    // compute relativ knob size and knob start...
    float relKnobSize = 1.0f;
    float relKnobStart = 0.0f;
    if (this->overallSize > 0.0f)
    {
        relKnobSize  = n_saturate(this->visibleSize / this->overallSize);
        relKnobStart = n_saturate(this->visibleStart / this->overallSize);
        if ((relKnobStart + relKnobSize) > 1.0f)
        {
            relKnobStart = 1.0f - relKnobSize;
            this->visibleStart = this->overallSize * relKnobStart;
        }
        if (relKnobStart < 0.0f)
        {
            relKnobStart = 0.0f;
            this->visibleStart = 0;
        }
    }
    float relKnobEnd = relKnobStart + relKnobSize;

    // Fix the layout offsets of the knob directly in our parent class's
    // layout rules array.
    // This is a wee bit dirty, but it does the job.
    float relArrowBtnSize;
    if (this->horizontal)
    {
        relArrowBtnSize = this->arrowBtnSize.x / newSliderRect.width();
    }
    else
    {
        relArrowBtnSize = this->arrowBtnSize.y / newSliderRect.height();
    }
    float relSliderSize = 1.0f - 2 * relArrowBtnSize;
    float relNegOffset = relArrowBtnSize + (relSliderSize * relKnobStart);
    float relPosOffset = relArrowBtnSize + (relSliderSize * relKnobEnd);
    this->attachRules[this->knobNegEdgeLayoutIndex].offset = relNegOffset;
    this->attachRules[this->knobPosEdgeLayoutIndex].offset = relPosOffset;

    // send a slider changed
    nGuiEvent event(this, nGuiEvent::SliderChanged);
    nGuiServer::Instance()->PutEvent(event);
}

//------------------------------------------------------------------------------
/**
    Called when mouse is moved. Handles the knob dragging.
*/
bool
nGuiSlider2::OnMouseMoved(const vector2& mousePos)
{
    if (this->IsShown() && this->dragging)
    {
        this->HandleDrag(mousePos);
    }
    return nGuiFormLayout::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    Check for child widget events and act accordingly.
*/
void
nGuiSlider2::OnEvent(const nGuiEvent& event)
{
    nGuiFormLayout::OnEvent(event);

    if (event.GetWidget() == this->refKnob.get())
    {
        if (event.GetType() == nGuiEvent::ButtonDown)
        {
            this->BeginSliderDrag();
        }
    }
    else if (event.GetWidget() == this->refPosButton.get())
    {
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->MoveSliderPos(false);
        }
    }
    else if (event.GetWidget() == this->refNegButton.get())
    {
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->MoveSliderNeg(false);
        }
    }
    else if (event.GetWidget() == this->refNegTrack.get())
    {
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->MoveSliderNeg(true);
        }
    }
    else if (event.GetWidget() == this->refPosTrack.get())
    {
        if (event.GetType() == nGuiEvent::ButtonUp)
        {
            this->MoveSliderPos(true);
        }
    }

    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (this->dragging)
        {
            this->EndSliderDrag();
        }
    }
    else if (event.GetType() == nGuiEvent::RButtonDown)
    {
        if (this->dragging)
        {
            this->CancelSliderDrag();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the slider.
*/
bool
nGuiSlider2::Render()
{
    if (this->IsShown())
    {
        // render the background brush
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);
        return nGuiFormLayout::Render();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Begin dragging the slider.
*/
void
nGuiSlider2::BeginSliderDrag()
{
    this->dragging          = true;
    this->startDragMousePos = nGuiServer::Instance()->GetMousePos();
    this->dragVisibleStart  = this->visibleStart; 
}

//------------------------------------------------------------------------------
/**
    Finish dragging the slider.
*/
void
nGuiSlider2::EndSliderDrag()
{
    n_assert(this->dragging);
    this->dragging = false;
}

//------------------------------------------------------------------------------
/**
    Cancel a knob dragging action.
*/
void
nGuiSlider2::CancelSliderDrag()
{
    n_assert(this->dragging);
    this->dragging = false;
    this->visibleStart = this->dragVisibleStart;
    this->UpdateKnobLayout(this->rect);
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
    Handle a knob dragging action.
*/
void
nGuiSlider2::HandleDrag(const vector2& mousePos)
{
    n_assert(this->dragging);

    // convert screen space drag distance to relative drag distance
    float dragDist;
    float screenSpaceSize;
    if (this->horizontal)
    {
        dragDist = mousePos.x - this->startDragMousePos.x;
        screenSpaceSize = n_saturate(this->rect.width() - 2.0f * this->arrowBtnSize.x);
    }
    else
    {
        dragDist = mousePos.y - this->startDragMousePos.y;
        screenSpaceSize = n_saturate(this->rect.height() - 2.0f * this->arrowBtnSize.y);
    }
    if (screenSpaceSize > 0.0f)
    {
        float relDragDist = dragDist / screenSpaceSize;
        
        // update range variables
        float newVisibleStart = this->dragVisibleStart + (relDragDist * this->overallSize);
        this->visibleStart = n_clamp(newVisibleStart, 0.0f, this->overallSize - this->visibleSize);

        this->UpdateKnobLayout(this->rect);
        this->UpdateLayout(this->rect);
    }
}

//------------------------------------------------------------------------------
/**
    The slider is moved either one line or one page depending on the value
    of pageJump.
*/
void
nGuiSlider2::MoveSliderPos(bool pageJump)
{
    if (pageJump)
    {
        this->visibleStart += this->visibleSize;
    }
    else
    {
        this->visibleStart++;
    }
    
    if ((this->visibleStart + this->visibleSize) >= this->overallSize)
    {
        this->visibleStart = this->overallSize - this->visibleSize;
    }
    this->UpdateKnobLayout(this->rect);
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
    The slider is moved either one line or one page depending on the value
    of pageJump.
*/
void
nGuiSlider2::MoveSliderNeg(bool pageJump)
{
    if (pageJump)
    {
        this->visibleStart -= this->visibleSize;
    }
    else
    {
        this->visibleStart--;
    }
    
    if (this->visibleStart < 0)
    {
        this->visibleStart = 0;
    }
    this->UpdateKnobLayout(this->rect);
    this->UpdateLayout(this->rect);
}
