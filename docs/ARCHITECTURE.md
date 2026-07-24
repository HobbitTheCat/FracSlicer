## Structure
```
src/
├── core/                   # DATA & ENTITIES (Immutable fractal structures)
│   ├── automaton.h         # Automaton transition rules
│   ├── fractal_filters.h   # CullingFilter (plane clipping math)
│   ├── fractal_model.h     # Stores Automaton, initial_meshes, max_iteration
│   ├── geometry_types.h    # Only MeshInstance, MeshTemplate
│   └── printer.h           # Printer parameters (mm, pixels, build area)
│
├── encoder/                # C++ port of [msla_format v0.2.0](https://crates.io/crates/msla_format/0.2.0)
│
├── services/               # MATH & COMPUTATION SERVICES (Synchronous Heavy Operations)
│   ├── fractal_generator.h  # Parallel IFS recursive generation, culling filters & RAM budget estimation
│   ├── mesh_assembler.h     # High-performance parallel merging of MeshInstances into a unified MeshTemplate
│   ├── geometry_converter.h # Parallel conversion of MeshTemplate into OpenGL-ready vertex/index arrays
│   ├── layer_slicer.h       # Mathematical plane intersection computing contours (Edges) for a given Z-level
│   └── rasterization.h      # Scanline rasterization algorithm converting 2D edges into compressed pixel rows
│
├── pipelines/              # STREAM CONTROLLERS (Managing asynchronous macro-tasks)
│   ├── interactive_viewer.h# Background thread: generator -> merger -> UI callback
│   ├── pipe_data.h         # Data structures used as pipeline containers
│   └── slicing_pipeline.h  # Background thread: layer loop -> generator -> slicer -> callback
│
├── renderer/               # VISUALIZATION LAYER (OpenGL, Framebuffers & Rendering Engine)
│   ├── renderer.h          # Abstract base interface (IRenderer) defining the pipeline contract
│   ├── scene.h             # Framebuffer (FBO) manager aggregating multiple renderers into a offscreen texture
│   ├── shader.h            # RAII OpenGL shader compiler with uniform location caching
│   ├── ui_structures.h     # UI state structs (CameraControls, PlaneSettings) for view manipulation
│   ├── fractal_renderer.h  # 3D mesh & cutting plane renderer implementing IRenderer
│   └── slice_renderer.h    # 2D layer-by-layer slice texture stack renderer implementing IRenderer
│
├── io/                     # FILE I/O
│   ├── fractal_import.h    # Reading fractal configuration
│   └── json_read.h         # Function for reading a JSON File
│
├── application/            # GLUE LAYER (Entry point)
│   ├── application.h       # Main loop, passing data from pipelines to renderer
│   └── fractal_gui.h       # ImGui interface description
└ app_main.cpp              # Main file
```


### Adding Bounding Constraints

The `FractalFilters` class has been added.

It allows creating filters that prune recursion branches to generate complex shapes and optimize RAM usage. This naturally stems from the fractal slicing task.
Bounding sphere calculation has been deliberately moved out of `MeshInstance` to save memory.

**Memory Calculation:**

```cpp
struct MeshInstance {
    arma::mat control_points
    std::size_t automat_state
};

```

* **Base structure size** — 40 bytes
* **Weight of `control_points**` for a cube ($8 \times 3 = 24$ doubles): $24 \times 8 = 192$ bytes
* **Weight of `automat_state**` — 8 bytes
* **Total weight** — 240 bytes/instance

| Iteration | Number of Cubes | Memory |
| --- | --- | --- |
|0|1|240 octet|
|1|20|4.8 Ko|
|2|400| 96 Ko|
| ... | ... | ... |
|6|$6.4 * 10^7$| 15.3 GB|
|7|$1.28 * 10^9$ | 307 GB|

Adding `sphere_center` and `sphere_radius` adds:

* `double radius` (8 bytes)
* `arma::rowvec center` (3 doubles, 56 bytes)

This results in a 25% increase in the structure's size. Therefore, we are moving the radius calculation out of the structure and into runtime.

---

## Questions / Future Opportunities

1. **Adding multithreading** (each IFS branch can be calculated completely independently of the others).
2. **Cube merging** (this would allow us to reduce the number of points).
3. **What if we store `fractal_filter` in `MeshTemplate**` instead of globally? This would allow applying it at each step to generate different shapes.
4. ~~Add normals and lighting~~
5. **Splitting the image generation:** It seems that to create the full image, we could divide it into several parts and compute them sequentially. This would help us reach up to 10 iterations.



## Minor Decisions
max_iteration has been removed from fractal_model and moved one level up because:
- Automaton represents pure mathematics with no awareness of the hardware.
- FractalModel is an entity describing a specific fractal.
- It makes the most sense to move the calculation to FractalGenerator or create a MemoryBudgetService.