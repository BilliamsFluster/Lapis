#include "VertexShader.h"
#include <iostream>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include "Graphics/GraphicsManager.h"

bool VertexShader::Initialize(ID3D11Device* device, std::string entryPoint, std::string shaderModel)
{
    HRESULT hr = this->CompileShader(entryPoint, shaderModel);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to compile the pixel shader. HRESULT: " << hr << std::endl;
        return false;
    }
    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    hr = D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
    // Create the pixel shader
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf());
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create the vertex shader. HRESULT: " << hr << std::endl;
        return false;
    }
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
        
        {"INSTANCEWORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEWORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEWORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"INSTANCEWORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}

    };
    
    device->CreateInputLayout(
        ied, (UINT)std::size(ied),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &m_InputLayout
    );
    GraphicsManager::Get().GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
    return true;
	
}

void VertexShader::Bind(ID3D11DeviceContext* deviceContext)
{
    deviceContext->IASetInputLayout(m_InputLayout.Get());
    deviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);

}