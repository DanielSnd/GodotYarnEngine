# GodotYEngine
Collection of helpful classes, variables, nodes, resources and methods for making games in the godot engine, as a C++ Godot Module (targetting 4.3+)


# YarnEngine

YarnEngine is a comprehensive custom module for Godot that enhances the engine with a variety of new functionalities, nodes, and resources. It aims to streamline game development workflows and provide powerful tools for creators.

## Features

### Asset Management and Import

- **AOBakeableMeshInstance**: Allows for ambient occlusion baking on vertex colors for meshes.
  - Customizable baking parameters like vertices per batch, max milliseconds per batch, occlusion distance, colors and angle.

- **ResourceImporterGLBasMesh**: Custom importer for GLB/GLTF files as meshes.
  - Imports GLTF/GLB files directly as ArrayMesh resources.
  - Options for scaling, offsetting, and applying materials during import.
  - Supports batch processing of multiple meshes within a single file.
  - Option for saving a new .mesh file after importing so once it has been processed the .glb/.gltf file can be removed from the project. 

### Visual Elements and Textures

- **CircleWithConeTexture2D**: A custom 2D texture generator for creating circle + cone shapes. (Initially made to represent a field of fiew in a top down 2d game.)
  - Highly customizable with properties for circle radius, cone angle, smoothing, and colors.
  - Real-time updates and preview in the editor.
 
- **CombinedTexture2D**: A texture that combines multiple textures with various blending options.
  - Supports layering of back, front, and overlay textures.
  - Customizable modulation and cropping for each layer.

### Scene Management and Spawning

- **YSceneSpawner3D**: A node for spawning scenes in 3D space with various options.
  - Customizable spawn area and visualization in the editor.

- **YSpecialPoint3D**: A system for creating and managing special points in 3D space.
  - Useful for pathfinding, spatial queries, and gameplay mechanics.
  - Customizable priority system for points.

### Animation and Skeletal Attachments

- **YBoneAttachment3D**: An enhanced extension of Godot's BoneAttachment3D.
  - Supports external skeleton references.
  - Options for overriding poses and ignoring scale.

### Editor Tools and Plugins

- **AOBakeEditorPlugin**: Editor plugin for ambient occlusion baking.
  - Integrates AO baking tools into the Godot editor interface.

- **CatButtonsEditorPlugin**: Adds custom buttons and categories to the editor inspector.
  - Allows for easy addition of custom function calls directly from the inspector.
  - Supports both buttons and categories for organizing inspector UI.

- **GSheetImporterEditorPlugin**: Google Spreadsheet Importer Editor Plugin allows for importing data from Google Sheets directly into your Godot project:
	- **Dynamic Script Loading**: Automatically loads importer scripts from a designated folder.
	- **Custom Import Logic**: Each importer script can define its own import logic and data processing.
	- **Editor Integration**: Adds menu items to the Godot editor for each valid importer script.
	- **Flexible Data Handling**: Can import various types of data and create corresponding resources or game objects.

### Gizmos and Visualization

- **YSpecialPoint3DGizmoPlugin**: Gizmo for YSpecialPoint3D nodes.
  - Visualizes special points in the 3D editor with customizable icons.
  - Color-coded based on point priority.

- **YSceneSpawner3DGizmoPlugin**: Gizmo for YSceneSpawner3D nodes.
  - Visualizes the spawn area in the 3D editor.
  - Customizable debug visualization options.

### Utility Classes

- **YBoxContainer**, **YHBoxContainer**, **YVBoxContainer**: Enhanced versions of Godot's container nodes. Supports automatic or callback modes for tweening contents.

### YarnPhysics

YarnPhysics simplifies physics operations in both 2D and 3D environments:

- **Simplified Physics Casts**: Provides easier-to-use wrappers for raycasts, shape casts, and overlap tests.
- **One-line Calls**: Allows developers to perform complex physics queries with single-line function calls, similar to GDScript's convenience methods.
- **2D and 3D Support**: Offers simplified physics functions for both 2D and 3D scenes.

### YarnTime

YarnTime is a comprehensive time management system that offers both real-time and in-game time tracking:

- **Dual Time Tracking**:
  - Real-time clock with pausable and non-pausable versions.
  - In-game clock system for managing game world time.
- **Customizable In-game Time**:
  - Configurable days per month and months per year.
  - Custom month names support.
- **Time Formatting**:
  - Various methods for formatting time (e.g., `clock_formatted`, `clock_formatted_day_month_year`).
- **Time-based Callbacks**:
  - Register callbacks to trigger at specific in-game times.
- **Time Elapsed Checks**:
  - Methods to check if a certain amount of time has passed (both for real-time and in-game time).

### YSave

YSave is a robust save system that integrates with YarnTime to provide time-aware saving and loading functionality:

- **Flexible Data Storage**:
  - Save and retrieve data using key-value pairs.
  - Support for nested data structures using "/" in keys.
- **JSON Serialization**:
  - Automatically serializes save data to JSON format.
- **Time-based Event System**:
  - Register and track time-based events.
  - Set callbacks for when specific events occur.
- **Save Details**:
  - Ability to store and retrieve metadata about saves.
- **Automatic Saving**:
  - Option to trigger saves at specified intervals or on demand.

#### Integration between YSave and YarnTime

- **Event Registration**: YSave can register events that are tied to YarnTime's in-game clock.
- **Time-based Callbacks**: Callbacks registered in YSave can be triggered based on YarnTime's clock progression.
- **Persistent Time Tracking**: Game time from YarnTime can be saved and loaded using YSave, ensuring continuity across game sessions.

### YTween
YTween is an extension to Godot's built-in Tween functionality:

- **Singleton-based Management**: Centralized control of all tweens through a singleton.
- **Unique Tweens**: 
  - `create_unique_tween`: Creates a tween that cancels any existing tweens for the same object and tag.
- **Simplified Tween Creation**: Provides easy-to-use methods for common tween operations:
  - `tween_scale`: Animate object scaling.
  - `tween_alpha`: Animate transparency.
  - `tween_modulate`: Animate color modulation.
  - `tween_global_position`: Animate global position.
  - `tween_position`: Animate local position.
  - `tween_position_rotation`: Animate position and rotation simultaneously.
  - `tween_position_rotation_scale`: Animate position, rotation, and scale simultaneously.
- **Automatic Cleanup**: Tweens are automatically removed when finished or when their target node is freed.
- **Tagging System**: Allows grouping and managing tweens with integer tags.
- **"Finished or Killed" Signal**: Provides an extra signal callbacks for tweens that are either finished OR killed as opposed to the regular finished signal.

### YState and YStateOverride State Machine System

The YState and YStateOverride classes form a powerful and flexible state machine system for game logic and AI behavior.

**Key Concepts**

1. **Auto-Override System**: 
   - States can automatically transition based on configurable conditions.
   - Supports weighted random selection among valid auto-override states.

2. **Conditional Transitions**: 
   - States can define pass and fail conditions for automatic transitions.

3. **Property-Based Checks**: 
   - Auto-override conditions can be based on property values of specified nodes.

4. **State Targeting**: 
   - The state machine can target specific nodes or positions, useful for AI behaviors.

5. **Override Mechanism**: 
   - YStateOverride allows for external control over state transitions, useful for event-driven behavior changes.
   
#### YStateMachine

The core of the state machine system, managing states and their transitions.

Key features:
- Automatic state discovery and setup
- Configurable processing intervals
- Support for server-only execution in multiplayer scenarios
- Built-in debugging options
- Customizable through virtual methods

#### YState

Represents individual states within the state machine.

Key features:
- Customizable enter, exit, and process behaviors
- Conditional state transitions
- Auto-override system for dynamic state changes
- Property-based condition checking
- Cooldown and weight system for state selection
- Support for ignoring or requiring specific states

#### YStateOverride

Provides additional control over state transitions outside of the normal flow.

Key features:
- Can override current state based on custom conditions
- Ignores specific states to prevent unwanted transitions
- Connects to the state machine's override attempt system

## YarnEngine Game State Management System

The YarnEngine module includes a powerful and flexible game state management system, designed specifically for turn-based games. This system consists of three main components: YGameState, YGameAction, and YGamePlayer, which work together to create a robust, deterministic, and easily synchronizable game flow.

### Key Components

#### YGameState

YGameState is the central manager for the game's state and flow. It handles:

- Player registration and management
- Action queue management (future, current, and past actions)
- Turn management
- State serialization and deserialization for save/load functionality
- Processing of game actions and their steps

Key features:
- Manages lists of future, overriding, and past game actions
- Handles action priorities for flexible game flow control
- Supports serialization for easy save/load and network synchronization
- Provides hooks for custom game logic implementation

#### YGameAction

YGameAction represents a single game action or turn. It includes:

- Action parameters for storing action-specific data
- A series of steps that make up the action
- Methods for action execution, including enter, process, and exit
- Serialization support for network play and save/load functionality

Key features:
- Step-based execution allows for granular control and pausing
- Support for instant execution for immediate actions
- Priority system for action ordering
- Customizable through virtual methods for game-specific logic

#### YGamePlayer

YGamePlayer represents a player in the game, storing player-specific data and state.

### How It Works

1. **Game Setup**: Create a YGameState and register YGamePlayer instances.
2. **Action Creation**: Define game actions by subclassing YGameAction and implementing game-specific logic in GDScript.
3. **Action Queueing**: Add actions to the YGameState's future or overriding action queues.
4. **Action Execution**: YGameState processes actions in order, executing their steps and managing turns.
5. **State Management**: Use YGameState's serialization methods to save/load game state or synchronize across network.

### Key Features

1. **Deterministic Execution**: Actions and steps are executed in a deterministic order, ensuring consistent results across saves, loads, and network play as long as you write the actions using only deterministic code.

2. **Flexible Action System**: 
   - Actions can have multiple steps, allowing for complex, multi-part turns.
   - Steps can be paused, waiting for player input or other game events.

3. **Priority-based Action Ordering**: Actions can be assigned priorities, allowing for dynamic reordering of the action queue.

4. **Override Actions**: Special actions can be inserted to override normal game flow, useful for handling special events or interrupts.

5. **Serialization Support**: The entire game state, including all actions and their steps, can be serialized and deserialized, enabling:
   - Easy save/load functionality
   - Efficient network synchronization for multiplayer games
   - Replay systems

6. **Turn Management**: Built-in support for managing player turns in turn-based games.

7. **Extensibility**: The system is designed to be easily extended with game-specific GDScript logic through virtual methods and signals.

## YThreader: Easy Multithreading Methods
YThreader simplifies multithreading in Godot projects. It provides an easy-to-use interface for queuing and executing jobs across multiple threads.

### Key Features
1. **Automatic Thread Pool**: YThreader creates and manages a pool of worker threads, automatically scaling to the number of available processors or a user-defined count.
2. **Job Queue System**: Easily queue callables (functions/methods) to be executed on separate threads.
3. **Thread-Safe Operations**: Includes built-in mutex locks for thread-safe operations and custom mutex for user-defined critical sections.
4. **Main Thread Checking**: Provides a static method to check if the current code is running on the main thread.
5. **Interruptible Jobs**: Includes a YThreaderInterrupter class for cancelling long-running jobs.

## Asset Placer: 3D Scene Placement Tool

The Asset Placer is a powerful editor tool integrated into the YarnEngine module, designed to streamline the process of placing 3D assets in your Godot scenes. It provides an intuitive interface for selecting, previewing, and placing 3D objects directly in the 3D viewport.

### Key Features

1. **Bottom Panel Interface**: A custom panel in the editor's bottom dock for easy access to asset selection and placement options.
2. **Asset Preview**: Displays thumbnails of available 3D assets for quick selection.
3. **Live Preview**: Shows a real-time preview of the selected asset in the 3D viewport as you move your mouse.
4. **Placement Options**:
   - Align to ground
   - Random rotation (Y-axis and Z-axis)
   - Random scaling
   - Offset adjustment
   - Multiple object placement
5. **Radius-based Placement**: Place objects within a specified radius for natural-looking distribution.
6. **Layer-based Placement**: Option to lock placement to specific collision layers.
7. **Undo/Redo Support**: Fully integrated with Godot's undo/redo system for easy mistake correction.
8. **Multi-asset Selection**: Ability to randomly choose from multiple selected assets during placement.

### Usage

1. **Accessing the Tool**: The Asset Placer panel appears in the bottom dock when in the 3D editor.

2. **Selecting Assets**: 
   - Click on asset thumbnails in the bottom panel to select them for placement.
   - Multi-select assets for random placement from the selection.

3. **Placement Preview**:
   - Move your mouse in the 3D viewport to see a live preview of where the asset will be placed.
   - The preview shows both the asset and the placement radius.

4. **Placing Assets**:
   - Left-click in the 3D viewport to place the selected asset(s).
   - Hold left-click to continuously place assets.

5. **Adjusting Settings**:
   - Use the bottom panel controls to adjust placement options like rotation, scale, and alignment.

6. **Cancelling Placement**:
   - Press ESC to deselect the current asset and exit placement mode.

## YarnMenu: Advanced Menu System

YarnMenu is a powerful and flexible menu system designed for the YarnEngine module. It provides a robust foundation for creating interactive and dynamic menu interfaces in your Godot projects.

### Key Features

1. **Menu Stack Management**: 
   - Automatically manages a stack of active menus.
   - Ensures only the top menu is interactive.

2. **Smooth Transitions**: 
   - Built-in fade-in and fade-out animations for smooth menu transitions.

3. **Back Button Handling**: 
   - Automatic back button functionality with customizable behavior.
   - Optional "ui_cancel" action binding for back functionality.

4. **Menu Instantiation Helpers**: 
   - Easy methods to instantiate child or replacement menus.

5. **Auto-Start Functionality**: 
   - Option to automatically start the menu when it enters the scene tree.

6. **Centralized Button Click Management**: 
   - Prevents rapid button clicking with a built-in cooldown system.

7. **GDScript Integration**: 
   - Virtual methods for easy extension in GDScript.

8. **Signal System**: 
   - Emits signals for key menu events (e.g., started, going back).

## Inspector Buttons

Thanks to [Valla-Chan for the inspiration](https://github.com/Valla-Chan/Godot-Valla-ExportHelper/tree/main) (and GDScript code I ended up using as base). I believe it isn't possible to add Buttons like this to any script using just GDScript without making the script you're adding the buttons to a *@tool*, so I recreated this implementation in this module working around that limitation.

### Usage:
This will create a button in the inspector that will call the button function.
```gdscript
@export var _btn_button:int

func button():
	pass
```
Essentially you make an export variable that starts with `_btn_yourfunctionname`.

![Example gif](http://yarncatgames.com/images/Example_Inspector_button.gif)

## Installation

1. Clone or download the YarnEngine repository.
2. Copy the `yarnengine` folder into your Godot project's `modules` directory.
3. Rebuild Godot from source with the new module included.

## Usage

After compiling the engine with the module, the new nodes, resources, and editor tools will be available in your Godot project. For builds you will need to also compile the export templates for the desired platforms.


## License

### Licensed under MIT License https://opensource.org/license/mit

## Acknowledgements

YarnEngine is a module for the Godot Engine. We are grateful to the Godot community for their incredible work.
