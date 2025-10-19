# High Priority

## Graphics

### UI

- [ ] Correct issues with OpenXR UI
- [ ] Implement UI components to use in world space
- [ ] Implement billboard ui effect
- [ ] Look into instance rendering ui elements

# Low Priority

## Graphics

- [ ] Frustum culling
- [ ] TODO: Allow multiple descriptor sets.

  https://stackoverflow.com/questions/56928041/what-is-the-purpose-of-multiple-setlayoutcounts-of-vulkan-vkpipelinelayoutcreate
  "A descriptor set layout describes the layout for a single descriptor set.
  But a pipeline can have multiple descriptor sets.
  This is what the layout(set = #) part of the qualifier in GLSL means:
  it specifies which set that this particular resource gets its descriptor from.
  The set is an index into the VkPipelineLayoutCreateInfo::pSetLayouts array.
  The descriptor is the index into that set's list of descriptors.
  The two of them combined identify the specific descriptor within the pipeline layout."

  https://www.reddit.com/r/vulkan/comments/zzspjn/need_clarification_regarding_descriptor_sets/
  "The general idea is that you create a few tables (descriptor sets) of data that a pipeline needs to run.
  The layout of these is entirely up to you, but you can design it so that your different pipelines can reuse
  the data by making the layouts of your descriptor sets compatible."

  I think this means you could have 1 set which contains scene data, and another for object specific data.
  Then you can reuse the scene descriptor set and data for every object and the object has it's own data.
  But to accomplish that, would be a bigger change i suspect.

  I don't think it's even really that necessary for it to share descriptor set layouts.
  Is there really a downside to just creating them again for difference materials?
  I'm pretty sure the system is designed in such a way that data (for example scene data) can be shared.
  And even if we create the same descriptor set layout multiple times, it just means they can differ slightly
  if desired right?
  And if they don't differ, is it really worth the extra effort to share them?
  Maybe it is if it turns out to be fairly simple. I just don't think it will.
  The most compelling reason is that it will improve performance.

  Oh, also, I believe this is vulkan only. OpenGL doesn't have "sets" so layout(set = #) wouldn't work there.
  So make sure it's a feature that's only available for vulkan. Like push constants.

  descriptor set layout rules: can't have a missing index?
  Example: 3 layouts with the indexes 0, 2, 4. Invalid. Missing 1 and 3

  setLayoutCount must be less than or equal to VkPhysicalDeviceLimits::maxBoundDescriptorSets

  I think we need to modify vkCmdBindDescriptorSets when this is no longer hard coded to 1.

  https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
  Section 12.2.3. Vulkan Only: Descriptor Sets

  https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/
  Section Frequency-based descriptor sets:

  A more Vulkan centric renderer would organize data that the shaders need to access into groups by frequency
  of change, and use individual sets for individual frequencies, with set=0 representing least frequent change,
  and set=3 representing most frequent. For example, a typical setup would involve:
  Set=0 descriptor set containing uniform buffer with global, per-frame or per-view data,
  as well as globally available textures such as shadow map texture array/atlas
  Set=1 descriptor set containing uniform buffer and texture descriptors for per-material data,
  such as albedo map, Fresnel coefficients, etc.
  Set=2 descriptor set containing dynamic uniform buffer with per-draw data, such as world transform array

- [ ] Generating mip maps at runtime isn't recommended. We should only do this as a last resort if the application
  requests it and if the image doesn't already have baked in mipmaps.

### Window Graphics

- [ ] Pass the oldSwapchain param in vk::SwapchainCreateInfoKHR when recreating the swapchain.

  https://docs.vulkan.org/spec/latest/chapters/VK_KHR_surface/wsi.html#wsi-swapchain
