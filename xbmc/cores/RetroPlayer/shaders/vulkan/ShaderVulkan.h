/*
 *  Copyright (C) 2019-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "ShaderTextureVulkan.h"
#include "cores/RetroPlayer/shaders/IShader.h"
#include "guilib/graphics/vulkan/VulkanTexture.h"
#include "rendering/vulkan/VulkanShader.h"

#include <array>
#include <stdint.h>

namespace KODI::SHADER
{
class CShaderVulkan : public IShader
{
public:
  CShaderVulkan();
  ~CShaderVulkan() override;

  // Disallow copy and move (this object owns raw GL IDs)
  CShaderVulkan(const CShaderVulkan&) = delete;
  CShaderVulkan& operator=(const CShaderVulkan&) = delete;
  CShaderVulkan(CShaderVulkan&&) = delete;
  CShaderVulkan& operator=(CShaderVulkan&&) = delete;

  // Implementation of IShader
  bool Create(unsigned int passIdx,
              std::string passAlias,
              std::string shaderPath,
              std::string shaderSource,
              ShaderParameterMap shaderParameters,
              std::vector<std::shared_ptr<IShaderLut>> luts,
              unsigned int frameCountMod = 0) override;
  void Render(IShaderTexture& sourceTexture, IShaderTexture& targetTexture) override;
  void SetSizes(const float2& nextSize,
                const float2& prevSize = float2{},
                const float2& prevTextureSize = float2{}) override;
  void PrepareParameters(IShaderTexture& sourceTexture,
                         const std::vector<std::unique_ptr<IShaderTexture>>& pShaderTextures,
                         const std::vector<std::unique_ptr<IShader>>& pShaders,
                         uint64_t frameCount) override;
  void UpdateMVP() override;

  // OpenGL interface
  void Delete();

private:
  struct UniformInputs
  {
    float2 video_size;
    float2 texture_size;
    float2 output_size;
    int frame_count;
    int frame_direction;
  };

  struct UniformFrameInputs
  {
    float2 input_size;
    float2 texture_size;
    unsigned int texture;
    std::string alias;
  };

  void UpdateUniformInputs(IShaderTexture& sourceTexture,
                           const std::vector<std::unique_ptr<IShaderTexture>>& pShaderTextures,
                           const std::vector<std::unique_ptr<IShader>>& pShaders,
                           uint64_t frameCount);
  UniformInputs GetInputData(uint64_t frameCount = 0) const;
  UniformFrameInputs GetFrameInputData(unsigned int texture) const;
  UniformFrameInputs GetFrameUniformInputs() const { return m_uniformFrameInputs; }
  void GetUniformLocs();
  void SetShaderParameters(IShaderTexture& sourceTexture);

  // Index of the video shader pass in the preset
  unsigned int m_passIdx{0};

  // Alias name for the video shader pass
  std::string m_passAlias;

  // Currently loaded shader's relative path
  std::string m_shaderPath;

  // Currently loaded shader's source code
  std::string m_shaderSource;

  // Struct with all parameters pertaining to the shader
  ShaderParameterMap m_shaderParameters;

  // Look-up textures pertaining to the shader
  std::vector<std::shared_ptr<IShaderLut>> m_luts;

  // Resolution of the input of the shader
  float2 m_inputSize;

  // Resolution of the texture that holds the input
  float2 m_inputTextureSize;

  // Resolution of the output viewport of the shader
  float2 m_outputSize;

  // Resolution of the destination rectangle of the shader
  float2 m_destSize;

  // Projection matrix
  std::array<std::array<float, 4>, 4> m_MVP;

  // Value to modulo (%) frame count with (unused if 0)
  unsigned int m_frameCountMod{0};

  unsigned int m_shaderProgram{0};

  std::array<std::array<float, 3>, 4> m_VertexCoords;
  std::array<std::array<float, 3>, 4> m_colors;
  std::array<std::array<float, 2>, 4> m_TexCoords;

  UniformInputs m_uniformInputs;
  UniformFrameInputs m_uniformFrameInputs;

  std::vector<UniformFrameInputs> m_passesUniformFrameInputs;

  int m_FrameDirectionLoc{-1};
  int m_FrameCountLoc{-1};
  int m_OutputSizeLoc{-1};
  int m_TextureSizeLoc{-1};
  int m_InputSizeLoc{-1};
  int m_MVPMatrixLoc{-1};

  std::array<unsigned int, 3> m_shaderVertexVBO{0};
  unsigned int m_shaderIndexVBO{0};
};
} // namespace KODI::SHADER
