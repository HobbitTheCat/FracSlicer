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
|6|$6.4 * 10^7$| 15.3 ГБ|
|7|$1.28 * 10^9$ | 307 ГБ|

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

## Structure
```
src/
├── core/                   # DATA & ENTITIES (Immutable fractal structures)
│   ├── fractal_model.h     # Stores Automaton, initial_meshes, max_iteration
│   ├── automaton.h         # Automaton transition rules
│   ├── fractal_filters.h   # CullingFilter (plane clipping math)
│   ├── geometry_types.h    # Only MeshInstance, MeshTemplate
│   └── printer.h           # Printer parameters (mm, pixels, build area)
│
├── services/               # MATH SERVICES (Heavy synchronous CPU functions)
│   ├── fractal_generator.h # Iteration computation (evaluate_internal + OpenMP/std::execution)
│   ├── mesh_assembler.h    # Merging instances into MeshTemplate for 3D
│   └── layer_slicer.h      # Layer slicing math (get_plane_intersection, generate_layer)
│
├── pipelines/              # STREAM CONTROLLERS (Managing asynchronous macro-tasks)
│   ├── interactive_viewer.h# Background thread: generator -> merger -> UI callback
│   └── slicing_pipeline.h  # Background thread: layer loop -> generator -> slicer -> callback
│
├── renderer/               # VISUALIZATION (Graphics bound to the Main Thread and OpenGL)
│   ├── ui_structures.h     # CameraControls, PlaneSettings (needed only for rendering and UI)
│   ├── fractal_renderer.h  # 3D OpenGL renderer for fractal meshes
│   └── slice_renderer.h    # 2D OpenGL renderer for contours/lines (for slicing preview)
│
├── io/                     # FILE I/O
│   ├── json_importer.h     # Reading fractal configuration
│   └── slice_writer.h      # (Future) Saving layers/images/G-code to disk
│
└── application/            # GLUE LAYER (Entry point)
    ├── application.h       # Main loop, ImGui, passing data from pipelines to renderer
    └── main.cpp            # Application initialization and launch
```

## Minor Decisions
max_iteration has been removed from fractal_model and moved one level up because:
- Automaton represents pure mathematics with no awareness of the hardware.
- FractalModel is an entity describing a specific fractal.
- It makes the most sense to move the calculation to FractalGenerator or create a MemoryBudgetService.