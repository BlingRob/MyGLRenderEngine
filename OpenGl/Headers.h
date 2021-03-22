#pragma once

//STL
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <list>
#include <functional>
#include <type_traits>


//opengl
#include "gl/gl.h"
//interface libs
#define SDL_MAIN_HANDLED
#include <sdl/SDL.h>
//math's libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//scence's loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

//#undef main
