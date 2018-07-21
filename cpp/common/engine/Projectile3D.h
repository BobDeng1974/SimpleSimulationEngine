#ifndef Projectile3D_h
#define Projectile3D_h

#include <vector>

#include "fastmath.h"
#include "Vec3.h"
#include "Body.h"
#include "geom3D.h"

#include "ShooterCommon.h"

class Projectile3D : public PointBody { public:
	int    id,kind;
    Vec3d  old_pos; // DEPRECATED: use pos+vel*dt does the job
    double time=0;

    inline void update_Projectile3D( double dt ){
        time   += dt;
        old_pos.set(pos);
        move_PointBody(dt);
    }

    virtual void update( double dt ){
        update_Projectile3D( dt );
    };

    // TO DO : would be usefull if material of target specified
    //virtual void hit(){}

};

class Burst3d { public:
    int    id=-1;
    double time=0;
    bool   discard=false;    // NOT NECESSARY  ... we may simply shift it to     1e+300 or to NaN

    ProjectileType* type = 0;

    Capsula3D bbox;

	std::vector<Particle3d> shots;

	void updateBBox( Vec3d& p0, Vec3d& p1, double dt ){
        Vec3d hdir; hdir.set_sub(p1,p0);
        double lmin = 0.0;
        double lmax = hdir.normalize();
        float r2max=0.0;
        int n = shots.size();
        //printf( "p0 (%g,%g,%g) p1 (%g,%g,%g) \n", p0.x, p0.y, p0.z, p1.x, p1.y, p1.z );
        for( int i=0; i<n; i++ ){
            Vec3d d;
            double r2,l;
            //Vec3d p = shots[i].pos;
            const Particle3d& p = shots[i];
            d.set_sub( p.pos-p.vel*-dt, p0); l=d.makeOrthoU(hdir); r2=d.norm2(); if(r2>r2max) r2max=r2; if(l>lmax)lmax=l; if(l<lmin)lmin=l; // printf(" fw %i %g %g \n", i, l, lmin);  //d.add_mul( hdir, -hdir.dot(d) );
            p.getOldPos(dt,d);    d.sub(p0); l=d.makeOrthoU(hdir); r2=d.norm2(); if(r2>r2max) r2max=r2; if(l>lmax)lmax=l; if(l<lmin)lmin=l; // printf(" bk %i %g %g \n", i, l, lmin);
        }
        bbox.r = sqrt(r2max);
        bbox.l = lmax-lmin;
        //printf( "lmin %g lmax %g \n", lmin, lmax );
        bbox.p.set_add_mul( p0, hdir, lmin );
        bbox.hdir=hdir;
	}

    void move(double dt, const Vec3d& accel0, double airDensity ){
        //printf("Burst3d::move \n");
        if(discard) return;
        int i=0;
        int n = shots.size();
        //Vec3d tmpPos[n];
        double balisticCoef = airDensity * type->balisticCoef;
        // TODO : we can calculate p.vel.norm() and supersonic drag by taylor expansion
        // double rv0 = shots[0].vel.norm();
        for( int i=0; i<n; i++ ){
            //printf( "shot[%i]\n", i );
            Particle3d& p = shots[i];
            //tmpPos[i] = p.pos;
            Vec3d accel; accel.set_add_mul( accel0, p.vel, p.vel.norm()*balisticCoef );
            p.move(dt, accel );
            //p.move(dt, accel0 );
            time+=dt;
        }
        Vec3d op0; shots[n-1].getOldPos(dt,op0);
        updateBBox( op0, shots[0].pos, dt ); // we asume shot[0] is most forward, shot[n-1] is least
        //printf("Burst3d::move DONE\n");
    }

    void addShot( Vec3d& pos, Vec3d& vel ){
        shots.push_back( (Particle3d){pos,vel} );
    }

    inline void hit( int i){ };

    Burst3d(){};
    Burst3d( ProjectileType* type_, int id_ ){ type=type_; id=id_; }

};

#endif
