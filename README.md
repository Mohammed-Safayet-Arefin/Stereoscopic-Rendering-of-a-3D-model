# Stereoscopic-Rendering-of-a-3D-model

Stereoscopic rendering means rendering an image or model side by side on the same screen. It is one of the most interesting and challenging task in the field of computer graphics. With the huge advancement in the modern technology specially on the 3D displays, the importance of stereoscopic 3D view has also increased at a rapid rate It plays a huge role to visualize the interactive display of data in many applications. It also can be useful in many sectors, for example, games, entertainment, scientific visualization, architectural application and many more. Additionally, big tech. companies like Google, Microsoft and many more, put a huge effort on their research for different types of applications as well as devices to make it more convenient to the users. 

I have worked with stereoscopic rendering of a 3D model using openGL. For implementing the side by side 3D model rendering in OpenGL I need to play with asymmetric adjustment as well as viewport adjustment. For loading the 3D model on the screen I have used the Open Asset Import Library (Assimp) which is suggested by Raymond C. H. Lo on his article about stereoscopic rendering [4]. Different parts of my project are briefly described below:


Main Goals:

Basically, my target is to develop a system to draw a 3D model for both the left and right eye individually in such a way that user can see a fused image in the end while viewing through an optical combiner. The goals of my project are mentioned below:
1.	Loading a 3D object on the screen using the Asset Import Library (Assimp).
2.	Apply stereoscopic view of the 3D model on the same screen considering side by side 3D model rendering 
I.	Left eye viewport
II.	Right eye viewport
III.	Left eye projection matrix
IV.	Right eye projection matrix
V.	Left eye view matrix
VI.	Right eye view matrix
3.	Apply Phong Lighting model.
4.	Playing with 3D model by keyboard key pressing 

Loading a 3D Model:

OpenGL is a powerful cross-platform graphics API but unfortunately it does not provide any mechanism to load, save and manipulate a 3D object. That’s why I have used Asset Import Library (Assimp) in my project to load, parse and draw the 3D object. This is an open source library to process different types of 3D data formats such as .obj, .stl, .3ds etc. In my project, I have used .obj file format for the 3D object. The library is also portable, c++ compatible and supports cross-platform. So, the library can be used in any operating system. 

Working Mechanism of Assimp:

The working mechanism of ASSIMP library will be started in your program by defining these three header files:
//assimp include files for data structure and I/O mechanism
#include <cimport.h>
#include <scene.h> 
#include <postprocess.h> 

The header files will include all the data structure and I/O mechanism of Assimp library in the system. Basically, the library started working by creating a bounding box after taking the 3D format as an input. The reason behind creating the bounding box is that it will help the model to fit properly in the screen. Basically, the library works recursively throughout the whole program. It considers the whole model as a tree. Considering the tree hierarchy, it walks through and gather information from every node. The library has some recursive functions those help the program to load, parse and draw the model. 
Parse:
	Recursively walk through every node in the tree hierarchy in order to parse the whole scene.
Load:
	Recursively extract all vertices from the scene and store in a single vertex buffer array. This single vertex buffer array feeds into the GPU for further processing. 

Draw:
	Traverse the scene object from the root node and draw the meshes one piece at a time

For learning the Assimp library I followed their official documentation [1] along with another tutorial to learn how to use different data structure of the library [2]. 

Stereoscopic Rendering:

As mentioned earlier, rendering stereoscopic 3D model means rendering 3D model side by side on the same plane and view the model with different perspective using the left and right eye. There must be left eye image as well as right eye image on the same plane which will be drawn based on the perspective of left and right eye. When the viewer independently views the images with some optical combiner then he/she will be able to see the fused image on the display. The idea of implementing projection matrix is gathered by reading the article of Raymond C. H. Lo [4]. That article along with the tutorial of the Paul Bourke [3] helped me a lot to understand the idea of not only projection matrix but also the view matrix, model matrix and viewport stuffs. The success of my project largely depends on how properly and accurately I implement the stereoscopic rendering technique. In order to understand the stereoscopic rendering technique one needs to have the basic idea of following these three topics:
1.	Intraocular Distance
2.	Horizontal Parallax
3.	Stereo type for rendering based on projection matrix

Intraocular Distance:
Intraocular distance is the distance between two human eyes. This distance can be varied person by person but in general human intraocular distance is 63mm or 2.5inches. In my project, I have considered the intraocular distance of 63mm. However, two situations can be raised while considering the intraocular distance. One is when the intraocular distance is too high (hyper-stereo) and another one is when the intraocular distance is too low. In both of the cases the viewer can’t see the fused image properly and will feel discomfort while viewing through the glass.  So, we need to adjust the intraocular distance to the right amount. 

     
Horizontal Parallax:

The distance between the left eye projection and right eye projection is known as horizontal parallax [3]. There are three kinds of horizontal parallax can be found.


Projection Principle of the System:

In my system, I have implemented the principle of Asymmetric frustum perspective projection. The reason behind using this projection matrix is that it allows to set the dimensions of the frustum. So, you can create a frustum for an arbitrary screen viewed by an arbitrary eye. 

Comparison with symmetric perspective projection:  

The perspective projection we learnt in the class in symmetric perspective projection. It considers that left plane is equal to the right plane and top plane is equal to the bottom plane. That’s why it only considers the FOV, aspect ratio, near and far plane. On the other hand, you can define your own right, left bottom, top, near and far plane of the frustum in the asymmetric frustum perspective projection. However, anything you can do with Perspective projection you can also do with Asymmetric Frustum Perspective projection but not vice versa [6].  

Frustum Shift Calculation:

The most critical part is to calculate the frustum shift. Based on that calculation we need to trimmed off the extended frustum shift from the left and right eye. Again, we also need to add another portion of frustum shift with left and right plane as we also have necessary frustum shift from both of the eyes. So, basically we need to extract the unwanted frustum shift from the left and right eye and again we need to add the necessary frustum shift with both the left and right eye. 
Here, the calculation is based on the concept of similar triangle. We know that when two triangles are similar, the ratios of the lengths of their corresponding sides are equal. By considering that concept I have calculated the frustum shift. T


View Matrix for Left Eye and Right Eye:

For setting the view matrix in the project, the idea is to set the camera position and need to point it to at the middle of the screen from the two eye positions. 
Here,  
The initial camera position = (0,0,0)
Left eye camera position = (-IPD/2, 0, 0)
Right eye camera position = (IPD/2, 0 0) 
So, for the left eye we need to translate the camera position by IPD/2 along x-axis and for the right eye we need to translate the camera position by –IPD/2 along x-axis. The following diagram shows the whole situation: 

10.	Viewport for Left Eye and Right Eye:
For implementing the viewport in the stereoscopic rendering, we need to divide the whole screen by half. In this case, left half of the screen will be used by the left eye image/model and right half of the screen will be used by the right eye image/model.


Playing with Model using Keyboard:
Another part of my project is to play with the 3D model using the key from the keyboard. In this case, we need a model matrix to hold the whole model because while doing any translation and rotation of the model then we need to translate and rotate the whole model rather than any part of the model. For implementing this I considered the key pressing configuration of Assignment-3 and the tutorial of Raymond C. H. Lo [4]. My system has the following key pressing functionality: 
Key = Left:
Rotate the model along positive x-axis 
Key = Right:
Rotate the model along negative x-axis 
Key= up:
Increase the depth of the model
Key = down:
Decrease the depth of the model
Key = A:
Rotate the model along positive y-axis
Key = S:
Rotate the model along negative y-axis

Shaders:
In my program, I have used two shaders: vertex and fragment shader.
Vertex Shader:
My vertex shader performs three things in my program:
1.	Compute the Vertex Position for each vertex
2.	Monochromatic color implementation to the whole model
3.	Compute the components for the Phong lighting model

Fragment Shader:
In my fragment, I am doing two things.

First, Finalize the Monochromatic color. 

Second, implementing the phong lighting model. For the phong lighting model, I have done per fragment lighting calculation in the fragment shader by considering the ambient, diffuse and specular light components. I have used the white specular light in my project. Again, for calculating the ambient, diffuse, specular and emissive light I have considered the following equations:


Additional Requirement: 

I mentioned in my proposal that I will use an optical combiner like 3D prism glass in order to view the stereoscopic 3D model more appropriately and to understand my implementation more clearly. For this, I have used a 3D prism glass to see the fused image of my implementation. 
How to use it:
It is very easy and simple to use. I have mentioned the required steps [7] below:
1.	Put the glasses on and start the program.
2.	Face the screen straight on and focus your gaze between the two images.
3.	The ideal distance from the screen is about 24 inches (60cm).
4.	Adjust the sliding panels for your left and right eye by pushing the panel right and left accordingly. 

System Output:

I have provided three 3D objects to give as an input to my system named “bench.obj”, “DNA.obj” and “Donut.obj”.  You need to adjust the depth and initial angle of the object before loading to have an optimal view or you can adjust the object to the optimal view on the screen by playing with the keyboard key pressing after running the program. 
Output without lighting: 


System Configuration:

For developing my project, I have developed my project using xcode at Mac OS. 

Required Libraries: 
1.	Assimp 
a.	Version: 4.0.1
b.	Source:

You can install assimp on your mac. In this way, you can get all the header files and library file of assimp on your system.
                      sudo install assimp
                      
2.	Glew
    a.	Version: 2.1.0
   
3.	Glfw
    a.	Version: 3.3

Frameworks: 
You need to add two frameworks and both of them you can find built in the xcode. 
      1.	OpenGL.framework
      2.	CoreFoundation.framework

Selecting a 3D object
I have provided a folder named “3D_objects” with three 3D models. The file names of the three 3D models are bench.obj, DNA.obj, and Donut.obj. In my project, I have considered only .obj file format and tested with these three objects. You can load any 3D objects by providing the path of the object under int load(const char *model_path)  function of the code

Run the Program

After linking and providing all the required libraries, header files and paths, you need to run the program. If libraries, header files and paths are perfectly linked and provided, then the program will run successfully. You can find a full screen display with a 3D model side by side. Different 3D object has different initial position as data set for each of the 3D model is different. That’s why you need to play with the keys from the keyboard to put the 3D object under optimizing view. Finally, you can enjoy the fused 3D object by viewing the left and right objects with the 3D prism glass.

References:
 
[1] 	Assimp Official Documentation: 
            http://assimp.sourceforge.net/main_doc.html 
[2] 	Assimp Tutorial:
            https://sites.google.com/site/gsucomputergraphics/educational/load_3d_model
[3]	  Stereoscopic Documentation by Paul Bourke:
            http://paulbourke.net/stereographics/stereorender/
[4]	  Online article: “Rendering Stereoscopic 3D Models using OpenGL” by Raymond C.H.  Lo.
[5]	  Book: “OpenGL Data Visualization Cookbook” by Raymond C. H. Lo, William C. Y. Lo.
[6] 	Article: “Generalized Perspective Projection” by Robert Kooima
      http://csc.lsu.edu/~kooima/articles/genperspective/
[7] 	Adjustable 3D prism glass
	    https://www.berezin.com/3d/3dprism.html

