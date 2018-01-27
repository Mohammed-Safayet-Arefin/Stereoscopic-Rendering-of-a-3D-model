// Final Project
// Name: Mohammed Safayet Arefin

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//assimp include files for data structure and I/O mechanism
#include <cimport.h>
#include <scene.h>
#include <postprocess.h>


using namespace std;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


const struct aiScene* scene;
aiVector3D scene_minimum, scene_maximum, scene_center;
int vertex_number;


//********************** Loading Object Using Asset Import Library ****************************//
//Reference Tutorial: https://sites.google.com/site/gsucomputergraphics/educational/load_3d_model


//Getting the bounding box for the scene by recursively walking through all nodes
void getting_far_near_node (struct aiNode* root_node, aiVector3D* min, aiVector3D* max, aiMatrix4x4* bound)
{
    aiMatrix4x4 previous;
    previous = *bound;
    aiMultiplyMatrix4(bound,&root_node->mTransformation);
    
    for (int mesh_get = 0; mesh_get<root_node->mNumMeshes; ++mesh_get)
    {
        struct aiMesh* mesh = scene->mMeshes[root_node->mMeshes[mesh_get]];
        
        for (int vert = 0;vert< mesh->mNumVertices; ++vert)
        {
            
            aiVector3D compare = mesh->mVertices[vert];
            aiTransformVecByMatrix4(&compare,bound);
            
            min->x = aisgl_min(min->x,compare.x);
            min->y = aisgl_min(min->y,compare.y);
            min->z = aisgl_min(min->z,compare.z);
            max->x = aisgl_max(max->x,compare.x);
            max->y = aisgl_max(max->y,compare.y);
            max->z = aisgl_max(max->z,compare.z);
        }
    }
    
    for (int child = 0; child< root_node->mNumChildren; ++child)
    {
        getting_far_near_node(root_node->mChildren[child],min,max,bound);
    }
    *bound = previous;
}
///****** Get the bounding box for the scene ********//
void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
    aiMatrix4x4 bound;
    aiIdentityMatrix4(&bound);
    min->x = min->y = min->z =  1e10f;
    max->x = max->y = max->z = -1e10f;
    getting_far_near_node(scene->mRootNode,min,max,&bound);
}


//recursively walk through the nodes and getting number of vertieces at each node

//mesh->face->indices
int getting_vertices(struct aiNode *root_node){
    int count=0;
    
    for (int get_mesh = 0; get_mesh < root_node->mNumMeshes; ++get_mesh)
    {
        struct aiMesh* mesh = scene->mMeshes[root_node->mMeshes[get_mesh]];
        for (int get_face = 0; get_face< mesh->mNumFaces; ++get_face)
        {
            struct aiFace* face = &mesh->mFaces[get_face];
            count=count+3*face->mNumIndices;
        }
    }
    
    for (int child = 0; child< root_node->mNumChildren; ++child)
    {
        count=count + getting_vertices(root_node->mChildren[child]);
    }
    return count;
}





//loading the vertex into single vertex buffer array.
int vertex_loading(struct aiNode *root_node,GLfloat *data, int vertex_count)
{
    for (int get_mesh = 0; get_mesh< root_node->mNumMeshes; ++get_mesh)
    {
        struct aiMesh* mesh = scene->mMeshes[root_node->mMeshes[get_mesh]];
        
        for (int face_get = 0; face_get < mesh->mNumFaces; ++face_get)
        {
            struct aiFace* face = &mesh->mFaces[face_get];
            for(int vert = 0;vert< face->mNumIndices; vert++)
            {
                int index = face->mIndices[vert];
                
                //getting vertices in the data array
                data[vertex_count]=(mesh->mVertices[index].x-scene_center.x);
                data[vertex_count+1]=(mesh->mVertices[index].y-scene_center.y);
                data[vertex_count+2]=(mesh->mVertices[index].z-scene_center.z);
                
                //getting normals in the data array
                data[vertex_count+3]=(mesh->mNormals[index].x);
                data[vertex_count+4]=(mesh->mNormals[index].y);
                data[vertex_count+5]=(mesh->mNormals[index].z);
                
                vertex_count=vertex_count+3;
            }
        }
    }
    
    for (int child = 0; child < root_node->mNumChildren; ++child)
    {
        vertex_count = vertex_loading(root_node->mChildren[child], data, vertex_count);
    }
    return vertex_count;
}




//***** Load the 3D File *******//

int load(const char *model_path){
    
    //The 3D model file is loaded into Assimp's data structure
    model_path = "3D_objects/bench.obj";
    
    scene = aiImportFile(model_path, aiProcessPreset_TargetRealtime_MaxQuality);
    
    
    if (scene) {
        //extract the bounding box for proper scaling to fit in the scence
        get_bounding_box(&scene_minimum,&scene_maximum);
        scene_center.x = (scene_minimum.x + scene_maximum.x)/2;
        scene_center.y = (scene_minimum.y + scene_maximum.y)/2;
        scene_center.z = (scene_minimum.z + scene_maximum.z)/2;
        
        
        vertex_number = getting_vertices(scene->mRootNode);
        return 0;
    }
    return 1;
}

///**************Draw the scene recursively draws all parts from a scene *****/
int Drawing(struct aiNode* root_node, int vertex_count){
    
    
    int total_count = vertex_count;
    for (int get_mesh=0; get_mesh< root_node->mNumMeshes; ++get_mesh)
    {
        int count=0;
        struct aiMesh* mesh = scene->mMeshes[root_node->mMeshes[get_mesh]];
        for (int get_face = 0; get_face< mesh->mNumFaces; ++get_face)
        {
            struct aiFace* face = &mesh->mFaces[get_face];
            count+=3*face->mNumIndices;
        }
        glDrawArrays(GL_TRIANGLES, total_count, count);
        total_count=total_count+count;
    }
    vertex_count = total_count;
    for (int child = 0; child< root_node->mNumChildren; ++child)
    {
        vertex_count = Drawing(root_node->mChildren[child], vertex_count);
    }
    return vertex_count;
}



//********************** OpenGl Implementation: Stereoscopic Rendering ****************//



//Global variables
GLFWwindow* window = NULL;
GLuint  vertexShaderID, // OpenGL id for the vertex shader
fragShaderID,   // OpenGL id for the fragment shader
programID;      // OpenGL id for the shader program

GLuint  bufferID,   // OpenGL id (unsigned int) for the vertex buffer
arrayID,    // OpenGL id for the vertex array
indexBufferID,
matrix_id;
glm::mat4 Model_View_Projection;
glm::vec3 rotate_to_zy(0.0f, 1.0f, 0.0f);
glm::vec3 rotate_to_zx(1.0f, 0.0f, 0.0f);


// Map of part names to colors
struct ColorEntry
{
    string part;
    float color[3];
};


//in world coords
glm::vec3 light_position;


//materials
unsigned int material_shininess;
float material_kd;
float material_ks;
float material_ka;
float material_ke;

float La;		//Ambient light intensity
float Ld;		//Diffuse light intensity
float Ls;		//Specular light intensity



// Prototypes: Rendering system management
void init();
void cleanup();

// Prototypes: Vertex buffer management

void createBuffers();
void destroyBuffers();

// Prototypes: Shader management
void createShaders();
void destroyShaders();

// Prototypes: Rendering and window callbacks
void render();
void onResize(GLFWwindow*, int, int);
void onKey(GLFWwindow* window, int keycode, int scancode, int keystate, int modifiers);


// Create context, window, and OGL buffers
void init()
{
    // Initialize GLFW
    if(!glfwInit())
    {
        cerr << "glfw failed to initialize" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Open a OpenGL window with at least an OpenGL 3.2 Core Profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    // These next two needed for OSX. If you cannot create a window, comment out
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1280, 720, "Final Project", NULL, NULL);
    if(!window)
    {
        cerr << "glfw failed to create window" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    // glew still uses a non-supported way of getting extensions in
    // Core Profiles. Have to force it to load everything using
    // glewExperimental = TRUE
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if(glewStatus != GLEW_OK)
    {
        cerr << "glew failed to initialize." << endl;
        cerr << glewGetErrorString(glewStatus) << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // Since glew uses an invalid approach, we have to clear the OpenGL error
    // stack. Otherwise, we'll detect an error even if there is not one.
    // Flush errors by calling glError (only GL_INVALID_ENUM is ignored)
    GLenum error = glGetError();
    if(!((error == GL_INVALID_ENUM) || (error == GL_NO_ERROR)))
    {
        // Something actually happned badly
        while(error != GL_NO_ERROR)
        {
            cerr << gluErrorString(error);
            error = glGetError();
        }
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    if(!GLEW_VERSION_3_2)
    {
        cerr << "glew failed to initialize OpenGL 3.2." << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    
    
    // Initialize vertex buffers, the vertex array, and the shaders
    
    light_position = glm::vec3(50,450,120);
    
    //material
    material_shininess = 100;
    material_kd = 0.5;
    material_ks = 0.5;
    material_ka=0.9;
    material_ke=0.0;
    
    
    La=0.1;		//Ambient light intensity
    Ld=0.5;		//Diffuse light intensity
    Ls=0.6;		//Specular light intensity
    
    
    scene = NULL;
    vertex_number = 0;
    createBuffers();
    createShaders();
    
    // Setup callbacks
    glfwSetKeyCallback(window, onKey);
    glClearColor(1, 1, 1, 1);
    glPointSize(3.0);
    glEnable(GL_DEPTH_TEST);
    
}

// Clean up: Shut down window system
void cleanup()
{
    if(scene)
        aiReleaseImport(scene);
    destroyShaders();
    destroyBuffers();
    
    // Close window and terminate GLFW
    glfwTerminate();
}




// TODO: Create, load, and bind your vertex & index buffers. Also
// setup any vertex attributes. Note that the geometry stores 8
// values per vertex (position x,y,z; normal x,y,z; texcoord u,v)
// as floats in that order. You only need vertices for this assignment.
// Your positions must be stored in vertex attribute 0.

float rotation_Y_axis = 0;
float rotation_X_axis = 0;
void createBuffers()
{
    //loading the model
    int model = load("");
    
    //for better viewing while start the code
    
    
    //for DNA object
    //rotation_Y_axis = 1.5;
    //rotation_X_axis = 1.5;
    
    
    //for bench object
    rotation_Y_axis = 0.3;
    rotation_X_axis = -1.2;
    
    
    
    //getting all data
    GLfloat	*data = (GLfloat*) malloc(vertex_number*sizeof(GLfloat));
    vertex_loading(scene->mRootNode, data, 0);
    
    
    //generate the Vertex Array Object
    
    glGenVertexArrays(1, &arrayID);
    glBindVertexArray(arrayID);
    
    //initialize the vertex buffer memory
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    //load data
    glBufferData(GL_ARRAY_BUFFER, vertex_number*sizeof(GLfloat), data, GL_STATIC_DRAW);
    
    
    //vertices for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    //normals for normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 0, (void*)(sizeof(float)*3));
    
    
}

// TODO: After you are done w/ your buffers, you have to deallocate them
void destroyBuffers()
{
    
    glDisableVertexAttribArray(0);
    
    //Unbind the buffer memory and delete it
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &bufferID);
    glDeleteBuffers(1, &indexBufferID);
    
    
    //Unbind the vertex array and delete it
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &arrayID);
    
}



///************************************** SHADER ****************************************** ///
// Load, compile, link and enable the shaders
void createShaders()
{
    // Create, load, and compile the vertex shader
    ifstream vertexFile("stereo.vert");
    vertexFile.seekg(0, ios_base::beg);
    ifstream::pos_type begin = vertexFile.tellg();
    vertexFile.seekg(0, ios_base::end);
    int size = 1 + static_cast<int>(vertexFile.tellg() - begin);
    GLchar *vertexShader = new GLchar[size];
    vertexFile.seekg(0, ios_base::beg);
    vertexFile.read(vertexShader, size - 1);
    vertexShader[size-1] = '\0';
    
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, (const GLchar**)&vertexShader, NULL);
    glCompileShader(vertexShaderID);
    vertexFile.close();
    delete [] vertexShader;
    
    
    // See if there are any errors
    GLsizei length;
    char log[1024];
    glGetShaderInfoLog(vertexShaderID, 1023, &length, log);
    cout << "Vertex Log: " << log << endl;
    
    // Create, load, and compile the fragment shader
    ifstream fragFile("stereo.frag");
    fragFile.seekg(0, ios_base::beg);
    begin = fragFile.tellg();
    fragFile.seekg(0, ios_base::end);
    size = 1 + static_cast<int>(fragFile.tellg() - begin);
    GLchar *fragShader = new GLchar[size];
    fragFile.seekg(0, ios_base::beg);
    fragFile.read(fragShader, size - 1);
    fragShader[size-1] = '\0';
    
    fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderID, 1, (const GLchar**)&fragShader, NULL);
    glCompileShader(fragShaderID);
    fragFile.close();
    delete [] fragShader;
    
    // See if there are any errors
    glGetShaderInfoLog(fragShaderID, 1023, &length, log);
    cout << "Fragment Log: " << log << endl;
    
    // Create the final program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragShaderID);
    
    // Associate the vertex shader with the vertex attributes:
    // Attrib array 0 stores the vertices
    
    glBindAttribLocation(programID, 0, "position");
    glBindAttribLocation(programID, 1, "normal");
    
    
    // Link and enable the program
    glLinkProgram(programID);
    glUseProgram(programID);
    
    /// **** Getting all the unifrom matrices from the Shader Program *******///
    
    //get the location for our "Model_View_Projection" uniform variable
    matrix_id = glGetUniformLocation(programID, "Model_View_Projection");
    
    
    glUniform3f(glGetUniformLocation(programID, "light_position"),light_position.x, light_position.y, light_position.z);
    
    glUniform1i(glGetUniformLocation(programID, "material_shininess"),material_shininess);
    glUniform1f(glGetUniformLocation(programID, "material_kd"),material_kd);
    glUniform1f(glGetUniformLocation(programID, "material_ks"),material_ks);
    glUniform1f(glGetUniformLocation(programID, "material_ka"),material_ka);
    glUniform1f(glGetUniformLocation(programID, "material_ke"),material_ke);
    
    glUniform1f(glGetUniformLocation(programID, "La"),La);
    glUniform1f(glGetUniformLocation(programID, "Ld"),Ld);
    glUniform1f(glGetUniformLocation(programID, "Ls"),Ls);
    
    
    
}


// Disable the shaders
void destroyShaders()
{
    // Disable the current program
    glUseProgram(0);
    
    // Detach the shaders
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragShaderID);
    
    // Destroy the shaders
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragShaderID);
    
    // Destroy the program
    glDeleteProgram(programID);
}



///******************* Stereo - Projection Matrix **************** ///



#define PI 3.14159265358979323846f


//for DNA obj
//float depth = 5.0;


//for donut/bench obj
float depth = 25;

//for Robot obj
//float depth = 2.0;



//Mean and median IPD values for adult humans are around 63 mm
float IPD = 0.63;



///**************************************** Left Eye Matrices ***************************** ///

//initial left eye camera postion and FOV
glm::vec3 left_eye_camera_position = glm::vec3(0,0,0);

//total = 180;draw a perpendicula = 90 per side, take half from that 45 then 45/180=0.25
float left_eye_initial_fov = PI*0.25;
void left_eye_projection(GLFWwindow* window, float IPD, float depth)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    //In openGL, camera is looked at negative (-) z-direction
    glm::vec3 direction_z(0, 0, -1);
    
    float aspect_ratio = (float)width/(float)height;
    
    //near clipping plane
    float near = 1.0;
    //far clipping plane
    float far = 100.0;
    
    //frustum shift
    double left_eye_frustumshift = (IPD/2)*near/depth;
    
    float top = tan(left_eye_initial_fov/2)*near;
    float bottom = -top;
    
    float left_side = aspect_ratio*top;
    float left = -(left_side-left_eye_frustumshift);
    float right = left_side+left_eye_frustumshift;
    
    glm::mat4 left_eye_projection_matrix = glm::frustum(left, right, bottom, top, near, far);
    
    
    ///******************************** Left Eye View Matrix *************************** ///
    
    
    //Set the camera position and point it at the middle of the screen from the two eye positions
    // translated by +IOD/2 along the X-axis for the Left Eye
    glm::vec3 translate_camera(IPD/2, 0, 0);
    
    glm::vec3 result(0,0,0);
    int i,j;
    for(i=0;i<3;i++)
    {
        result[i]=left_eye_camera_position[i]-direction_z[i]+translate_camera[i];
    }
    
    glm::vec3 camera_position(result);
    
    glm::vec3 l(IPD/2, 0, 0);
    for(i=0;i<3;i++)
    {
        result[i]=left_eye_camera_position[i]+translate_camera[i];
    }
    
    glm::vec3 gaze_direction (result);
    
    //up vector- y axis represents the "up" direction of the camera
    glm::vec3 up = glm::vec3(0,-1,0);
    
    glm::mat4 left_eye_view_matrix = glm::lookAt(camera_position, gaze_direction, up);
    
    
    ///*********************** Left Eye Model Matrix **************** ///
    //Refernce: https://www.packtpub.com/books/content/rendering-stereoscopic-3d-models-using-opengl
    
    glm::mat4 myModelMatrix = glm::mat4(1.0);
    myModelMatrix = glm::translate(myModelMatrix, glm::vec3(0.0, 0.0, -depth));
    float rotate_to_x = PI* rotation_X_axis;
    float rotate_to_y = PI* rotation_Y_axis;
    myModelMatrix = glm::rotate(myModelMatrix, rotate_to_y, rotate_to_zy);
    myModelMatrix = glm::rotate(myModelMatrix, rotate_to_x, rotate_to_zx);
    
    Model_View_Projection = left_eye_projection_matrix * left_eye_view_matrix * myModelMatrix;
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &Model_View_Projection[0][0]);
    
    
}


///******************************** Right Eye Matrices *********************************** ///


//initial right eye camera postion and FOV
glm::vec3 right_eye_camera_position = glm::vec3(0,0,0);
//total = 180;draw a perpendicula = 90 per side, take half from that 45 then 45/180=0.25
float right_eye_initial_fov = PI*0.25;
void right_eye_projection(GLFWwindow* window, float IPD, float depth)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    //direction vector- z axis represents the direction the camera is looking
    glm::vec3 direction_z(0, 0, -1);
    float aspect_ratio = (float)width/(float)height;
    
    //near clipping plane
    float near = 1.0;
    //far clipping plane
    float far = 100.0;
    
    double right_eye_frustumshift = (IPD/2)*near/depth;
    
    float top = tan(right_eye_initial_fov/2)*near;
    float bottom = -top;
    
    float right_side =aspect_ratio*top;
    
    float right = right_side-right_eye_frustumshift;
    float left = -(right_side+right_eye_frustumshift);
    
    glm::mat4 right_eye_projection_matrix = glm::frustum(left, right, bottom, top, near, far);
    
    
    ///****************************** Right Eye View Matrix *************************** ///
    
    
    
    //Set the camera position and point it at the middle of the screen from the two eye positions
    // translated by -IOD/2 along the X-axis for the Right Eye
    
    glm::vec3 translate_camera(IPD/2, 0, 0);
    
    glm::vec3 result(0,0,0);
    int i,j;
    for(i=0;i<3;i++)
    {
        result[i]=left_eye_camera_position[i]-direction_z[i]-translate_camera[i];
    }
    
    glm::vec3 camera_position(result);
    
    //point it at the middle of the screen from the two eye positions
    for(i=0;i<3;i++)
    {
        result[i]=left_eye_camera_position[i]-translate_camera[i];
    }
    glm::vec3 gaze_direction (result);
    
    
    //up vector- y axis represents the "up" direction of the camera
    glm::vec3 up = glm::vec3(0,-1,0);
    
    glm::mat4 right_eye_view_matrix = glm::lookAt(camera_position, gaze_direction, up);
    
    
    ///******************************** Right Model Matrix ********************************** ///
    
    //Refernce: https://www.packtpub.com/books/content/rendering-stereoscopic-3d-models-using-opengl
    glm::mat4 myModelMatrix = glm::mat4(1.0);
    myModelMatrix = glm::translate(myModelMatrix, glm::vec3(0, 0, -depth));
    float rotate_to_x = PI* rotation_X_axis;
    float rotate_to_y = PI* rotation_Y_axis;
    myModelMatrix = glm::rotate(myModelMatrix, rotate_to_y, rotate_to_zy);
    myModelMatrix = glm::rotate(myModelMatrix, rotate_to_x, rotate_to_zx);
    
    
    Model_View_Projection = right_eye_projection_matrix * right_eye_view_matrix * myModelMatrix;
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &Model_View_Projection[0][0]);
    
    
}

///*********************** Rendering Left Eye **************** ///

void rendering_left_eye(int width,int height)
{
    //left half of the screen
    left_eye_projection(window, IPD, depth);
    glViewport(0, 0, width/2, height);
    Drawing(scene->mRootNode, 0);
    
    
}
///*********************** Rendering Right Eye **************** ///

void rendering_right_eye(int width,int height)
{
    //Right half of the screen
    glViewport(width/2, 0, width/2, height);
    right_eye_projection(window, IPD, depth);
    Drawing(scene->mRootNode, 0);
    
    
    
    
}

///*********************** Main Rendering Function **************** ///

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    //rendering left eye
    rendering_left_eye(width,height);
    //rendering right eye
    rendering_right_eye(width,height);
    glfwSwapBuffers(window);
}



///*********************** key pressed **************** ///

void onKey(GLFWwindow* window, int keycode, int scancode, int keystate, int modifiers)
{
    // Only worry when released
    if(keystate == GLFW_PRESS)
        return;
    if(keycode == GLFW_KEY_ESCAPE)
    {
        cleanup();
        exit(EXIT_SUCCESS);
    }
    else if(keycode == GLFW_KEY_LEFT)
    {
        
        rotation_Y_axis=rotation_Y_axis+0.05;
    }
    else if(keycode == GLFW_KEY_RIGHT)
    {
        rotation_Y_axis=rotation_Y_axis-0.05;
    }
    else if(keycode == GLFW_KEY_A)
    {
        rotation_X_axis=rotation_X_axis+0.05;
    }
    else if(keycode == GLFW_KEY_S)
    {
        rotation_X_axis=rotation_X_axis- 0.05;
    }
    else if(keycode == GLFW_KEY_UP)
    {
        depth=depth+0.5;;
    }
    else if(keycode == GLFW_KEY_DOWN)
    {
        depth=depth-0.5;
    }
    
    
}


///*********************** MAIN function **************** ///


int main(int argc, char **argv)
{
    
    
    // Initialize OpenGL and create our window
    init();
    
    // Main loop
    int running = GL_TRUE;
    double prevTime = glfwGetTime();
    while(running)
        
    {
        // Render the scene
        render();
        
        // Check if window was closed
        glfwPollEvents();
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(window);
    }
    
    // Exit program
    cleanup();
    return EXIT_SUCCESS;
    
}
