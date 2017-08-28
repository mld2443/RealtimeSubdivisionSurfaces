# Realtime Subdivision Surfaces
## Subdivision surface approximation in real-time using hardware tesselation
![desktop screenshot 2017 08 28 - 11 44 40 31](https://user-images.githubusercontent.com/5340992/29792525-496be36e-8c06-11e7-8516-a33a06eca0ca.png)
This is an upload of a final project I did for CSCE 645 Geometric Modelling. Below is the report I wrote to turn in at the time. It is an accurate representation of my knowledge of the work at the time. I have updated it to fit the Markdown syntax and for clarity now that it's no longer in the context of an assignment submission. I unfortunately did not use source control while working on this project.
### Problem Summary
The goal of my project was to utilize hardware acceleration to render smooth surfaces in real-time. I chose to adapt an animation I had written in another course to suit the needs here. The chosen work was a simulation of a springy mesh as it falls in space. This accomplishes two things: it gives the viewer something nice to look at, and also gives a dynamic surface to demonstrate the real-time nature of the hardware acceleration. 
### Previous Work
- [Approximating Subdivision Surfaces with Gregory Patches for Hardware Tessellation](http://faculty.cs.tamu.edu/schaefer/research/greg.pdf): This paper put me in the right direction. It showed me how I would use the DirextX11 API to implement a tesselator that smoothed surfaces. It was however overkill for what I was planning on doing. I did not implement gregory patches, my final result assumes regular quads with valence 4.
- [These Rastertek tutorials](http://www.rastertek.com/tutdx11.html): This site had a good deal of extremely helpful example implementations of DX11 and was the ultimate deciding factor in my choise to use the API. It was however, not very descriptive, and it took a lot more help to understand the shaders.
- [This AMD tutorial](http://twvideo01.ubm-us.net/o1/vault/gdc10/slides/Bilodeau_Bill_Direct3D11TutorialTessellation.pdf): This resource made it click for me how the tesselator actually works. Up until this point I was basically assuming the tesselator implemented an algorithm that I could manipulate and control, (and that's half true, but not in the way I thought). This resource also provided the suggestion to implement 2d smoothing algorithms like those we wrote for assignment 1, (though, the tesselator has a fixed limit of how many control points it can take, limiting the degree of our polynomials to 3).
### Description of Work
I implemented a DirectX11 renderer, and adapted a [simulation I wrote for CSCE 647](https://github.com/mld2443/Spring) to demonstrate the real-time aspect. My tesselator also computes normals and interpolates color as well. In adapting the simulation, I updated many procedures, (I wrote the code months ago and have since had more experience writing particle systems, and I reengineered things to make it simpler in code and faster in execution). 
### Results
They're better than I imagined it would be last week! That is to say, I did it! mostly...
It's not Catmull-Clark like I would have liked, but I was very pressed for time towards the end, and nearly lost all my data at one point. I however never said I was going to implement Catmull-Clark subdivision, perhaps I will on my own in the near future.
I also made changes to my simulation ealry on that I later realized (as in the night before the deadline) prevented me from adding wind to the simulation. Nonetheless, it serves it's purpose, and animates smoothly. I was able to optimize several aspects of the simulation and as a result it runs like a dream compared to the old OpenGL program I adapted it from, (never mind that the machine I'm running it is orders of magnitude more capable in every way than my laptop).
It's great to see the results since I now feel like I really understand what tesselation is and what shaders are. 
## Update (12/2015)
I got splines and interpolation working. You can download the runnable and edit domainshader.hlsl to switch between them or add different subdivision schemes.

[Video](https://drive.google.com/open?id=0B6zqLLI5AaSjX2htaGZ5UGVaLVE)

[Runnable Executable](https://github.com/mld2443/RealtimeSubdivisionSurfaces/releases/tag/1.1) *Requires DX11 runtime*
### Analysis
#### New results
Being honest, there's not a whole lot here that hasn't been done before. What I've implemented is arguably what Direct3D 11 was made for (it introduced tesselation to the DirectX API). My simulation was not hardware accelerated itself, although as I'm writing this, I realize now it could have been, (pass the particles in to the the shaders and perform calculations in either the hull or vertex shaders). 
#### Meeting Goals
In this, I succeeded. I used an API to implement a hardware accelerated surface subdivision algorithm, and I implemented a simulation to demonstrate it in real-time. 
### Future Work
I have already stated several shortcomings that I would change if I did it again. I would try to perform the particle simulation on the GPU, perhaps implement a smoothing algorithm for arbitrary shapes (using gregory patches). 
### What I've learned
I had a very poor idea of what a shader was before this, and my idea of tesselation was "add a texture to a rock." Implementing this was illuminating, and I feel like I've learned enough to describe how tesselation works to a layman.
I also had to program in a windows environment for the first time in years. I've learned how dependant on debuggers I have become with complex projects like this one. The first shader error I had, I spent a few minutes fiddling with the n-sight debugger before giving up and googling it. Debugging a shader is not as simple as a normal executable, and simply googling the errors turned up little. I fixed one bug completely by a lucky guess, (The mystery of the HullShaderInput type), and likely if that didn't happen I might not have had anything to turn in.
