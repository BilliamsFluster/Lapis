#include "Model.h"
#include <iostream>
#include "Graphics/GraphicsManager.h"
#include "Graphics/Texture.h"
#include "Graphics/Sampler.h"
#include "Graphics/Surface.h"
#include "Graphics/ModelLoader.h"


Model::Model(ID3D11Device* device, int instanceCount)
    :m_instanceCount(instanceCount)

{
    
    m_VertexShader = std::make_shared<VertexShader>(L"d");
    m_PixelShader = std::make_shared<PixelShader>(L"d");


    if (!m_VertexShader->Initialize(device)) {
        // Handle errors
    }
    if (!m_PixelShader->Initialize(device)) {
        // Handle errors
    }
    if (!InitializeBuffers(device)) {
        std::cerr << "Error: Failed to initialize buffers." << std::endl;
    }
}

Model::~Model()
{
}

void Model::Update(float deltaTime)
{
    if (canRotate)
    {
        float rotationSpeed = 1.0f;  // Define rotation speed
        DirectX::XMFLOAT3 rotation = this->GetRotation();
        rotation.y += deltaTime * rotationSpeed;  // Increment Y rotation based on elapsed time
        this->SetRotation(rotation);
    }
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
    m_VertexShader->Bind(deviceContext);
    m_PixelShader->Bind(deviceContext);

    if (m_Texture)
    {
        m_Texture->Bind();
        m_Sampler->Bind();

    }

    unsigned int strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
    unsigned int offsets[2] = { 0, 0 };
    ID3D11Buffer* buffers[2] = { m_VertexBuffer.Get(), m_InstanceBuffer.Get() };

    deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
    deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->DrawIndexedInstanced(m_Indices.size(), m_instanceCount, 0, 0, 0);
}

void Model::UpdateInstanceData(const InstanceData& data)
{
    if (m_instanceCount > 0) {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        auto deviceContext = GraphicsManager::Get().GetDeviceContext();
        deviceContext->Map(m_InstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, &data, sizeof(InstanceData));
        deviceContext->Unmap(m_InstanceBuffer.Get(), 0);
    }
}

void Model::SetTexture(const std::string& path)
{
    m_Texture = std::make_shared<Texture>(Surface::FromFile(path));
    m_Sampler = std::make_shared<Sampler>();
}


bool Model::InitializeBuffers(ID3D11Device* device)
{
    HRESULT hr;

    // Load model data
    
    ModelLoader::LoadModel("Engine\\src\\Graphics\\Cubone.glb", m_Vertices, m_Indices);
    
    

    std::cout << "Loaded " << m_Vertices.size() << " vertices and " << m_Indices.size() << " indices.\n";

    // Instance buffer setup
    if (m_instanceCount > 0) {
        D3D11_BUFFER_DESC instanceBufferDesc = {};
        instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        instanceBufferDesc.ByteWidth = sizeof(InstanceData) * m_instanceCount;
        instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&instanceBufferDesc, nullptr, &m_InstanceBuffer);
        if (FAILED(hr)) {
            std::cerr << "Failed to create instance buffer. HRESULT: " << std::hex << hr << std::endl;
            return false;
        }
    }

    // Vertex buffer setup
    D3D11_BUFFER_DESC vertexBufferDesc = {
        static_cast<UINT>(m_Vertices.size() * sizeof(Vertex)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0,
        0,
        0
    };
    D3D11_SUBRESOURCE_DATA vertexData = { m_Vertices.data(), 0, 0 };
    hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create vertex buffer. HRESULT: " << std::hex << hr << std::endl;
        return false;
    }

    // Index buffer setup
    D3D11_BUFFER_DESC indexBufferDesc = {
        static_cast<UINT>(m_Indices.size() * sizeof(unsigned int)),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0,
        0
    };
    D3D11_SUBRESOURCE_DATA indexData = { m_Indices.data(), 0, 0 };
    hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create index buffer. HRESULT: " << std::hex << hr << std::endl;
        return false;
    }

    return true;

}