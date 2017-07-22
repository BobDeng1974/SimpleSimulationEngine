
#ifndef  GL3Utils_h
#define  GL3Utils_h

#include <GL/glew.h>
//#define GL_GLEXT_PROTOTYPES
//#include <GL/gl.h>

#include "Vec2.h"
#include "Vec3.h"
#include "GLObject.h"
#include "GLobjects.h"
#include "GLUtils.h"
#include "CMesh.h"

//GLObject * qaudPatchHard( int n, Vec2d p0, Vec2d da, Vec2d db, Vec3d (vertFunc)(Vec2d) ){


GLObject * makeOgl_flat( const CMesh& mesh ){
    GLObject * ogl = new GLObject();
    ogl->setup( countVerts( mesh.nfaces, mesh.ngons ) );
    hardFace( mesh.nfaces, mesh.ngons, mesh.faces, mesh.verts, ogl->buffs[0].cbuff, ogl->buffs[1].cbuff );
    ogl->init();
    return ogl;
}

template<typename Func>
GLObject * qaudPatchHard( int n, Vec2d p0, Vec2d da, Vec2d db, Func vertFunc ){
    //GLObject * ogl = new GLObject( 3*2*n*n );
    GLObject * ogl = new GLObject();
    ogl->setup( 3*2*n*n );
    Vec3f * verti = (Vec3f*)ogl->buffs[0].cbuff;
    Vec3f * normi = (Vec3f*)ogl->buffs[1].cbuff;
    double d = 1.0d/(n);
    Vec3d p00,p01,p10,p11,nv;
    for(int i=0; i<n; i++){
        Vec2d uv  = p0 + da*i;
        p00 = vertFunc( uv );
        p01 = vertFunc( uv+da );
        for(int j=0; j<n; j++){
            uv.add(db);
            p10 = vertFunc( uv    );
            p11 = vertFunc( uv+da );
            nv.set_cross(p10-p00,p01-p00); nv.normalize();
            convert(p00,verti[0]); convert(p01,verti[1]); convert(p10,verti[2]);
            convert(nv ,normi[0]); convert(nv ,normi[1]); convert(nv ,normi[2]);
            nv.set_cross(p01-p11,p10-p11); nv.normalize();
            convert(p11,verti[3]); convert(p01,verti[4]); convert(p10,verti[5]);
            convert(nv ,normi[3]); convert(nv ,normi[4]); convert(nv ,normi[5]);
            p00=p10; p01=p11;
            verti+=6;
            normi+=6;
        }
    }
    ogl->init();
    return ogl;
}

template<typename Func>
GLMesh * qaudPatchSmooth( Vec2i n, Vec2f p0, Vec2f da, Vec2f db, Func vertFunc, void**cbuffs=NULL ){
    int nVerts = n.a*n.b;
    int nTris  =(n.a-1)*(n.b-1)*2;
    Vec3f * vpos = new Vec3f[nVerts];
    Vec3f * vnor = new Vec3f[nVerts];
    Vec3i * tris = new Vec3i[nTris ];
    int iv = 0;
    int ii = 0;
    //printf( "(%i,%i) (%i,%i) (%g,%g)(%g,%g)(%g,%g)\n", n.a,n.b, nVerts, nInds , p0.x,p0.y, da.x,da.y, db.x,db.y );
    for(int ia=0; ia<n.a; ia++){
        for(int ib=0; ib<n.b; ib++){
            Vec2f p = p0 + da*ia + db*ib;
            vertFunc( p, vpos[iv], vnor[iv] );
            //printf( "%i %i %i (%g,%g) (%g,%g,%g) \n", ia, ib, iv, p.x, p.y, vpos[iv].x, vpos[iv].y, vpos[iv].z );
            if(ia<(n.a-1)&&(ib<(n.b-1))){ tris[ii].set(iv,iv+1,iv+n.b); ii++; tris[ii].set(iv+n.b,iv+1,iv+n.b+1); ii++; }
            iv++;
        }
    }
    GLMesh * glmesh = new GLMesh();
    glmesh->init( nVerts, nTris*3, tris, vpos, vnor, NULL, NULL );
    if(cbuffs){ cbuffs[0]=tris; cbuffs[1]=vpos; cbuffs[2]=vnor; }
    else{ delete [] vpos; delete [] vnor; delete [] tris; }
    return glmesh;
}



template<typename Func>
GLMesh * qaudPatchSmooth( Vec2i n, Vec2f span, int pattern, Func vertFunc, void**cbuffs=NULL ){
    int nVerts=0, nTris=0;
    switch(pattern){
        case 0: nTris=(n.a-1)*(n.b-1)*2; nVerts=n.a*n.b;       break;
        case 1: nTris=(n.a-1)*(n.b-1)*4; nVerts=n.a*(2*n.b-1); break;
    }
    Vec3f * vpos = new Vec3f[nVerts];
    Vec3f * vnor = new Vec3f[nVerts];
    Vec3i * tris = new Vec3i[nTris ];
    int iq = 0; int iv = 0; int ii = 0;
    float da = span.a/(n.a-1);
    float db = span.b/(n.b-1);
    //printf( "(%i,%i) (%i,%i) (%g,%g)(%g,%g)(%g,%g)\n", n.a,n.b, nVerts, nInds , p0.x,p0.y, da.x,da.y, db.x,db.y );
    for(int ia=0; ia<n.a; ia++){
        for(int ib=0; ib<n.b; ib++){
            Vec2f p = { da*ia, db*ib };
            vertFunc( p, vpos[iv], vnor[iv] ); iv++;
            if(ia<(n.a-1)&&(ib<(n.b-1))){
                switch(pattern){
                    case 0: tris[ii].set(iv-1,iv,iv+n.b-1); ii++; tris[ii].set(iv+n.b-1,iv,iv+n.b); ii++; break;
                    case 1:
                        p.add(0.5f*da,0.5f*db);
                        vertFunc( p, vpos[iv], vnor[iv] );
                        int iv2 = (n.b<<1)-1 + iv;
                        //if(ia<(n.a-2)){ iv2+=iv; }else{ iv2+=(iv>>1); }
                        if(ia==(n.a-2)){ iv2-=ib; if(ib==(n.b-2)) iv2--; };
                        tris[ii].set(iv,iv -1,iv +1); ii++;
                        tris[ii].set(iv,iv -1,iv2-1); ii++;
                        tris[ii].set(iv,iv2+1,iv2-1); ii++;
                        tris[ii].set(iv,iv2+1,iv +1); ii++;
                        iv++;
                        break;
                }
            }
            //printf( "%i %i %i (%g,%g) (%g,%g,%g) \n", ia, ib, iv, p.x, p.y, vpos[iv].x, vpos[iv].y, vpos[iv].z );
            iq++;
        }
    }
    GLMesh * glmesh = new GLMesh();
    glmesh->init( nVerts, nTris*3, tris, vpos, vnor, NULL, NULL );
    if(cbuffs){ cbuffs[0]=tris; cbuffs[1]=vpos; cbuffs[2]=vnor; }
    else{ delete [] vpos; delete [] vnor; delete [] tris; }
    return glmesh;
}

template<typename Func>
GLMesh * qaudPatchUV( Vec2i n, void**cbuffs=NULL ){
    int nVerts = n.a*n.b;
    int nTris  =(n.a-1)*(n.b-1)*2;
    Vec2f * vUVs = new Vec3f[nVerts];
    Vec3i * tris = new Vec3i[nTris ];
    int iv = 0;
    int ii = 0;
    float da = 1.0f/(n.a-1);
    float db = 1.0f/(n.b-1);
    //printf( "(%i,%i) (%i,%i) (%g,%g)(%g,%g)(%g,%g)\n", n.a,n.b, nVerts, nInds , p0.x,p0.y, da.x,da.y, db.x,db.y );
    for(int ia=0; ia<n.a; ia++){
        for(int ib=0; ib<n.b; ib++){
            vUVs[iv] = { da*ia, db*ib };
            //printf( "%i %i %i (%g,%g) (%g,%g,%g) \n", ia, ib, iv, p.x, p.y, vpos[iv].x, vpos[iv].y, vpos[iv].z );
            if(ia<(n.a-1)&&(ib<(n.b-1))){ tris[ii].set(iv,iv+1,iv+n.b); ii++; tris[ii].set(iv+n.b,iv+1,iv+n.b+1); ii++; }
            iv++;
        }
    }
    GLMesh * glmesh = new GLMesh();
    glmesh->init( nVerts, nTris*3, tris, NULL, NULL, NULL, vUVs );
    if(cbuffs){ cbuffs[0]=tris; cbuffs[1]=vUVs; }
    else{ delete [] tris; delete [] vUVs; }
    return glmesh;
}

template<typename Func>
GLObject * makeNVerts( int n, Func vertFunc ){
    //GLObject * ogl = new GLObject( 3*2*n*n );
    GLObject * ogl = new GLObject();
    ogl->setup( n );
    Vec3f * verti = (Vec3f*)ogl->buffs[0].cbuff;
    Vec3f * normi = (Vec3f*)ogl->buffs[1].cbuff;
    for(int i=0; i<n; i++){
        Vec3d p,nv;
        vertFunc( i, p, nv );
        convert(p,verti[i]); convert(nv,normi[i]);
    }
    ogl->init();
    return ogl;
}


template<typename Func>
GLObject * makeNTris( int n, Func vertFunc ){
    //GLObject * ogl = new GLObject( 3*2*n*n );
    GLObject * ogl = new GLObject();
    ogl->setup( n*3 );
    Vec3f * verti = (Vec3f*)ogl->buffs[0].cbuff;
    Vec3f * normi = (Vec3f*)ogl->buffs[1].cbuff;
    int ii = 0;
    for(int i=0; i<n; i++){
        Vec3d p,nv;
        vertFunc( i, 0, p, nv ); convert(p,verti[ii]); convert(nv,normi[ii]); ii++;
        vertFunc( i, 1, p, nv ); convert(p,verti[ii]); convert(nv,normi[ii]); ii++;
        vertFunc( i, 2, p, nv ); convert(p,verti[ii]); convert(nv,normi[ii]); ii++;
    }
    ogl->init();
    return ogl;
}


#endif
