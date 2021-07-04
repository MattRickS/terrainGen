The goal is to have everything in an openGL UI. This would mean dynamically building the mesh from settings. It should therefore be possible to tweak a layer and have everything else update, how could the grid support this?

# Configuration
Configuration files for different profiles:

## Materials
Can configure the different types, eg
```
[HardRock]
...

[Soil]
...
```

## Terrain
Layering settings for how to build a starting terrain grid. Should allow use of noise and existing source images.
```
[HardRock]
add noise fractal ...
subtract noise voronoi ...

[Soil]
file /...
```

## Erosion
Settings for the type(s) of erosion to apply
```
[Rain]
...

[Wind]
...

[WaterSource]
```


    // TODO: Should this have a smarter iterator type that can handle sections?
    //       Eg, it = iterator(fromX, fromY, toX, toY) - and it++ knows to wrap around?
    //       Don't actually need it for this, would just be cool.


The grid corners are the vertices which is the actual terrain. Use bilinear interpolation to calculate the heights.
Might be able to use these newer values for more accurate normals?
Each particle iteration should extend the normal to the end of the cell - no point nudging it within the cell while the normal remains unchanged (technically other particles could modify it while this is happening if we parallelised, but if there's that many particles interacting on the same cell then we have other problems to deal with).
Use the integer voxel stepping (same as used in the nuke voxel nodes, but for 2D) to ensure we always hit the edge (I believe this doesn't use any costly length/sqrt calculations).

grid.normal(vec3f pos)
    // bilinear interpolation ? Shouldn't make enough of a difference to be worth it.

Now we should only ever need ~(width + height) iterations (technically possible to go higher if there was a lot of spiralling/winding) before we hit a stable point.


Need to have the concept of water building up (or existing water bodies being filled and sediment deposited) as particles stop. If doing this though, the water needs to be capable of flowing out again if a hole/channel opens up. Off the top of my head.
    Water could be a layer type, but with some way of identifying it's surface area.
    Particles hitting the water are absorbed and distribute their volume over the surface area, potentially filling in neighbouring cells.
    Sediment from "landed" particles is distributed within a radius of where it contacted - this should be done before adding the particle water height as the displacement will also add to it.
    When terrain is eroded down, it can check for adjacent water segments, and if there are any it can compare the height difference to determine how many particles to convert the difference into (this wouldn't work well for punching a hole at the bottom of lake, but that's a very long term problem).



Vector map for water flow.
As particle enters a new cell, it calculates the normal and blends it's target vector with the vector map, then updates the vector map to blend towards the new target vector.
Ie, the particle is influenced by the current flow, and then itself influences the flow.
    Is it possible for the vector map to ever differ from the normal-vector calculations? It will initially be seeded by the normal map after all. Deposition and Erosion will change the height of a cell and it's neighbours, and therefore the normal. The water flow would then be slowly adjusting.

