# PathTracer

This is my personally developed CPU-based path tracer. It simulates the boucing of light around a scene of objects to produce beautiful images.

* Written completly in **C**, no outside libraries used other than SDL for display of the final image
* Supports multiple models all loaded from **obj** files
* Uses a **bounding volume heiarchy** (BVH) to spatial partition the scene and speed up rendering
* Takes advantage of **SSE** on modern CPUs for ray-bounding box and soon ray-triangle intersections
* Uses all your CPU cores for close to linear performance scalling

# Examples:
Standford Dragon - 8000 triangles
![stanford dragon 8000 Triangle](https://github.com/AlexanderKubarakos/PathTracer/blob/master/dragon.png "stanford dragon 8000 Triangle")
Sphere Scene - 300 Spheres
![sphere scene](https://github.com/AlexanderKubarakos/PathTracer/blob/master/spheres.png "sphere scene")
