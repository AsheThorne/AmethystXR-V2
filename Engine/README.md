## Rules to follow for engine development

Src files (files in the `src` directory) should ONLY include headers from the `include/axr` directory.
NEVER from the `include/axr-hpp` directory.

The `include/axr-hpp` directory is ONLY for wrapping functions in `include/axr` into a c++ friendly style.
For example, wrapping functions into namespaces and classes.

Everything in `include/axr-hpp` is header only. No definitions in a `.cpp` file.

The `include/axr` directory MUST ONLY contain `.h` files. These files MUST be valid c code if they're exporting from the
axr library. They can contain c++ code if they are header only and just wrappers for libraries like spdlog.

The `include/axr-hpp` directory MUST ONLY contain `.hpp` files.

The `include` directory is for public facing header files only.
Any header file which isn't accessed by the application, must go inside the `src` directory.

- Avoid `dynamic_cast`
- Minimal polymorphism. Only for small things with minimal impact on the engine

Avoid putting `#ifdef` preprocessors around enum values. The available enums shouldn't change depending on platform or
graphics api or anything else really.

For the AXR API (include headers), use `const char*` for strings that don't need to be stored.
Use string arrays, like `char ApplicationName[AXR_MAX_APPLICATION_NAME_SIZE]` for strings
that will be stored.

### Definitions

`cleanup` functions are reserved for cleaning up the entire class.
These are typically only used in destructors, move and copy functions.

`reset` functions are for resetting another function back to before it was called.
For example, `resetSetup()` will destroy anything that was created in the `setup()` function and everything that
relies on the objects created in the `setup()`.

## Engine Assets Table

`Only If Necessary` under the `Engine Creates Defaults` column means the engine only creates it if it is required
by another asset that the application wants to use and that asset doesn't already exist.

| Engine Asset                                       | Engine Defined Name | App Accessibility | Engine Creates Defaults |
|----------------------------------------------------|:-------------------:|:-----------------:|:-----------------------:|
| AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT               |         Yes         |   Constructable   |    Only If Necessary    |
| AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG               |         Yes         |   Constructable   |    Only If Necessary    |
| AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK          |         Yes         |   Constructable   |    Only If Necessary    |
| AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT            |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG          |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG             |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_SHADER_UI_IMAGE_FRAG              |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_SHADER_UI_TEXT_FRAG               |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA         |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA        |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS        |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_GLYPHS          |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_UNIFORM_BUFFER_FONT_DATA          |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MVP_MATRIX   |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE             |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_IMAGE_UV_TESTER                   |         No          |   Constructable   |           No            |
| AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT      |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_IMAGE_SAMPLER_LINEAR_REPEAT       |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_MATERIAL_UI_RECTANGLE             |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_MATERIAL_UI_BORDER                |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE                 |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_MATERIAL_UI_TEXT                  |         Yes         |     ReadOnly      |           Yes           |
| AXR_ENGINE_ASSET_MODEL_TRIANGLE                    |         No          |   Constructable   |           No            |
| AXR_ENGINE_ASSET_MODEL_SQUARE                      |         No          |   Constructable   |           No            |
| AXR_ENGINE_ASSET_MODEL_CUBE                        |         No          |   Constructable   |           No            |
| AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE                |         Yes         |   Constructable   |           Yes           |
| AXR_ENGINE_ASSET_FONT_JETBRAINS_MONO_REGULAR       |         Yes         |   Constructable   |           No            |

If an asset has `App Accessibility` set to `Constructable` and `Engine Creates Defaults` set to `Yes`,
then it gets added to the `GlobalAssetCollection` as part of the `AxrApplication`.

If an asset has `App Accessibility` set to `ReadOnly`, then nothing gets added to any asset collection.
It gets handled internally wherever it's needed
