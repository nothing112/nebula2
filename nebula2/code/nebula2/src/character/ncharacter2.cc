//------------------------------------------------------------------------------
//  ncharacter2.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharacter2.h"

vector4 nCharacter2::scratchKeyArray[MaxCurves];
vector4 nCharacter2::keyArray[MaxCurves];
vector4 nCharacter2::transitionKeyArray[MaxCurves];

//------------------------------------------------------------------------------
/**
*/
nCharacter2::nCharacter2() :
    animStateArray(0),
    animation(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2::nCharacter2(const nCharacter2& src) :
    animStateArray(0),
    animation(0)
{
    *this = src;
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2::~nCharacter2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set a new animation state, and handle stuff necessary for
    blending between previous and current state.
*/
void
nCharacter2::SetActiveState(int stateIndex, nTime time)
{
    n_assert(ValidStateIndex(stateIndex));

    // only overwrite previous state if the current state was active 
    // for at least a little while to prevent excessive "plopping"
    if (this->curStateInfo.IsValid())
    {
        nAnimState& curAnimState = this->animStateArray->GetStateAt(this->curStateInfo.GetStateIndex());
        float fadeInTime = curAnimState.GetFadeInTime();
        if ((time - this->curStateInfo.GetStateStarted()) > fadeInTime)
        {
            this->prevStateInfo = this->curStateInfo;
        }
    }
    else
    {
        // no valid current state
        this->prevStateInfo = this->curStateInfo;
    }
    this->curStateInfo.SetStateIndex(stateIndex);
    this->curStateInfo.SetStateStarted(float(time));
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter2::EvaluateSkeleton(float time, nVariableContext* varContext)
{
    n_assert(this->curStateInfo.IsValid());
    n_assert(this->animStateArray);
    n_assert(this->animation);

    // check if a state transition is necessary
    nAnimState& curAnimState = this->animStateArray->GetStateAt(this->curStateInfo.GetStateIndex());
    float curRelTime = time - this->curStateInfo.GetStateStarted();
    
    // handle time exception (this happens when time is reset to a smaller value
    // since the last anim state switch)
    if (curRelTime < 0.0f)
    {
        curRelTime = 0.0f;
        this->curStateInfo.SetStateStarted(time);
    }

    float fadeInTime = curAnimState.GetFadeInTime();
    float lerp = 1.0f;
    bool transition = false;
    if ((fadeInTime > 0.0f) && (curRelTime < fadeInTime) && this->prevStateInfo.IsValid())
    {
        // state transition is necessary, compute a lerp value
        // and sample the previous anim state
        nAnimState& prevAnimState = this->animStateArray->GetStateAt(this->prevStateInfo.GetStateIndex());
        float prevRelTime = time - this->prevStateInfo.GetStateStarted();
        if (prevAnimState.Sample(prevRelTime, this->animation, varContext, transitionKeyArray, scratchKeyArray, MaxCurves))
        {
            transition = true;
            lerp = curRelTime / fadeInTime;
        }
    }

    // get samples from current anim state
    if (curAnimState.Sample(curRelTime, this->animation, varContext, keyArray, scratchKeyArray, MaxCurves))
    {
        // transfer the sampled animation values into the character skeleton
        int numJoints = this->charSkeleton.GetNumJoints();
        int jointIndex;
        const vector4* keyPtr = keyArray;
        const vector4* prevKeyPtr = transitionKeyArray;

        vector3 translate, prevTranslate;
        quaternion rotate, prevRotate;
        vector3 scale, prevScale;
        for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
        {
            // read sampled translation, rotation and scale
            translate.set(keyPtr->x, keyPtr->y, keyPtr->z);          keyPtr++;
            rotate.set(keyPtr->x, keyPtr->y, keyPtr->z, keyPtr->w);  keyPtr++;
            scale.set(keyPtr->x, keyPtr->y, keyPtr->z);              keyPtr++;

            if (transition)
            {
                prevTranslate.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z);              prevKeyPtr++;
                prevRotate.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z, prevKeyPtr->w);  prevKeyPtr++;
                prevScale.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z);                  prevKeyPtr++;
                translate.lerp(prevTranslate, lerp);
                rotate.slerp(prevRotate, rotate, lerp);
                scale.lerp(prevScale, lerp);
            }

            nCharJoint& joint = this->charSkeleton.GetJointAt(jointIndex);
            joint.SetTranslate(translate);
            joint.SetRotate(rotate);
            joint.SetScale(scale);
            joint.Evaluate();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Finds a state index by name. Returns -1 if state not found.
*/
int
nCharacter2::FindStateIndexByName(const nString& n)
{
    return this->animStateArray->FindStateIndex(n);
}

//------------------------------------------------------------------------------
/**
    Returns the duration of an animation state in seconds.
*/
nTime
nCharacter2::GetStateDuration(int stateIndex) const
{
    n_assert(this->ValidStateIndex(stateIndex));
    n_assert(this->animation);
    n_assert(this->animStateArray);

    int animGroupIndex = this->animStateArray->GetStateAt(stateIndex).GetAnimGroupIndex();
    nTime dur = this->animation->GetDuration(animGroupIndex);
    return dur;
}

//------------------------------------------------------------------------------
/**
    Returns the fadein time of an animation state in seconds.
*/
nTime
nCharacter2::GetStateFadeInTime(int stateIndex) const
{
    n_assert(this->ValidStateIndex(stateIndex));
    n_assert(this->animStateArray);
    return this->animStateArray->GetStateAt(stateIndex).GetFadeInTime();
}
