#ifndef  RigidMolecule_h
#define  RigidMolecule_h

#include "fastmath.h"
#include "Vec3.h"
#include "Mat3.h"
#include "quaternion.h"

#define R2SAFE  1.0e-8
#define F2MAX   10.0

//  ============== Globals

constexpr float R_MAX = 1.8;
constexpr float R2MAX = R_MAX*R_MAX;

constexpr int nAtoms = 4;
constexpr int nMols  = 2;
//constexpr int nMols  = 128;
float pos   [nMols*8];
float vel   [nMols*8];
float force [nMols*8];
float force_[nMols*8];

Vec3f atomsT [nAtoms*nMols];
float atomsT_[nAtoms*nMols*4];

Vec3f forceAtomT[nAtoms*nMols];

//Vec3f molAtoms   [nAtoms*3] = { 0.0f,0.0f,0.0f,   1.0f,0.0f,0.0f,   0.0f,1.0f,0.0f,   0.0f,0.0f,1.0f  };
//Vec3f molAtoms     [nAtoms] = { 1.0f,1.0f,1.0f,   -1.0f,-1.0f,1.0f,   -1.0f,1.0f,-1.0f,   1.0f,-1.0f,-1.0f  };
Vec3f molAtoms     [nAtoms] = { 0.5f,0.5f,0.5f,   -0.5f,-0.5f,0.5f,   -0.5f,0.5f,-0.5f,   0.5f,-0.5f,-0.5f  };
//Vec3f molAtoms     [nAtoms] = { 1.0f,0.0f,0.0f,   -1.0f,0.0f,0.0f  };
//Vec3f molAtoms     [nAtoms] = { 2.0f,0.0f,0.0f };

float molAtoms_[nAtoms*4];

void initParticles( int n, float * pos, double step, double drnd ){
    float root_n = pow(n,0.33333);
    int nz    = (int)(floor(root_n));
    int ny    = (int)(floor(sqrt((n/nz)+1)));
    int nx    = ((int)(n/(nz*ny)))+1;
    int nrest = n - nz*ny*nx;
    printf( "n %i %g   (%i,%i,%i) \n", n, root_n, nx, ny, nz );
    int i = 0;
    for(int iz=0; iz<nz; iz++){
        for(int iy=0; iy<ny; iy++){
            for(int ix=0; ix<nx; ix++){
                if(i<n){
                    int i8 = i<<3;
                    ((Vec3f*)(pos+i8))-> set(
                        (ix-0.5*nx)*step + randf(-drnd,drnd),
                        (iy-0.5*ny)*step + randf(-drnd,drnd),
                        (iz-0.5*nz)*step + randf(-drnd,drnd)
                    );
                    ((Quat4f*)(pos+i8+4))->fromUniformS3( {randf(), randf(), randf()} );
                    //((Quat4f*)(pos+i8+4))->setOne();
                }
                i++;
            }
        }
    }
}

inline void addAtomicForceSR( const Vec3f& dp, Vec3f& f ){
    //Vec3f dp; dp.set_sub( p2, p1 );
    float r2 = dp.norm2();
    //printf( "%g (%g,%g,%g)\n", r2, dp.x, dp.y, dp.z );
    if (r2 > R2MAX) return;
    //Draw2D::drawLine(p1,p2);
    float ir2 = 1/( r2 + R2SAFE );
    //float fr  = (0.2-ir2)*(R2MAX-r2);
    float fr  = (0.7-ir2)*(R2MAX-r2);
    f.add_mul( dp, fr );
}

inline void addAtomicForceLJ( const Vec3f& dp, Vec3f& f, float C6, float C12 ){
    //Vec3f dp; dp.set_sub( p2, p1 );
    float ir2 = 1/( dp.norm2() + R2SAFE );
    float ir6 = ir2*ir2*ir2;
    float fr  = (6*C6*ir6 - 12*C12*ir6*ir6)*ir2;
    f.add_mul( dp, fr );
}

inline void addAtomicForceR24( const Vec3f& dp, Vec3f& f, float C2, float C4 ){
    //Vec3f dp; dp.set_sub( p2, p1 );
    float ir2 = 1/( dp.norm2() + R2SAFE );
    float fr  = C2*ir2 - C4*ir2*ir2;
    f.add_mul( dp, fr );
}

inline void addAtomicForceCoulomb( const Vec3f& dp, Vec3f& f, float kQQ ){
    //Vec3f dp; dp.set_sub( p2, p1 );
    float ir2 = 1/( dp.norm2() + R2SAFE );
    float ir  = sqrt(ir2);
    f.add_mul( dp, ir2*ir*kQQ );
}

inline void addAtomicForceSpring( const Vec3f& dp, Vec3f& f, float k ){
    f.add_mul( dp, k );
}

void transformAtoms( const Vec3f& pos, const Quat4f& rot, int npoints, Vec3f * points, Vec3f * Tpoints ){
    // https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/
    Mat3f T;
    rot.toMatrix( T);
    for( int i=0; i<npoints; i++ ){
        Vec3f Tp;
        T.dot_to_T(   points[i],  Tp ); // this is correct as far as I know
        //T.dot_to(   points[i],  Tp );
        Tpoints[i].set_add( pos, Tp  );
    }
}

void transformAtoms_noMat( const Vec3f& pos, const Quat4f& rot, int npoints, Vec3f * points, Vec3f * Tpoints ){
    for( int i=0; i<npoints; i++ ){
        Vec3f Tp;
        //printf( "%i (%g,%g,%g) (%g,%g,%g)\n", i, rot.x, rot.y, rot.z, rot.w,   points[i].x, points[i].y, points[i].z  );
        rot.transformVec(points[i],Tp);
        //printf( "%i (%g,%g,%g,%g) (%g,%g,%g)\n", i, rot.x, rot.y, rot.z, rot.w,   points[i].x, points[i].y, points[i].z );
        //printf( "%i (%g,%g,%g,%g) (%g,%g,%g) (%g,%g,%g)\n", i, rot.x, rot.y, rot.z, rot.w,   points[i].x, points[i].y, points[i].z, Tp.x,Tp.y,Tp.z  );
        Tpoints[i].set_add( pos, Tp  );
    }
}

void transformAllAtoms( int nMols, int nAtoms, float * pos, Vec3f * molAtoms, Vec3f * atomsT ){
    int iatom0 = 0;
    for(int i=0; i<nMols; i++){
        int i8 = i<<3;
        //transformAtoms( *(Vec3f*)(pos+i8), *(Quat4f*)(pos+i8+4), nAtoms, molAtoms, atomsT+iatom0 );
        transformAtoms_noMat( *(Vec3f*)(pos+i8), *(Quat4f*)(pos+i8+4), nAtoms, molAtoms, atomsT+iatom0 );
        iatom0  += nAtoms;
    }
}

void RBodyForce( int nMols, int nAtoms, float * pos, float * force, Vec3f * atomsT, Vec3f * molAtoms ){
    int i=0;
    for(int imol=0; imol<nMols; imol++){
        for(int iatom=0; iatom<nAtoms; iatom++){
            Vec3f f; f.set(0.0f);
            //f.set(0.0f,0.1f,0.0f);
            Vec3f p = atomsT[i];
            int j = 0;
            for(int jmol=0; jmol<nMols; jmol++){
                if(jmol==imol){ j+=nAtoms; continue; }
                for(int jatom=0; jatom<nAtoms; jatom++){
                    //addAtomicForceSR( p, atomsT[j], f );
                    //addAtomicForceLJ( atomsT[j]-p, f );
                    addAtomicForceR24( atomsT[j]-p, f, 1.0f, 3.0f );
                    //addAtomicForceCoulomb( atomsT[j]-p, f );
                    //addAtomicForceSpring( atomsT[j]-p, f );
                    //printf( "(%i,%i) (%g,%g,%g)\n",i,j,  f.x, f.y, f.z );
                    //if((iatom==0)&&(imol==0)){
                    //    //printf( "(%i,%i) (%g,%g,%g) (%g,%g,%g) (%g,%g,%g) \n", jmol,j, p.x,p.y,p.z,  atomsT[j].x,atomsT[j].y,atomsT[j].z,  f.x, f.y, f.z );
                    //    printf( "(%i,%i) (%g,%g,%g) (%g,%g,%g) \n", jmol,j, atomsT[j].x,atomsT[j].y,atomsT[j].z,  f.x, f.y, f.z );
                    //}
                    j++;
                }
            }
            //printf( "%i %i %i (%g,%g,%g)\n", imol, iatom, i, f.x, f.y, f.z );
            forceAtomT[i] = f;
            ((Quat4f*)(pos+4))->addForceFromPoint( molAtoms[iatom], f, *((Quat4f*)(force+4)) );
            ((Vec3f *)(force))->add( f );
            i++;
        }
        force+=8;
        pos  +=8;
    }
    //exit(0);
}

void setArray( int n, float * arr, float f ){ for(int i=0;i<n;i++){ arr[i]=f; }}

float move_leap_frog( int nMols, float * pos, float * vel, float * force, float dt, float damp ){
    for(int i=0;i<nMols;i++){
        int i8=i<<3;
        Quat4f& rot = *((Quat4f*)(pos+i8+4));
        ((Quat4f*)(vel  +i8+4))->outproject(rot);
        ((Quat4f*)(force+i8+4))->outproject(rot);

        //((Vec3f*)(vel  +i8))->set(0.0f);
        //((Vec3f*)(force+i8))->set(0.0f);
        //printf("%i (%g,%g,%g)  (%g,%g,%g,%g)\n", i, force[i8+0], force[i8+1], force[i8+2],   force[i8+4], force[i8+5], force[i8+6], force[i8+7]);
    }
    //exit(0);
    int n = nMols<<3;
    float f2norm = 0.0f;
    for(int i=0; i<n; i++){
        float v = vel[i];
        float f = force[i];
        f2norm += f*f;
        v       = v*damp + f*dt;
        vel[i]  = v;
        pos[i] += v*dt;
    }
    for(int i=0;i<nMols;i++){
        ((Quat4f*)(pos+(i<<3)+4))->normalize();
    }
    return f2norm;
}

/*
class RBody{
    Vec3f  pos,vpos,fpos;
    Quat4f rot,vrot,frot;

    void transPoints(int n, Vec3f * points, Vec3f * Tpoints ){
        Mat3f T;
        rot.toMatrix(T);
        for( int i=0; i<n; i++ ){
            Vec3f Tp;
            //T.dot_to_T(   points[i],  Tp );
            T.dot_to(   points[i],   Tp );
            Tpoints[i].set_add( pos, Tp  );
        }
    }

    void forceFromPoints(int n, Vec3f * points, Vec3f * force ){
        for(int i=0; i<n; i++){
            rot.addForceFromPoint( points[i], forces[i], frot );
            fpos.add(forces[i]);
        }
    }

    void move_leap_frog(float dt, float damp){
        vrot.outproject(rot);
        frot.outproject(rot);

        vrot.mul(damp);
        vrot.add_mul(frot,dt);
         rot.add_mul(vrot,dt);
        vpos.mul(damp);
        vpos.add_mul(fpos,dt);
         pos.add_mul(vpos,dt);

        rot.normalize();
    }
};
*/

#endif

