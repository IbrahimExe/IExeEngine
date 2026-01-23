#pragma once

#include "ModelManager.h"

namespace IExeEngine::Graphics::AnimationUtil
{
    // Defining a vector of bone matrices to use for skeleton calculations
    using BoneTransforms = std::vector<Math::Matrix4>;

    // Compute all the matricies for all the bones in the hierarchy
    void ComputeBoneTransforms(ModelId modelId, BoneTransforms& boneTransforms);
    
    // To be called after ComputeBoneTransforms, draws the skeleton hierarchy
    void DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransforms);

    // To be called to apply bone offsets for skinning data
    void ApplyBoneOffsets(ModelId modelId, BoneTransforms& boneTransforms);
}