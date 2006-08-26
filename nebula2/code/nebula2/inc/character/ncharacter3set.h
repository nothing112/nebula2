#ifndef N_CHARACTER3SET_H
#define N_CHARACTER3SET_H
//------------------------------------------------------------------------------
/**
    @class nCharacter3Set
    @ingroup Character

    (C) 2005 RadonLabs GmbH
*/
#include "scene/ncharacter3node.h"

class nRenderContext;
//------------------------------------------------------------------------------
class nCharacter3Set
{
public:
    /// constructor
    nCharacter3Set();
    /// destructor
    virtual ~nCharacter3Set();

    /// reads the available skins and animations from a character3 node
    void Init(nCharacter3Node* characterNode);

    /// is the character3set correctly initialized ?
    bool IsValid() const;

    /// retreive count of available skins
    int GetNumAvailableSkins() const;
    /// get names of loaded skins
    nArray<nString> GetNamesOfLoadedSkins() const;
    /// sets the visibility of the skin with the given name
    void SetSkinVisible(const nString& name, bool value);
    /// returns if the skin at index is switched on or off
    bool IsSkinVisible(const nString& name);

    /// sets the visibility of the skin at the index
    void SetSkinVisibleAtIndex(int index, bool value);
    /// returns if the skin at index is switched on or off
    bool IsSkinVisibleAtIndex(int index);

    /// retreive count of available animations
    int GetNumAvailableAnimations() const;
    /// retreive count of available animations
    int GetCurrentAnimation() const;
    /// sets the current animation
    void SetCurrentAnimation(int index);
    /// get names of animations
    nArray<nString> GetNamesOfLoadedAnimations() const;

    /// retreive count of available variations
    int GetNumAvailableVariations() const;
    /// get names of loaded variations
    nArray<nString> GetNamesOfLoadedVariations() const;
    /// set current variation
    void SetCurrentVariation(const nString& name);
    /// set current variation
    int GetCurrentVariationIndex() const;
    /// set current variation
    void SetCurrentVariationIndexed(int index);

    /// converts the skin name to an index
    int ConvertSkinNameToIndex(const nString& name);
    /// converts the variation name to an index
    int ConvertVariationNameToIndex(const nString& name);
    /// converts the animation name to an index
    int ConvertAnimationNameToIndex(const nString& name);

    /// loads a characterset from a xml file
    bool LoadCharacterSetFromXML(nCharacter3Node* characterNode, const nString& fileName);

protected:

    bool isSetUp;
    int numAvailableSkins;
    int numAvailableAnimations;
    int numAvailableVariations;
    nArray<nString> skinNames;
    nArray<nString> animationNames;
    nArray<nString> variationNames;
    nArray<bool> selectedSkins;
    int selectedAnimation;
    int selectedVariation;
};
//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter3Set::SetSkinVisibleAtIndex(int index, bool value)
{
    n_assert( (index >= 0) && (index < this->numAvailableSkins));
    this->selectedSkins[index] = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter3Set::SetSkinVisible(const nString& name, bool value)
{
    int index = this->ConvertSkinNameToIndex(name);
    if (-1 == index)
    {
        n_error("nCharacterSet3: invalid skin name: %s", name.Get());
    }
    this->SetSkinVisibleAtIndex(index,value);
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter3Set::IsSkinVisibleAtIndex(int index)
{
    n_assert( (index >= 0) && (index < this->numAvailableSkins));
    return this->selectedSkins[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter3Set::IsSkinVisible(const nString& name)
{
    int index = this->ConvertSkinNameToIndex(name);
    n_assert(index != -1);
    return this->IsSkinVisibleAtIndex(index);
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter3Set::IsValid() const
{
    return this->isSetUp;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nString>
nCharacter3Set::GetNamesOfLoadedSkins() const
{
    return this->skinNames;
}


//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::GetNumAvailableSkins() const
{
    return this->numAvailableSkins;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::GetNumAvailableAnimations() const
{
    return this->numAvailableAnimations;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::GetNumAvailableVariations() const
{
    return this->numAvailableVariations;
};


//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::GetCurrentAnimation() const
{
    return this->selectedAnimation;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::GetCurrentVariationIndex() const
{
    return this->selectedVariation;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nString>
nCharacter3Set::GetNamesOfLoadedAnimations() const
{
    return this->animationNames;
}

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nString>
nCharacter3Set::GetNamesOfLoadedVariations() const
{
    return this->variationNames;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter3Set::SetCurrentAnimation(int index)
{
    n_assert( (index >= 0) && (index < this->numAvailableAnimations) );
    this->selectedAnimation = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter3Set::SetCurrentVariationIndexed(int index)
{
    // allow -1 for "no variation"
    n_assert( (index >= -1) && (index < this->numAvailableVariations) );
    this->selectedVariation = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter3Set::SetCurrentVariation(const nString& name)
{
    this->SetCurrentVariationIndexed(this->ConvertVariationNameToIndex(name));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::ConvertSkinNameToIndex(const nString& name)
{
    int result = -1;

    int i;
    for( i = 0; i < this->skinNames.Size(); i++)
    {
        if(name == this->skinNames[i])
        {
            result = i;
            break;
        };
    };

    return result;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::ConvertVariationNameToIndex(const nString& name)
{
    int result = -1;

    int i;
    for( i = 0; i < this->variationNames.Size(); i++)
    {
        if(name == this->variationNames[i])
        {
            result = i;
            break;
        };
    };

    return result;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter3Set::ConvertAnimationNameToIndex(const nString& name)
{
    int result = -1;

    int i;
    for( i = 0; i < this->animationNames.Size(); i++)
    {
        if(name == this->animationNames[i])
        {
            result = i;
            break;
        };
    };

    return result;
};
//------------------------------------------------------------------------------
#endif
