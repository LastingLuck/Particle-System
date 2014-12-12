#include <cstdio>
#include <map>
#include <string>
#include <fstream>

#include "Particle.h"

float lastTime = 0;
float curTime = 0;
float difTime = 0;
int vNum = 0;

float camx = -5.0f, camy = 0.0f, camz = 0.0f;
float dirx = 1.0f, diry = 0.0f, dirz = 0.0f;
float rdirx = 0.0, rdiry = 0.0f, rdirz = 1.0f;
int windowWidth = 800, windowHeight = 600;
float mSense = 60.0f;
int pointSize = 3;
std::vector<glm::vec3> modelPos;

bool DEBUG_ON = true;
GLuint InitShader(const char* gShaderFileName, const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
    //SDL_SetRelativeMouseMode(SDL_TRUE);
    
	//GLEW loads new OpenGL functions
	glewExperimental = GL_TRUE; //Use the new way of testing which methods are supported
	glewInit();
    
    GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
    
    //Load Model 1
	std::ifstream modelFile;
	modelFile.open("models/cube.txt");
	int numLines = 0;
	modelFile >> numLines;
	float* model1 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model1[i];
	}
	printf("%d\n",numLines);
	int numVerts1 = numLines/8;
    vNum = numVerts1;
	modelFile.close();
    //for(int i = 0; i < numLines; i+=8) {
    //    printf("(%f %f %f) (%f %f) (%f %f %f)\n", model1[i], model1[i+1], model1[i+2], model1[i+3], model1[i+4], model1[i+5], model1[i+6], model1[i+7]);
    //}
    int vSize = numVerts1*3;
    float triverts[vSize];
    int vIndex = 0;
    for(int i = 0; i < numLines; i+=8) {
        triverts[vIndex] = model1[i];
        triverts[vIndex+1] = model1[i+1];
        triverts[vIndex+2] = model1[i+2];
        vIndex += 3;
    }
    //for(int i = 0; i < vSize; i+=3) {
    //    printf("(%f %f %f)\n", triverts[i], triverts[i+1], triverts[i+2]);
    //}
    
    //Hardcode positions of cubes
    modelPos.push_back(glm::vec3(0.0f, 0.0f, 2.0f));
    
    //Point, plane (x, y, z), (x, y ,z), (x, y, z) x 2
    //float emitterData[21] = {0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f};
    
    float vboData[numLines+8];
    float partData[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f};
    for(int i = 0; i < numLines+8; i++) {
        vboData[i] = (i < 8) ? partData[i] : model1[i-8];
    }
    GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, (numLines+8)*sizeof(float), vboData, GL_STATIC_DRAW); //upload vertices to vbo
    
    int shaderProgram = InitShader("geometry.glsl", "vertex.glsl", "fragment.glsl");
	glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)
    printf("Shader Compiled\n");
    
    //Emitter plane
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, normalized?, stride, offset
	  //Binds to VBO current GL_ARRAY_BUFFER 
	glEnableVertexAttribArray(posAttrib);
    
    GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);
    
    glEnable(GL_DEPTH_TEST);
    SDL_Event windowEvent;
    glPointSize(pointSize);
    
    std::vector<Particle> plist;
    std::vector<bool> livelist;
    Particle p;
    Emitter* emitter = new Emitter(1000);
    emitter->setColor(glm::vec3(1, 0, 0));
    emitter->setVelocity(5.0f);
    emitter->setDirection(glm::vec3(0, 1, 1));
    emitter->setParticleLife(5.0f);
    emitter->setDirectionRand(0.1f);
    emitter->setLifeRand(0.4f);
    emitter->setColorRand(1.0f);
    emitter->setShaderProgram(shaderProgram);
    
    SDL_SetWindowTitle(window, "FPS: 60    0 Particles");
    int numFrames = 0;
    
    lastTime = SDL_GetTicks()/1000.f;
    curTime = lastTime;
    difTime = 0;
    float fpsTime = 0;
    bool testIntersect = false;
    //Map of keys to states, with 0 and 1 being left and right mouse button
    std::map<char, bool> kmap;
    bool quit = false;
    while(true) {
        while(SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) {
                quit = true;
                break;
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) {
                #ifdef DEBUG
                printf("Escape\n");
                #endif
                quit = true;
                break;
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_p) {
                if(emitter->isPaused()) {
                    printf("Unpaused\n");
                    emitter->resume();
                }
                else {
                    printf("Paused\n");
                    emitter->pause();
                }
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_s) {
                if(emitter->isRunning()) {
                    printf("Stopped\n");
                    emitter->stop();
                }
                else {
                    printf("Started\n");
                    emitter->start();
                }
            }
            else if(windowEvent.type == SDL_MOUSEBUTTONDOWN && windowEvent.button.button == SDL_BUTTON_LEFT) {
                //Shift - translate position
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[SDL_SCANCODE_LSHIFT]) {
                    kmap[2] = true;
                    kmap[0] = false;
                }
                else {
                    kmap[0] = true;
                    kmap[2] = false;
                }
            }
            else if(windowEvent.type == SDL_MOUSEBUTTONUP && windowEvent.button.button == SDL_BUTTON_LEFT) {
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[SDL_SCANCODE_LSHIFT]) {
                    kmap[2] = false;
                    kmap[0] = false;
                }
                else {
                    kmap[0] = false;
                    kmap[2] = false;
                }
            }
            else if(windowEvent.type == SDL_MOUSEBUTTONDOWN && windowEvent.button.button == SDL_BUTTON_RIGHT) {
                if(windowEvent.key.keysym.mod & KMOD_SHIFT) {
                    kmap[3] = true;
                    kmap[1] = false;
                }
                else {
                    kmap[1] = true;
                    kmap[3] = false;
                }
            }
            else if(windowEvent.type == SDL_MOUSEBUTTONUP && windowEvent.button.button == SDL_BUTTON_RIGHT) {
                if(windowEvent.key.keysym.mod & KMOD_SHIFT) {
                    kmap[3] = false;
                    kmap[1] = false;
                }
                else {
                    kmap[1] = false;
                    kmap[3] = false;
                }
            }
            else if(windowEvent.type == SDL_MOUSEMOTION && kmap[0]) { //rotate around point
                int xdist = windowEvent.motion.xrel;
                int ydist = windowEvent.motion.yrel;
                float xtheta = atan2f((float)xdist, 1.0f);
                float ytheta = atan2f((float)ydist, 1.0f);
                //printf("Theta: %f\n", -theta/25.0f);
                glm::mat4 rot = glm::rotate(glm::mat4(1.0f), -xtheta/mSense, glm::vec3(0, 1, 0));
                rot = glm::rotate(rot, -ytheta/mSense, glm::vec3(0, 0, 1));
                glm::vec4 newDir = rot * glm::vec4(dirx, diry, dirz, 1.0f);
                glm::vec4 rnewDir = rot * glm::vec4(rdirx, rdiry, rdirz, 1.0f);
                dirx = newDir.x;
                diry = newDir.y;
                dirz = newDir.z;
                rdirx = rnewDir.x;
                rdiry = rnewDir.y;
                rdirz = rnewDir.z;
            }
            else if(windowEvent.type == SDL_MOUSEMOTION && kmap[2]) { //translate point
                int xdist = windowEvent.motion.xrel;
                int ydist = windowEvent.motion.yrel;
                float scale = 0.2f;
                glm::vec3 forward = glm::vec3(dirx, diry, dirz);
                glm::vec3 right = glm::vec3(rdirx, rdiry, rdirz);
                glm::vec3 newpos = glm::vec3(camx, camy, camz);
                newpos += right * (float)(xdist * scale);
                glm::vec3 up = glm::cross(forward, right);
                newpos += up * (float)(ydist * scale);
                camx = newpos.x;
                camy = newpos.y;
                camz = newpos.z;
            }
            else if(windowEvent.type == SDL_MOUSEMOTION && kmap[1]) {
                
            }
            else if(windowEvent.type == SDL_MOUSEMOTION && kmap[3]) {
                
            }
            else if(windowEvent.type == SDL_MOUSEWHEEL) {
                int ydist = windowEvent.wheel.y;
                float scale = 0.5f;
                glm::vec3 forward = glm::vec3(dirx, diry, dirz);
                glm::vec3 newpos = glm::vec3(camx, camy, camz);
                newpos += forward * (float)(ydist * scale);
                camx = newpos.x;
                camy = newpos.y;
                camz = newpos.z;
            }
            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT) {
                emitter->setPosition(emitter->getPosition() + glm::vec3(0, 0, -1));
            }
            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT) {
                emitter->setPosition(emitter->getPosition() + glm::vec3(0, 0, 1));
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_EQUALS) {
                ++pointSize;
                glPointSize(pointSize);
                printf("Point Size: %d\n", pointSize);
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_MINUS) {
                --pointSize;
                if(pointSize < 1) {
                    pointSize = 1;
                }
                glPointSize(pointSize);
                printf("Point Size: %d\n", pointSize);
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_g) {
                emitter->toggleGravity();
                if(emitter->usingGravity()) {
                    printf("Gravity On\n");
                }
                else {
                    printf("Gravity Off\n");
                }
            }
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_i) {
                testIntersect = !testIntersect;
                if(testIntersect) {
                    printf("Collision Detection On\n");
                }
                else {
                    printf("Collision Detection Off\n");
                }
            }
        }
        if(quit) {
            break;
        }
        
        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        #ifdef DEBUG
        printf("Update\n");
        #endif
        glm::mat4 model;
        glm::mat4 view = glm::lookAt(
            glm::vec3(camx, camy, camz),  //Cam Position
            glm::vec3(camx+dirx, camy+diry, camz+dirz),  //Look at point
            glm::vec3(0.0f, 1.0f, 0.0f)); //Up
        GLint uniView = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
      
        glm::mat4 proj = glm::perspective(3.14f/2, 800.0f / 600.0f, 0.1f, 25.0f); //FOV, aspect, near, far
        GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        int num = emitter->render();
        //Objects
        GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
        GLint uniModel = glGetUniformLocation(shaderProgram, "model");
        for(int i = 0; i < (signed)modelPos.size(); i++) {
            glUniform3f(uniColor, 0.5f, 0.5f, 0.5f);
            
            model = glm::mat4();
            model = glm::translate(model, modelPos[i]);
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 1, numVerts1);
        }
        
        lastTime = curTime;
        curTime = SDL_GetTicks()/1000.f;
        difTime = curTime - lastTime;
        
        if(testIntersect) {
            emitter->update(triverts, vSize, modelPos, numVerts1);
        }
        else {
            emitter->update(0, 0, modelPos, numVerts1);
        }
        
        numFrames++;
        fpsTime += difTime;
        if(fpsTime > 1.0f) {
            //printf("FPS: %f\n", numFrames / fpsTime);
            SDL_SetWindowTitle(window, ("FPS: " + std::to_string(numFrames/fpsTime) + "    " + std::to_string(num).c_str() + " Particles").c_str());
            fpsTime -= 1.0f;
            numFrames = 0;
        }
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);
    
	//Clean Up
	SDL_GL_DeleteContext(context);
	SDL_Quit();
    
    return 0;
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* gShaderFileName, const char* vShaderFileName, const char* fShaderFileName)
{
	GLuint geometry_shader, vertex_shader, fragment_shader;
	GLchar *gs_text, *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
    gs_text = readShaderSource(gShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
    if(gs_text == NULL) {
        printf("Failed to read from geometry shader file %s\n", gShaderFileName);
		exit(1);
    }
    else if(DEBUG_ON) {
        printf("Geometry Shader:\n=====================\n");
		printf("%s\n", gs_text);
		printf("=====================\n\n");
    }
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders
	GLint compiled;
	// Check for errors
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
    
    // Load Geometry Shader
    const char *gg = gs_text;
	glShaderSource(geometry_shader, 1, &gg, NULL);  //Read source
	glCompileShader(geometry_shader); // Compile shaders
    //Check for errors
	glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Geometry shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(geometry_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(geometry_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	
	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	
	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
    glAttachShader(program, geometry_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);
	glUseProgram(program);

	return program;
}

void Win2PPM(int width, int height){
	char outdir[10] = "out/"; //Must be defined!
	int i,j;
	FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;
    
    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
      fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }
    
    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"ERROR: Failed to open file for window capture\n");
    }
    
    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
    
    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
    }
    
    free(image);
    fclose(fptr);
    counter++;
}
