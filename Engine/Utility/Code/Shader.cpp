#include "..\..\Header\Shader.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CShader::CShader(const CShader & rhs)
	: CComponent{ rhs }
	, m_pEffect{ rhs.m_pEffect }
{
	m_pEffect->AddRef();
}

HRESULT CShader::Ready_Shader(const wstring & strShaderFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, strShaderFilePath.c_str(), nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Begin_Shader(_uint iPassIndex)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->Begin(nullptr, 0);
	m_pEffect->BeginPass(iPassIndex);

	return S_OK;
}

HRESULT CShader::End_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->EndPass();
	m_pEffect->End();

	return S_OK;
}

HRESULT CShader::Bind_Matrix(D3DXHANDLE hParameter, const _matrix* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetMatrix(hParameter, pMatrix);	
}

HRESULT CShader::Bind_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(hParameter, pTexture);
}

HRESULT CShader::Bind_Float(D3DXHANDLE hParameter, _float fValue)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetFloat(hParameter, fValue);
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const wstring & strShaderFilePath)
{
	CShader *	pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Shader(strShaderFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CShader Create Failed");		
	}

	return pInstance;
}


CComponent * CShader::Clone()
{
	return new CShader(*this);
}

void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
