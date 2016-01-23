
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "drawMath2D.h"

#include "AppSDL2OGL.h"
#include "testUtils.h"

#include "NBodyWorld2D.h"

class NBodyWorldApp : public AppSDL2OGL {
	public:
    NBodyWorld world;

	virtual void draw   ();
	virtual void drawHUD();
	virtual void mouseHandling( );
	virtual void eventHandling   ( const SDL_Event& event  );

	void pickParticle( Particle2D*& picked );

	NBodyWorldApp( int& id, int WIDTH_, int HEIGHT_ );

};

NBodyWorldApp::NBodyWorldApp( int& id, int WIDTH_, int HEIGHT_ ) : AppSDL2OGL( id, WIDTH_, HEIGHT_ ) {
    world.init();
}

void NBodyWorldApp::draw(){
    glClearColor( 0.9f, 0.9f, 0.9f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    world.update();

/*
    glColor3f( 0.2f, 0.2f, 0.2f );
    for( int i=0; i<world.nParticles; i++ ){
        Particle2D* pi = &(world.particles[i]);
        ULONG icell = world.map.getBucket( pi->pos.x, pi->pos.y );
        double x,y;
        world.map.unfoldBucket( icell, x, y );
        Draw2D::drawRectangle( (float)x, (float)y, (float)(x+world.map.step), (float)(y+world.map.step), false );
    }
*/

    glColor3f( 0.2f, 0.2f, 0.2f );
    for( ULONG icell : world.activeCells ){
        double x,y;
        world.map.unfoldBucket( icell, x, y );
        Draw2D::drawRectangle( (float)x, (float)y, (float)(x+world.map.step), (float)(y+world.map.step), false );
    }

    Particle2D* screenObjects[65536];
    //float camXmin_ =-1; float camXmax_ =+1;
    //float camYmin_ =-1; float camYmax_ =+1;
    float camXmin_ =camXmin; float camXmax_ =camXmax;
    float camYmin_ =camYmin; float camYmax_ =camYmax;
/*
    Draw2D::drawRectangle( camXmin_, camYmin_, camXmax_, camYmax_, false );
	UINT nfound = world.map.getObjectsInRect( camXmin_, camYmin_, camXmax_, camYmax_, &(screenObjects[0]) );
	//glBegin(GL_POINTS);
	for( int i=0; i<nfound; i++ ){
        Particle2D* p = screenObjects[i];
		//glVertex3f( (float) p->pos.x, (float)p->pos.y, 1.0f );
		Draw2D::drawCircle_d( p->pos, 0.25, 8, true );
	}
	//glEnd();
	printf( "nfound %i filled %i \n", nfound, world.map.filled );
*/
    UHALF ix0 = world.map.getIx( camXmin_ );  UHALF iy0 = world.map.getIy( camYmin_ );
    UHALF ix1 = world.map.getIx( camXmax_ );  UHALF iy1 = world.map.getIy( camYmax_ );
    UINT nfound_tot = 0;
    int ncells  = 0;
    for( UHALF iy = iy0; iy<=iy1; iy++ ){
        for( UHALF ix = ix0; ix<=ix1; ix++ ){
            UINT nfoundi = world.map.getBucketObjectsInt( ix, iy, screenObjects );
            nfound_tot += nfoundi;
            for( int i=0; i<nfoundi; i++ ){
                Particle2D* p = screenObjects[i];
                if( p->charge > 0 ){ glColor3f( 0.0f, 0.5f, 1.0f ); }else{ glColor3f( 1.0f, 0.5f, 0.0f ); }
                Draw2D::drawCircle_d( p->pos, 0.5, 8, true );
            }
            /*
            if( nfoundi > 0 ){
                glColor3f( 0.3f, 0.3f, 0.3f );
                double x = world.map.getX(ix);
                double y = world.map.getY(iy);
                Draw2D::drawRectangle( (float)x, (float)y, (float)(x+world.map.step), (float)(y+world.map.step), false );
            }
            */
            //printf( " ix %i iy %i  \n", ix, iy, ni );
        }
    }

    Draw2D::drawPointCross_d( world.anchor, 0.5 );
    if( world.picked != NULL ) Draw2D::drawLine_d( world.anchor, world.picked->pos );

    printf( " ======== frame %i DONE ( map.filled %i nfound_tot %i )\n", frameCount, world.map.filled, nfound_tot );
	//STOP = true;

};

void NBodyWorldApp::mouseHandling( ){
    Uint32 buttons = SDL_GetMouseState( &mouseX, &mouseY );
    defaultMouseHandling( mouseX, mouseY );
    world.anchor.set( mouse_begin_x, mouse_begin_y );
}


void NBodyWorldApp::pickParticle( Particle2D*& picked ){
    Particle2D* screenObjects[256];
    // mouse picking
    double rmax  = 2.0d;
    double r2max = rmax*rmax;
    Vec2d vmouse;
    vmouse.set( mouse_begin_x, mouse_begin_y );
    UINT mfound = world.map.getObjectsInRect( (float)(vmouse.x - rmax ), (float)(vmouse.y - rmax ), (float)(vmouse.x + rmax ), (float)(vmouse.y + rmax ), &(screenObjects[0]) );
    //printf( "mfound  %i \n", mfound );
    int imin = -1;
    double r2min = 1e+300;
    for( int i=0; i<mfound; i++ ){
        Particle2D* p = screenObjects[i];
        Vec2d d;
        d.set_sub( p->pos, vmouse );
        double r2 = d.norm2();
        if( r2 < r2max ){
            if( r2 < r2min ){  r2min = r2; imin = i; }
        }
        //printf( " r2 %3.3f r2max %3.3f r2min %3.3f imin %i \n", r2, r2max, r2min, imin );
    }
    if( imin >= 0 ){
        picked = screenObjects[imin];
    }else{
        picked = NULL;
    }
    //if( world.picked != NULL ) printf( " MOUSE picked (%f,%f) \n", world.picked->pos.x, world.picked->pos.y );
}

void NBodyWorldApp::eventHandling ( const SDL_Event& event  ){
    printf( "NBodyWorldApp::eventHandling() \n" );
    switch( event.type ){
        case SDL_MOUSEBUTTONDOWN:
            switch( event.button.button ){
                case SDL_BUTTON_LEFT:
                    //printf( "left button pressed !!!! " );
                    pickParticle( world.picked );
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch( event.button.button ){
                case SDL_BUTTON_LEFT:
                    //printf( "left button pressed !!!! " );
                    world.picked = NULL;
                    break;
            }
            break;
    };
    AppSDL2OGL::eventHandling( event );
}

void NBodyWorldApp::drawHUD(){

}

// ===================== MAIN

NBodyWorldApp * thisApp;

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	int junk;
	thisApp = new NBodyWorldApp( junk , 800, 600 );
	thisApp->zoom = 30;
	thisApp->loop( 1000000 );
	return 0;
}
















