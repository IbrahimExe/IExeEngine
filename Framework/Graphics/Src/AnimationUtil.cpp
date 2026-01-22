#include "Precompiled.h"
#include "AnimationUtil.h"

#include "Color.h"
#include "SimpleDraw.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

// Empty namespace for global helper functions isolated to the cpp file
namespace
{
    void ComputeBoneTransformsRecursive(const Bone* bone, AnimationUtil::BoneTransforms& boneTransforms)
    {
        if (bone != nullptr)
        {
            // Set the bone transform to the array of the matrices
            boneTransforms[bone->index] = bone->toParentTransform;

            // If there is a parent, apply the parent's transform too
            if (bone->parent != nullptr)
            {
                boneTransforms[bone->index] = boneTransforms[bone->index] * boneTransforms[bone->parentIndex];
            }

            // Go through the children and apply thier transforms 
            for (const Bone* child : bone->children)
            {
                ComputeBoneTransformsRecursive(child, boneTransforms);
            }
        }
    }
}

void AnimationUtil::ComputeBoneTransforms(ModelId modelId, BoneTransforms& boneTransforms)
{
    const Model* model = ModelManager::Get()->GetModel(modelId);
    if (model != nullptr && model->skeleton != nullptr)
    {
        // Resize to sync the number of bones with the matrices
        boneTransforms.resize(model->skeleton->bones.size());

        // Generate the matrices recursively starting from the root bone
        ComputeBoneTransformsRecursive(model->skeleton->root, boneTransforms);
    }
}

void AnimationUtil::DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransforms)
{
    const Model* model = ModelManager::Get()->GetModel(modelId);
    if (model != nullptr && model->skeleton != nullptr)
    {
        // Iterate through the unique bone pointers to the bones in the skeleton
        for (const auto& bone : model->skeleton->bones)
        {
            if (bone->parent != nullptr)
            {
                // Gets the bone and parent positions
                const Math::Vector3 bonePos = Math::GetTranslation(boneTransforms[bone->index]);
                const Math::Vector3 parentPos = Math::GetTranslation(boneTransforms[bone->parentIndex]);
                // Draws a line from the bone to its parent
                SimpleDraw::AddLine(bonePos, parentPos, Colors::Orange);
                // Draws a sphere at the bone position (joint location)
                SimpleDraw::AddSphere(10, 10, 0.02f, Colors::Red, bonePos);
            }
        }
    }
}
