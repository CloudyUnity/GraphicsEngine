#ifndef _FRUSTUMCLASS_H_
#define _FRUSTUMCLASS_H_

#include <directxmath.h>
using namespace DirectX;

class FrustumClass
{
public:
    FrustumClass();
    FrustumClass(const FrustumClass&);
    ~FrustumClass();

    void ConstructFrustum(XMMATRIX, XMMATRIX, float);

    bool CheckPoint(float, float, float);
    bool CheckCube(float, float, float, float);
    bool CheckSphere(float, float, float, float);
    bool CheckRectangle(float, float, float, float, float, float);

    void Shutdown();

private:
    XMFLOAT4 m_planes[6];
};

#endif
