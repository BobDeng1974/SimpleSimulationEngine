
// http://outerra.blogspot.nl/2012/11/maximizing-depth-buffer-range-and.html

#include <stdlib.h>
#include <stdio.h>

//#define GL3_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "Vec2.h"
#include "Vec3.h"
#include "Mat3.h"
#include "quaternion.h"

#include "Solids.h"
#include "Noise.h"
#include "Mesh.h"

#include "GL3Utils.h"
#include "GLObject.h"
#include "Shader.h"


Shader   * shader1;
GLObject * object1;

Shader   * shader2;
GLObject * object2;

Mesh mesh;

GLuint vao;     // vertex array object
GLuint textureID;
GLuint uloc;

GLfloat modelPos[3] = { 0.0f,  0.0f,  -5.0f };
GLfloat modelMat[9] = {
  1.0f,  0.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  0.0f,  1.0f
};

GLfloat camRot[9] = {
  1.0f,  0.0f,  0.0f,
  0.0f,  1.0f,  0.0f,
  0.0f,  0.0f,  1.0f
};


GLfloat cam_pos      [3] = { 0.0f,  0.0f,  -10.0f };
GLfloat light_pos    [3] = { 1.0f,  1.0f,   -1.0f };

GLfloat lightColor   [3] = { 1.0f,  0.9f,   0.8f  };
//GLfloat lightColor   [3] = { 1.0f,  1.0f,   1.0f  };
GLfloat diffuseColor [3] = { 1.0f,  1.0f,   1.0f  };
//GLfloat diffuseColor [3] = { 0.0f,  0.0f,   0.0f  };
GLfloat ambientColor [3] = { 0.2f,  0.2f,   0.3f  };
GLfloat specularColor[3] = { 1.0f,  1.0f,   1.0f  };
//GLfloat specularColor[3] = { 0.0f,  0.0f,   0.0f  };


float resolution[2] = {800,800};

float terrain_0[2]   = {128.0,128.0};
float terrain_size[2]= {256.0,256.0};


int ninstancs = 10;
GLfloat * instance_points;

GLfloat QUAD_verts[4][2] = {
	{  -0.9f,  -0.9f  },
	{  -0.9f,   0.9f  },
	{   0.9f,  -0.9f  },
	{   0.9f,   0.9f  } };

int WIDTH  = 800;
int HEIGHT = 800;


int mouseX, mouseY;
SDL_Window * window     = NULL;
SDL_GLContext   context = NULL;
Quat4f qCamera;

//   http://www.songho.ca/opengl/gl_projectionmatrix.html

void getPerspectiveMatrix( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, float * mat ){
    float invdx = xmax-xmin;
    float invdy = ymax-ymin;
    float invdz = zmax-zmin;
    mat[0 ]  = 2*zmin*invdx; mat[1 ] = 0;            mat[2 ] =  (xmax+xmin)*invdx;  mat[3 ] = 0;
    mat[4 ]  = 0;            mat[5 ] = 2*zmin*invdy; mat[6 ] =  (ymin+ymax)*invdy;  mat[7 ] = 0;
    mat[8 ]  = 0;            mat[9 ] = 0;            mat[10] = -(zmin+zmax)*invdz;  mat[11] = -2*zmax*zmin*invdz;
    mat[12]  = 0;            mat[13] = 0;            mat[14] = -1;                  mat[15] = 0;
}

// ============= FUNCTIONS

int frameCount = 0;
bool STOP = false;

void quit();
void die ( char const *msg );
void inputHanding ();
void init();
void draw();
void loop( int niters );


int  render_type  = 1;
bool terrain_mode = true;

void setup(){


    if ( render_type == 0      ){
        // --- vertex const color
        shader1=new Shader();
        shader1->init( "shaders/basicColor3D_vert.c", "shaders/basicColor3D_frag.c" );
    }else if ( render_type == 1 ){
        // --- shading
        shader1=new Shader();
        shader1->init( "shaders/basicShading3D_vert.c", "shaders/basicShading3D_frag.c" );
        glUseProgram(shader1->shaderprogram);
    };
	//mesh.fromFileOBJ("common_resources/turret.obj");


	/*
    object1 = new GLObject( );
    object1->nVert    = nVert;
    object1->buffs[0].setup(0,3,GL_FALSE,vertexes,'v'); // vertexes
    object1->buffs[1].setup(1,3,GL_FALSE,vertexes,'n'); // normals
    object1->init();
    */

    /*
    object1 = new GLObject( );
    object1->nVert    = nVert;
    object1->buffs[0].setup(0,3,GL_FALSE,vertexes,'v'); // vertexes
    object1->buffs[1].setup(1,3,GL_FALSE,vertexes,'n'); // normals
    object1->init();
    */

    /*
    object1 = new GLObject( );
    object1->nVert    = Solids::Octahedron_nverts;
    object1->setIndexes( Solids::Octahedron_nfaces*3, Solids::Octahedron_faces );
    printf( "nVert %i nInd %i \n", object1->nVert, object1->nInd);
    object1->buffs[0].setup(0,3,GL_FALSE,(double*)Solids::Octahedron_verts, object1->nVert, 'v'); // vertexes
    object1->buffs[1].setup(1,3,GL_FALSE,(double*)Solids::Octahedron_verts, object1->nVert, 'n'); // normals
    object1->init();
    */

    /*
    object1 = new GLObject( );
    object1->nVert    = Solids::Tetrahedron_nverts;
    object1->setIndexes( Solids::Tetrahedron_nfaces*3, Solids::Tetrahedron_faces );
    printf( "nVert %i nInd %i \n", object1->nVert, object1->nInd);
    object1->buffs[0].setup(0,3,GL_FALSE,(double*)Solids::Tetrahedron_verts, object1->nVert, 'v'); // vertexes
    object1->buffs[1].setup(1,3,GL_FALSE,(double*)Solids::Tetrahedron_verts, object1->nVert, 'n'); // normals
    object1->init();
    */

    /*
    object1 = new GLObject( );
    object1->nVert    = Solids::Icosahedron_nverts;
    object1->setIndexes( Solids::Icosahedron_nfaces*3, Solids::Icosahedron_faces );
    printf( "nVert %i nInd %i \n", object1->nVert, object1->nInd);
    object1->buffs[0].setup(0,3,GL_FALSE,(double*)Solids::Icosahedron_verts, object1->nVert, 'v'); // vertexes
    object1->buffs[1].setup(1,3,GL_FALSE,(double*)Solids::Icosahedron_verts, object1->nVert, 'n'); // normals
    object1->init();
    */

    /*
    int nVert = 3*countTris( Solids::Tetrahedron_nfaces, Solids::Tetrahedron_ngons );
    GLfloat * verts   = new GLfloat[nVert];
    GLfloat * normals = new GLfloat[nVert];
    hardFace( Solids::Tetrahedron_nfaces, Solids::Tetrahedron_ngons, Solids::Tetrahedron_faces, Solids::Tetrahedron_verts, verts, normals );
    */

    int nVert = countVerts( Solids::Icosahedron_nfaces, Solids::Icosahedron_ngons );
    GLfloat * verts   = new GLfloat[nVert*3];
    GLfloat * normals = new GLfloat[nVert*3];
    hardFace( Solids::Icosahedron_nfaces, Solids::Icosahedron_ngons, Solids::Icosahedron_faces, Solids::Icosahedron_verts, verts, normals );

    object1 = new GLObject( );
    object1->nVert    = nVert;
    object1->buffs[0].setup(0,3,GL_FALSE,verts,'v'); // vertexes
    object1->buffs[1].setup(1,3,GL_FALSE,normals,'n'); // normals
    object1->init();

        // shading
    if ( render_type == 1 ){
        uloc = glGetUniformLocation( shader1->shaderprogram, "cam_pos"       ); glUniform3fv      (uloc, 1, cam_pos      );
        uloc = glGetUniformLocation( shader1->shaderprogram, "light_pos"     ); glUniform3fv      (uloc, 1, light_pos     );
        uloc = glGetUniformLocation( shader1->shaderprogram, "lightColor"    ); glUniform3fv      (uloc, 1, lightColor    );
        uloc = glGetUniformLocation( shader1->shaderprogram, "diffuseColor"  ); glUniform3fv      (uloc, 1, diffuseColor  );
        uloc = glGetUniformLocation( shader1->shaderprogram, "ambientColor"  ); glUniform3fv      (uloc, 1, ambientColor  );
        uloc = glGetUniformLocation( shader1->shaderprogram, "specularColor" ); glUniform3fv      (uloc, 1, specularColor );
    };

    ninstancs = 10;
    instance_points = new GLfloat[3*ninstancs];
    for (int i=0; i<ninstancs; i++){
        int i3 = 3*i;
        instance_points[i3+0] = randf(-5.0,5.0);
        instance_points[i3+1] = randf(-5.0,5.0);
        instance_points[i3+2] = randf(-50.0,-1000.0);
    }

    // ------------- Terrain
	shader2=new Shader();
	if  ( terrain_mode ){ shader2->init( "shaders/terrain_vert.c", "shaders/terrain_frag.c" ); }
    else                {   shader2->init( "shaders/plain_vert.c", "shaders/texture_frag.c" ); };

    object2 = new GLObject( );
    object2->draw_mode = GL_TRIANGLE_STRIP;
	object2->nVert   = 4;
    object2->buffs[0].setup(0,2,GL_FALSE,&QUAD_verts[0],'v');
	object2->init();
    // ------------- texture
    const int imgW = 256;
    const int imgH = 256;
    unsigned int imgData [imgW*imgH];
    Vec2d pos,rot,dpos;
    rot.fromAngle( 45454*0.1 );
    for( int iy=0; iy<imgH; iy++ ){
        for( int ix=0; ix<imgW; ix++ ){
            /*
            float r = sin(ix*0.16);
            float g = sin(iy*-0.31);
            float b = sin((ix+iy)*0.1);
            r*=g*b; g=r; b=r;
            r+=1.0f; g+=1.0f; b+=1.0f;
            */
            //8, 2.0, 1.0,  0.5, 0.8, 45454, {100.0,100.0}
            //int n, double scale,  double hscale,  double fdown, double strength, int seed, const Vec2d& pos0
            pos.set(ix*5,iy*5);
            Noise::warpNoise3R( pos, rot, 0.4, 0.4, 6, dpos );
            float r = 1-(dpos.x * dpos.y)*2+0.5;
            //float r = 0.3*sin(ix*0.1)*cos(iy*0.1) + 0.2*cos(ix*0.3)*sin(iy*0.3); r+=0.5;
            float g=r; float b=r;
            imgData[ iy*imgW + ix ] =  ((int)(255*r) <<16) | ((int)(255*g)<<8) | ((int)(255*b));
        }
    }
    glGenTextures  (0, &textureID);    // Create one OpenGL texture
    glBindTexture  (GL_TEXTURE_2D, textureID); // "Bind" the newly created texture : all future texture functions will modify this texture
    glTexImage2D   (GL_TEXTURE_2D, 0,GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);   // Give the image to OpenGL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



	qCamera.setOne();
}

void draw(){
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    float camMat[16];
    //getPerspectiveMatrix( -WIDTH, WIDTH, -HEIGHT, HEIGHT, 1.0, 10.0, camMat );
    getPerspectiveMatrix( -WIDTH, WIDTH, -HEIGHT, HEIGHT, 1.0, 20.0, camMat );

    //Quat4f qCamera_; convert(qCamera,qCamera_);
    Mat3f mouseMat; qCamera.toMatrix(mouseMat);
    //printf( " (%3.3f,%3.3f,%3.3f,%3.3f) \n", qCamera.x,qCamera.y,qCamera.z,qCamera.w );
    //printf( " (%3.3f,%3.3f,%3.3f,%3.3f) \n", qCamera.x,qCamera.y,qCamera.z,qCamera.w );
    //printf( "mouseMat (%3.3f,%3.3f,%3.3f) (%3.3f,%3.3f,%3.3f) (%3.3f,%3.3f,%3.3f) \n", mouseMat.ax, mouseMat.ay, mouseMat.az,  mouseMat.bx, mouseMat.by, mouseMat.bz,   mouseMat.cx, mouseMat.cy, mouseMat.cz );

    // ============= Terrain
    glUseProgram(shader2->shaderprogram);

    glActiveTexture(GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, textureID );
    float depth[] = {0.99999};
    uloc = glGetUniformLocation( shader2->shaderprogram, "depth" );	        glUniform1fv(uloc, 1, depth  );
    uloc = glGetUniformLocation( shader2->shaderprogram, "resolution" );	glUniform2fv(uloc, 1, resolution  );
    uloc = glGetUniformLocation( shader2->shaderprogram, "texture1");       glUniform1i (uloc, 0);

    if( terrain_mode ){
        uloc = glGetUniformLocation( shader2->shaderprogram, "camMat"   ); glUniformMatrix4fv(uloc, 1, GL_FALSE, camMat   );
        uloc = glGetUniformLocation( shader2->shaderprogram, "modelMat" ); glUniformMatrix3fv(uloc, 1, GL_FALSE, (float*)&mouseMat );
        uloc = glGetUniformLocation( shader2->shaderprogram, "modelPos" ); glUniform3fv      (uloc, 1, modelPos );

        float h0     = -50.0;
        float hrange =  49.0;
        //float hmax   = h0 + hrange;
        float  dhmax  =  1.0;
        float  dtmin  =  1.0;
        GLint  maxRayIter = 200;

        uloc = glGetUniformLocation( shader2->shaderprogram, "h0"        ); glUniform1fv (uloc, 1, &h0         );
        uloc = glGetUniformLocation( shader2->shaderprogram, "hrange"    ); glUniform1fv (uloc, 1, &hrange     );
        uloc = glGetUniformLocation( shader2->shaderprogram, "dhmax"     ); glUniform1fv (uloc, 1, &dhmax      );
        uloc = glGetUniformLocation( shader2->shaderprogram, "dtmin"     ); glUniform1fv (uloc, 1, &dtmin      );
        uloc = glGetUniformLocation( shader2->shaderprogram, "maxiter"   ); glUniform1i  (uloc,     maxRayIter );

        uloc = glGetUniformLocation( shader2->shaderprogram, "size"      ); glUniform2fv(uloc, 1, terrain_size );
        uloc = glGetUniformLocation( shader2->shaderprogram, "tx0"       ); glUniform2fv(uloc, 1, terrain_0    );
        uloc = glGetUniformLocation( shader2->shaderprogram, "cam_pos"   ); glUniform3fv(uloc, 1,  cam_pos     );
        uloc = glGetUniformLocation( shader2->shaderprogram, "light_pos" ); glUniform3fv(uloc, 1, light_pos    );
    };

    object2->draw();


    // ============= Objects
    glUseProgram(shader1->shaderprogram);

    uloc = glGetUniformLocation( shader1->shaderprogram, "camMat"   ); glUniformMatrix4fv(uloc, 1, GL_FALSE, camMat   );
    uloc = glGetUniformLocation( shader1->shaderprogram, "modelMat" ); glUniformMatrix3fv(uloc, 1, GL_FALSE, (float*)&mouseMat );
    uloc = glGetUniformLocation( shader1->shaderprogram, "modelPos" ); // glUniform3fv      (uloc, 1, modelPos );

    //object1->draw();
    object1->preDraw();
    for(int i=0; i<ninstancs; i++){
        glUniform3fv( uloc, 1, instance_points+i*3 );
        //glDrawArrays( object1->draw_mode, 0, object1->nVert);
        object1->draw_instance();
    }
    object1->afterDraw();
    SDL_GL_SwapWindow(window);

}


// =============================================================
// ========== BORING DEFAULT RUTINES
// =============================================================

// FUNCTION ======	inputHanding
void inputHanding(){
    float posstep = 2.0;
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if( event.type == SDL_KEYDOWN ){
            switch( event.key.keysym.sym ){
                case SDLK_ESCAPE: quit(); break;
                case SDLK_w: terrain_0[1] +=posstep; break;
                case SDLK_s: terrain_0[1] -=posstep; break;
                case SDLK_a: terrain_0[0] -=posstep; break;
                case SDLK_d: terrain_0[0] +=posstep; break;
                case SDLK_KP_PLUS:  terrain_size[0] *=1.1; terrain_size[2] *=1.1; break;
                case SDLK_KP_MINUS: terrain_size[0] /=1.1; terrain_size[2] /=1.1; break;
                //case SDLK_r:  world.fireProjectile( warrior1 ); break;
            }
		}
		if( event.type == SDL_QUIT){ quit();  };
	}

	int dmx,dmy;
	SDL_GetMouseState( &mouseX, &mouseY );
    Uint32 buttons = SDL_GetRelativeMouseState( &dmx, &dmy);
    //printf( " %i %i \n", mx,my );
    float mouseRotSpeed = 0.01;
    if ( buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        Quat4f q; q.fromTrackball( 0, 0, -dmx*mouseRotSpeed, dmy*mouseRotSpeed );
        //printf( " %i %i  (%3.3f,%3.3f,%3.3f,%3.3f) \n", dmx,dmy, q.x,q.y,q.z,q.w );
        //qCamera.qmul_T( q );
        qCamera.qmul( q );
        //qCamera.normalize();
    }
}

void loop( int nframes ){
    for ( int iframe=1; iframe<nframes; iframe++)    {
 		if( !STOP ) draw();
		inputHanding();
		frameCount++;
        SDL_Delay(10);
    }
}

int main(int argc, char *argv[]){
    init();
	setup();
	loop( 100000 );
    quit();
    return 0;
}

void init(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) die( "Unable to initialize SDL" );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // Opengl 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2); // Opengl 3.2
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    window = SDL_CreateWindow("Tutorial2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if ( !window ) die("Unable to create window");
    context = SDL_GL_CreateContext( window );
    SDL_GL_SetSwapInterval(1);

	// vertex array object
	glGenVertexArrays(1, &vao);  				// Allocate and assign a Vertex Array Object to our handle
	glBindVertexArray(vao); 					// Bind our Vertex Array Object as the current used object
}

void quit(){
	glDeleteVertexArrays(1, &vao);
    if( context != NULL ) SDL_GL_DeleteContext( context );
    if( window  != NULL ) SDL_DestroyWindow   ( window  );
    SDL_Quit();
	exit(0);
};

void die( char const *msg ){
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}
