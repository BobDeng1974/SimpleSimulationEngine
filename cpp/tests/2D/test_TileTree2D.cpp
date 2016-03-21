
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>

#include "fastmath.h"
#include "Vec2.h"
#include "geom2D.h"
#include "TileTree2D.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "Draw2D.h"
#include "AppSDL2OGL.h"
#include "testUtils.h"

// ======================  TestApp

#define CELL_TYPE int

class TestAppTileTree2D : public AppSDL2OGL {
	public:
	//TileTree2D<GridCell,2,3,5> map;
	TileTree2D_d<CELL_TYPE,3,200,300> map;

    bool mouse_left = false;
    bool mouse_right = false;

	// ---- function declarations

	void         drawMap( );

	void   set_speed( int ntry, double xmin, double ymin, double xmax, double ymax );
	double get_speed( int ntry, double xmin, double ymin, double xmax, double ymax );

	virtual void draw   ();
	void eventHandling( const SDL_Event& event );
	TestAppTileTree2D( int& id, int WIDTH_, int HEIGHT_ );

};

TestAppTileTree2D::TestAppTileTree2D( int& id, int WIDTH_, int HEIGHT_ ) : AppSDL2OGL( id, WIDTH_, HEIGHT_ ) {
	printf( " %i %i   %i %i %i \n", map.nsub, map.sub_mask, map.ntotx, map.ntoty, map.ntotxy );

    double step = 0.5;
    map.setSpacing( step, step );

    long t1,t12;
    int n_sets = 1000000;
	t1 = getCPUticks();
    set_speed( n_sets, -50.0, -50.0, 50.0, 50.0 );
	t12 = getCPUticks() - t1;
	printf( " set_speed %3.3f ticks/iter | %3.3f Mticks iterations %i \n", t12/((double)n_sets), t12*1.0e-6, n_sets );

    int n_gets = 1000000;
	t1 = getCPUticks();
    get_speed( n_gets, -50.0, -50.0, 50.0, 50.0 );
	t12 = getCPUticks() - t1;
	printf( " get_speed %3.3f ticks/iter | %3.3f Mticks iterations %i \n", t12/((double)n_gets), t12*1.0e-6, n_gets );

}

void TestAppTileTree2D::draw(){
    glClearColor( 0.5f, 0.5f, 0.5f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH );

    if      ( mouse_left  ){ ( *map.getValidPointer_d( mouse_begin_x, mouse_begin_y,  0 ) ) = 255; }
    else if ( mouse_right ){ ( *map.getValidPointer_d( mouse_begin_x, mouse_begin_y,  0 ) ) = 0;   }

    drawMap();

};

void TestAppTileTree2D::drawMap(){
    for( int iy=0; iy<map.ny; iy++ ){
		for( int ix=0; ix<map.nx; ix++ ){
		    auto ptile = map.tiles[ map.isup2D( ix, iy ) ];
            if( ptile != NULL ){
                double xi = map.getX( (ix<<map.power) );
                double yi = map.getY( (iy<<map.power) );
                glColor3f( 0, 0, 0 );
                Draw2D::z_layer = 0.0f;
                Draw2D::drawRectangle_d( {xi,yi}, {xi+map.xStep*map.nsub,yi+map.yStep*map.nsub}, true );
                Draw2D::z_layer = 1.0f;
                for( int jy=0; jy<map.nsub;  jy++ ){
                    double y = map.getY( (iy<<map.power) + jy );
                    for( int jx=0; jx<map.nsub; jx++ ){
                        double x = map.getX( (ix<<map.power) + jx );
                        CELL_TYPE* pcell= ptile->getPointer( jx, jy );
                        if( *pcell != 0 ){
                            float c = *pcell/255.0f;
                            glColor3f( c, c, c );
                            Draw2D::drawRectangle_d( {x,y}, {x+map.xStep,y+map.yStep}, true );
                        }
                    }
                }
            }
		}
    }
}

void TestAppTileTree2D::set_speed( int ntry, double xmin, double ymin, double xmax, double ymax ){
    double sum = 0;
    double xsc = ( xmax - xmin )/65536.0d;
    double ysc = ( ymax - ymin )/65536.0d;
    int h = rand_hash2( 2147483647 );
    for( int i=0; i<ntry; i++ ){
        h = rand_hash( h );
        double x = ( ( h&0xFFFF          ) ) * xsc + xmin;
        double y = ( ((h&0xFFFF0000)>>16 ) ) * ysc + ymin;
        int val  =   (h&0xFF0000)>>16 ;

        CELL_TYPE* pcell = map.getValidPointer_d( x, y, 0 );
        (*pcell) = val;

        sum += x + y + val;
    }
    printf( " sum %e \n", sum );
}

double TestAppTileTree2D::get_speed( int ntry, double xmin, double ymin, double xmax, double ymax ){
    double sum = 0 ;
    double xsc = ( xmax - xmin )/65536.0d;
    double ysc = ( ymax - ymin )/65536.0d;
    int h = rand_hash2( 2147483647 );
    for( int i=0; i<ntry; i++ ){
        h = rand_hash( h );
        double x = ( ( h&0xFFFF          ) ) * xsc + xmin;
        double y = ( ((h&0xFFFF0000)>>16 ) ) * ysc + ymin;
        int val  =   (h&0xFF0000)>>16 ;

        CELL_TYPE* pcell = map.getPointer_d( x, y );
        if( pcell != NULL ){
            sum += *pcell;
        }

        sum += x + y + val;
    }
    printf( " sum %e \n", sum );
}

void TestAppTileTree2D::eventHandling( const SDL_Event& event ){
    switch( event.type ){
        case SDL_MOUSEBUTTONDOWN:
            switch( event.button.button ){
                case SDL_BUTTON_LEFT:   mouse_left  = true; break;
                case SDL_BUTTON_RIGHT:  mouse_right = true; break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch( event.button.button ){
                case SDL_BUTTON_LEFT:   mouse_left  = false; break;
                case SDL_BUTTON_RIGHT:  mouse_right = false; break;
            }
            break;
    };
    AppSDL2OGL::eventHandling( event );
};

// ===================== MAIN

TestAppTileTree2D * testApp;

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	int junk;
	testApp = new TestAppTileTree2D( junk , 800, 600 );
	testApp->loop( 1000000 );
	return 0;
}
















