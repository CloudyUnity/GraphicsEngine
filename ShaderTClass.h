#ifndef _SHADERT_H_
#define _SHADERT_H_

#include "ShaderClass.h"
#include <variant>

class ShaderTClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	std::variant test;
};

#endif
