

### Добавление ограничивающих условий
Добавлен класс FractalFilters

Он позволяет создавать фильтры отсечения веток рекурсии для создания сложных форм и экономии оперативной памяти. 
Натруально вытекает из задачи слайсинга фрактала.
Вычисления ограничивающей сферы сознательно вынесено из MeshInstance для экономии памяти

Расчет памяти:
```C++
struct MeshInstance {
    arma::mat control_points
    std::size_t automat_state
};
```
Вес структуры - 40 байт
Вес `control_points` для куба 8*3 = 24 числа, 24 * 8 = 192 байта
Вес `automat_state` 8 байт
Общий вес - 240 байт/инстанс

|Итерация|Количество кубов|Память|
|---|---|---|
|0|1|240 байт|
|1|20|4.8 КБ|
|2|400| 96 КБ|
| ... | ... | ... |
|6|$6.4 * 10^7$| 15.3 ГБ|
|7|$1.28 * 10^9$ | 307 ГБ|

Добавление `sphere_center` и `sphere_radius` добавляет 
`double radius` 8 байт
`arma::rowvec center` (3 double) 56 байт
Что влечет увеличение структуры на 25%
Следовательно вычисление радиуса виносим из структуры в рантайм

## Вопросы/дальнейшие возможности
1. Добавление многопоточности (каждая ветка IFS может быть вычислена полностью независимо от других)
2. Слияние кубов (это бы позволило уменьшить количество точек)
3. Что если хранить fractal_filter не глобально, а в MeshTemplate тогда его можно было бы применять на каждом этапе для получени других форм.
4. ~~Добавить нормали и совещение~~ 
5. Кажется что для создания полного изображения мы можем разделить его на несоклько чатстей, которые мы будем считать по очереди что поможет дойти до 10 итераций.

## Стурктура
src/
├── core/                   # DATA & ENTITIES (Immutable fractal structures)
│   ++├── fractal_model.h     # Stores Automaton, initial_meshes, max_iteration
│   ++├── automaton.h         # Automaton transition rules
│   -+├── fractal_filters.h   # CullingFilter (plane clipping math)
│   -+├── geometry_types.h    # Only MeshInstance, MeshTemplate
│   ++└── printer.h           # Printer parameters (mm, pixels, build area)
│
├── services/               # MATH SERVICES (Heavy synchronous CPU functions)
│   ++├── fractal_generator.h # Iteration computation (evaluate_internal + OpenMP/std::execution)
│   ++├── mesh_assembler.h    # Merging instances into MeshTemplate for 3D
│   +└── layer_slicer.h      # Layer slicing math (get_plane_intersection, generate_layer)
│
├── pipelines/              # STREAM CONTROLLERS (Managing asynchronous macro-tasks)
│   ++├── interactive_viewer.h# Background thread: generator -> merger -> UI callback
│   +└── slicing_pipeline.h  # Background thread: layer loop -> generator -> slicer -> callback
│
├── renderer/               # VISUALIZATION (Graphics bound to the Main Thread and OpenGL)
│   +├── ui_structures.h     # CameraControls, PlaneSettings (needed only for rendering and UI)
│   ├── fractal_renderer.h  # 3D OpenGL renderer for fractal meshes
│   └── slice_renderer.h    # 2D OpenGL renderer for contours/lines (for slicing preview)
│
├── io/                     # FILE I/O
│   +├── json_importer.h     # Reading fractal configuration
│   +└── slice_writer.h      # (Future) Saving layers/images/G-code to disk
│
└── application/            # GLUE LAYER (Entry point)
    ├── application.h       # Main loop, ImGui, passing data from pipelines to renderer
    └── main.cpp            # Application initialization and launch

## Мелкие решения
max_iteration убран из fractal_model и перемещен на уровень выше так как 
  Automaton - чистая математике без знания о железе
  FractalModel - сущность описывающая конкретный фрактал
  Логичнее всего вынести расчет в FractalGenerator или создать MemoryBudgetService

вынести создание объекта FractalModel из FractalModel так как он не должен знать о том как грузить Json