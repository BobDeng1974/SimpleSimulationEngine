#ifndef Projectile3D_h
#define Projectile3D_h

#include "fastmath.h"
#include "Vec3.h"
#include "Body.h"

class Projectile3D : public PointBody {
	public:
	int    id,kind;
    Vec3d  old_pos;
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

#endif
