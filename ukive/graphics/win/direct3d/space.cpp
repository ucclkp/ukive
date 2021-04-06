// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of ukive project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "space.h"

#include <fstream>

#include "utils/number.hpp"
#include "utils/log.h"

#include "ukive/app/application.h"


namespace ukive {

    void Space::draw(ID3D11Buffer* vertices, ID3D11Buffer* indices, int structSize, int indexCount) {
        UINT vertexDataOffset = 0;
        UINT vertexStructSize = structSize;

        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());

        gdm->getD3DDeviceContext()->IASetVertexBuffers(
            0, 1, &vertices, &vertexStructSize, &vertexDataOffset);
        gdm->getD3DDeviceContext()->IASetIndexBuffer(indices, DXGI_FORMAT_R32_UINT, 0);
        gdm->getD3DDeviceContext()->DrawIndexed(indexCount, 0, 0);
    }

    void Space::setVertexShader(ID3D11VertexShader* shader) {
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());
        gdm->getD3DDeviceContext()->VSSetShader(shader, nullptr, 0);
    }

    void Space::setPixelShader(ID3D11PixelShader* shader) {
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());
        gdm->getD3DDeviceContext()->PSSetShader(shader, nullptr, 0);
    }

    void Space::setInputLayout(ID3D11InputLayout* inputLayout) {
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());
        gdm->getD3DDeviceContext()->IASetInputLayout(inputLayout);
    }

    void Space::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) {
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());
        gdm->getD3DDeviceContext()->IASetPrimitiveTopology(topology);
    }

    void Space::setConstantBuffers(
        UINT startSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
    {
        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());
        gdm->getD3DDeviceContext()->VSSetConstantBuffers(
            startSlot, NumBuffers, ppConstantBuffers);
    }

    bool Space::createVertexShader(
        const std::wstring& file_name,
        D3D11_INPUT_ELEMENT_DESC* layout,
        UINT layout_count,
        ID3D11VertexShader** vertex_shader,
        ID3D11InputLayout** input_layout)
    {
        std::ifstream reader(file_name.c_str(), std::ios::binary);
        if (!reader) {
            LOG(Log::WARNING) << "Failed to open file: " << file_name.c_str();
            return false;
        }

        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        auto charSize = utl::num_cast<size_t>(std::streamoff(reader.tellg()));
        reader.seekg(cpos);

        std::unique_ptr<char[]> buf(new char[charSize]());
        reader.read(buf.get(), charSize);

        auto gdm = static_cast<DirectXManager*>(Application::getGraphicDeviceManager());

        HRESULT hr = gdm->getD3DDevice()->CreateVertexShader(
            buf.get(), charSize, nullptr, vertex_shader);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create vertex shader: " << hr;
            return false;
        }

        hr = gdm->getD3DDevice()->CreateInputLayout(
            layout, layout_count,
            buf.get(), charSize, input_layout);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create input layout: " << hr;
            return false;
        }
        return true;
    }

    bool Space::createPixelShader(
        const std::wstring& file_name,
        ID3D11PixelShader** pixel_shader)
    {
        std::ifstream reader(file_name.c_str(), std::ios::binary);
        if (!reader) {
            LOG(Log::WARNING) << "Failed to open file: " << file_name.c_str();
            return false;
        }

        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        auto charSize = utl::num_cast<size_t>(std::streamoff(reader.tellg()));
        reader.seekg(cpos);

        std::unique_ptr<char[]> buf(new char[charSize]());
        reader.read(buf.get(), charSize);

        HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDevice()->CreatePixelShader(buf.get(), charSize, nullptr, pixel_shader);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create pixel shader: " << hr;
            return false;
        }
        return true;
    }


    ComPtr<ID3D11Buffer> Space::createVertexBuffer(
        void* vertices, UINT struct_size, UINT vertex_count)
    {
        D3D11_BUFFER_DESC vb_desc;
        D3D11_SUBRESOURCE_DATA vertex_data;

        vb_desc.Usage = D3D11_USAGE_DYNAMIC;
        vb_desc.ByteWidth = struct_size * vertex_count;
        vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vb_desc.MiscFlags = 0;
        vb_desc.StructureByteStride = 0;

        vertex_data.pSysMem = vertices;
        vertex_data.SysMemPitch = 0;
        vertex_data.SysMemSlicePitch = 0;

        ComPtr<ID3D11Buffer> vertex_buffer;
        HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDevice()->CreateBuffer(&vb_desc, &vertex_data, &vertex_buffer);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create vertex buffer: " << hr;
        }

        return vertex_buffer;
    }

    ComPtr<ID3D11Buffer> Space::createIndexBuffer(int* indices, UINT index_count) {
        D3D11_BUFFER_DESC ib_desc;
        D3D11_SUBRESOURCE_DATA index_data;

        // 设置索引缓冲描述.
        ib_desc.Usage = D3D11_USAGE_DYNAMIC;
        ib_desc.ByteWidth = sizeof(int)* index_count;
        ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ib_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ib_desc.MiscFlags = 0;
        ib_desc.StructureByteStride = 0;

        // 指向存临时索引缓冲.
        index_data.pSysMem = indices;
        index_data.SysMemPitch = 0;
        index_data.SysMemSlicePitch = 0;

        // 创建索引缓冲.
        ComPtr<ID3D11Buffer> index_buffer;
        HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDevice()->CreateBuffer(&ib_desc, &index_data, &index_buffer);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create index buffer: " << hr;
        }

        return index_buffer;
    }

    ComPtr<ID3D11Buffer> Space::createConstantBuffer(UINT size) {
        D3D11_BUFFER_DESC cb_desc;

        cb_desc.Usage = D3D11_USAGE_DYNAMIC;
        cb_desc.ByteWidth = size;
        cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cb_desc.MiscFlags = 0;
        cb_desc.StructureByteStride = 0;

        ComPtr<ID3D11Buffer> buffer;
        HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDevice()->CreateBuffer(&cb_desc, nullptr, &buffer);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create const buffer: " << hr;
            return {};
        }

        return buffer;
    }

    D3D11_MAPPED_SUBRESOURCE Space::lockResource(ID3D11Resource* resource) {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        HRESULT hr = static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDeviceContext()->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr)) {
            return mappedResource;
        }

        mappedResource.pData = nullptr;
        return mappedResource;
    }

    void Space::unlockResource(ID3D11Resource* resource) {
        static_cast<DirectXManager*>(Application::getGraphicDeviceManager())->
            getD3DDeviceContext()->Unmap(resource, 0);
    }

}