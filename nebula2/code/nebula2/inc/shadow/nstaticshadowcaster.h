#ifndef N_STATICSHADOWCASTER_H
#define N_STATICSHADOWCASTER_H
//------------------------------------------------------------------------------
/**
    @class nStaticShadowCaster
    @ingroup Shadow

    A shadow caster for static geometry.

    (C) 2004 RadonLabs GmbH
*/
#include "shadow/nshadowcaster.h"

//------------------------------------------------------------------------------
class nStaticShadowCaster : public nShadowCaster
{
public:
    /// constructor
    nStaticShadowCaster();
    /// destructor
    virtual ~nStaticShadowCaster();

protected:
    /// get the face normals
    virtual vector3* GetFaceNormals() const;
    /// get the coordiantes
    virtual vector3* GetCoords() const;
    /// get num coordiantes
    virtual int GetNumCoords() const;
    
    /// perform actual resource unloading
    virtual void UnloadResource();
    
    /// perform data load from source mesh
    virtual void LoadShadowData(nMesh2* sourceMesh);

private:
    /// read vertex data from source mesh
    void LoadVerticies(nMesh2* sourceMesh);
    /// generate face normals
    void CreateFaceNormales();

    /// array of the face normals of the current load triangels
    vector3* faceNormales;
    int numFaceNormales;

    /// coordiantes of the all groups of the shadow geometry, is shared via resource system
    nRef<nMesh2> refCoordMesh;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector3*
nStaticShadowCaster::GetFaceNormals() const
{
    n_assert(0 != this->faceNormales)
    return this->faceNormales;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStaticShadowCaster::GetNumCoords() const
{
    return this->refCoordMesh->GetNumVertices();
}
//------------------------------------------------------------------------------
#endif