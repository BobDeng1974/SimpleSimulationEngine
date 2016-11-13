
#ifndef AeroCraftWorld_h
#define AeroCraftWorld_h

#include <vector>

#include "Vec2.h"
#include "Vec3.h"

#include "FieldPatch.h"
#include "AeroCraft.h"

// Aerocraft handling
static constexpr double wingLiftDefault = 1.0;
static constexpr double wingLiftDown    = 0.0;
static constexpr double wingLiftUp      = 2.0;


class AeroCraftWorld {
	public:

	double ground_level;
	Vec3d  wind_speed;
	Vec2d  watter_speed;

	int perFrame = 10;
	double dt = 0.001;

	AeroCraft * myCraft;
	AeroCraft * myCraft_bak;

	FieldPatch fieldPatch;
	int buildings_shape = -1;
	int terrain_shape   = -1;


	int fontTex_DEBUG;

/*
	std::vector<Frigate2D*>  ships;
	std::vector<Projectile*> projectiles;  // see http://stackoverflow.com/questions/11457571/how-to-set-initial-size-of-stl-vector
*/

    void resetSteer( );
	void steerToDir( const Vec3d& dir, bool on );
	void update( );
	void init( );


	// filling game with objects
	void makeAeroCraft();
	int  makeBuildingsGrid    ( int nx, int ny, float sx, float sy, float cx, float cy,  float min_height, float max_height );
	int  makeBuildingsClusters( int nclustest, int nmin, int nmax, float minx, float maxx, float miny, float maxy, float min_dist, float max_dist, float min_size, float max_size, float min_height, float max_height );
	void makeEnvironment      ( float size );


//	void projectile_collisions();

};

#endif  // #ifndef AeroCraftWorld_h
