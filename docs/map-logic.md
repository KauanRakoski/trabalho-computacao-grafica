# Crash Kart: Map Logic & Architecture

This document outlines the architecture and technical logic behind the "Once Upon a Tire" track map implementation in the Crash Kart Racing Clone. The implementation is primarily contained within `TrackMap.h` and `TrackMap.cpp`.

## 1. Map Loading & Initialization

### OBJ and MTL Parsing
The track map is stored as a massive `.obj` file (`Once Upon A Tire.obj`), containing 237 different shapes and 488 unique materials.
- We utilize `tinyobjloader` to parse the `.obj` and its associated `.mtl` (Material) file.
- **Path Resolution:** The MTL file specifies the textures needed for each material. A common issue is spaces in filenames (e.g., `Once Upon A Tire.mtl`), which break parsing. To resolve this, spaces in the `mtllib` declaration and the material file name itself were replaced with underscores.

### Texture Registration
Textures are loaded dynamically during the map initialization phase using `stb_image`:
- As the `.obj` is parsed, the program reads the `diffuse_texname` (or `ambient_texname` as a fallback) for each shape.
- `TrackMap::LoadTexture` binds the image data directly to OpenGL using `GL_TEXTURE_2D`, sets the correct wrapping (`GL_REPEAT`) and filtering parameters (`GL_LINEAR_MIPMAP_LINEAR`), and stores the generated OpenGL Texture ID in an internal dictionary to avoid loading duplicate textures.

### Geometry Pipeline
Each "shape" inside the `.obj` file is converted into an OpenGL Vertex Array Object (VAO):
- **Vertex Data:** Vertices are transformed immediately by the model matrix (specifically scaled by `0.05f`) so that the collision system operates entirely in world space.
- **Attributes:** The coordinates (model), normal vectors, and UV (texture) mapping coefficients are structured into interleaved buffers (VBOs) and an element array buffer (EBO).
- **Fallback Data:** If a shape lacks normals or UV coordinates, fallback values `(0, 1, 0)` and `(0, 0)` are injected to prevent buffer misalignment which would cause black textures.

## 2. Rendering

The rendering pipeline in `TrackMap::Draw()` is heavily optimized to handle the massive geometry:
- It loops over all pre-calculated shapes.
- For each shape, it binds the associated VAO.
- It activates `GL_TEXTURE0` and binds the specific Texture ID for that shape.
- A single `glDrawElements` call pushes the geometry to the GPU using the `MAP_SHADER_ID`.

Because the map consists of dozens of independent textures mapped to different shapes, this loop ensures each segment of the track is drawn with its appropriate material (e.g., dirt, wood, brick).

## 3. Collision System

Handling collisions on a highly complex, non-uniform track geometry required pivoting away from Axis-Aligned Bounding Boxes (AABBs).

### Why Map AABBs Were Disabled
Originally, the collision system iterated over the `aabb` bounding boxes of all 237 map shapes. However, shapes in `.obj` files are often grouped arbitrarily (e.g., an entire curved ramp). The AABB of a ramp is a massive rectangular box that encompasses the highest and lowest points of the ramp. If Crash drove into this empty space, the AABB collision would push him away, effectively trapping him inside invisible walls.
For this reason, standard AABB wall collisions were explicitly disabled for the track map, allowing the Kart to move freely.

### Ray-Triangle Intersection (Floor Detection)
To keep the kart anchored to the ground and allow smooth driving up slopes and down hills, a custom Ray-Triangle intersection logic was implemented in `TrackMap::GetFloorHeight()`.

1. **Spatial Filtering:** The function filters triangles based on a rapid 2D AABB check in the XZ plane to quickly discard geometry far from the Kart.
2. **Normal Filtering (Ceiling & Wall Rejection):** 
   - A surface is only considered a "Floor" if its normal points upwards (`normal.y > 0.1f`).
   - Walls (`normal.y` near 0) and Ceilings (`normal.y` pointing downwards) are explicitly ignored to prevent the kart from snapping to the roof of castles or tunnels.
3. **Barycentric Point Check:** The function projects the Kart's 3D position down to the 2D XZ plane (`PointInTriangleXZ`) and uses barycentric signs (Cross Product) to determine if the Kart is hovering directly above/below the triangle.
4. **Plane Equation:** If the Kart is inside the triangle boundaries, the exact Y height (`Py`) of the triangle surface at the Kart's XZ coordinate is calculated using the plane equation:
   `Py = tri.v0.y - (N.x * (Px - tri.v0.x) + N.z * (Pz - tri.v0.z)) / N.y`
5. **Jump Threshold:** To handle ramps, the calculated height `Py` is only accepted if it is below `world_pos.y + 0.3f`. This generous `0.3f` threshold allows Crash to drive up steep stairs and slopes without accidentally teleporting to low-hanging overhead geometry.

### Integration in `main.cpp`
Every frame, the physics loop calls `GetFloorHeight(crashPos)`. If a floor is found, and Crash is close enough to it, his vertical velocity is zeroed out, and his position is clamped smoothly to the floor height, creating a seamless racing experience across complex terrain.
