
#ifndef  Truss_h
#define  Truss_h

#include <vector>
//#include <unordered_map>
#include <cstdio>
#include <cstring>
//#include <string>

class GirderParams{  public:
    int n=5;
    int m=2;
    int kind=0;
    double widthX = 0.5;
    double widthY = 0.5;
    int kind_long   = 0;
    int kind_perp   = 1;
    int kind_zigIn  = 2;
    int kind_zigOut = 3;
};

class TrussEdge{ public:
    int a,b;
    int type;

    void fromString(char* str){ sscanf ( str, "%i %i %i",&a,&b,&type); a--; b--; }
    void toString  (char* str){ sprintf( str, "%i %i %i", a+1, b+1, type); }
    void print     (         ){  printf(      "%i %i %i", a+1, b+1, type); }
};

class Truss{ public:
    int    rendered_shape;

    std::vector<Vec3d>        points;
    std::vector<TrussEdge>     edges;

    std::vector<Vec2i>        blocks;

    std::vector<int>          removed_points;
    std::vector<int>          removed_edges;

    void sticksFormString( char * str ){
        printf( str );
        char * pch = strtok (str,";\n");
        while (pch != NULL){
            printf ("%s\n",pch);
            TrussEdge edge;
            edge.fromString( pch );
            edges.push_back(edge);
            //char str_tmp[32];
            //edges[edges.size()-1].print();
            //edges[edges.size()-1].toString(str_tmp);
            //printf ("%s\n", str_tmp );
            pch = strtok (NULL, ";\n");
        }
    }

    int loadXYZ( char* fname ){
        FILE * pFile = fopen(fname,"r");
        if( pFile == NULL ){
            printf("cannot find %s\n", fname );
            return -1;
        }
        char buff[1024];
        char * line;
        int nl;
        line = fgets( buff, 1024, pFile );
        int nvert, nedges;
        sscanf( line, "%i %i\n",  &nvert, &nedges );
        printf(       "%i %i \n",  nvert,  nedges );

        line = fgets( buff, 1024, pFile );
        sticksFormString(line); // load stricks // like comment in .xyz file

        for(int i=0; i<nvert; i++){
            char at_name[8];
            line = fgets( buff, 1024, pFile );  //printf("%s",line);
            Vec3d p;
            sscanf( line, "%s %lf %lf %lf \n", at_name, &p.x, &p.y, &p.z );
            printf(       "%s %lf %lf %lf \n", at_name,  p.x,  p.y,  p.z );
            points.push_back(p);
        }
        printf( "loadXYZ DONE\n");
    }

    int pickVertex( const Vec3d &ray0, const Vec3d &hRay ){
        double r2min=1e+300;
        int imin=0;
        for(int i=0; i<points.size(); i++){
            double t;
            double r2 = rayPointDistance2( ray0, hRay, points[i], t );
            if(r2<r2min){ imin=i; r2min=r2; }
        }
        return imin;
    };

    void affineTransform( Mat3d mat, bool T ){
        if( T ) { for(int i=0; i<points.size(); i++){ points[i] = mat.dotT(points[i]); }; }
        else    { for(int i=0; i<points.size(); i++){ points[i] = mat.dot (points[i]); }; }
    }

void girder1( Vec3d p0, Vec3d p1, Vec3d up, int n, double width ){
    int kind_long   = 0;
    int kind_perp   = 1;
    int kind_zigIn  = 2;
    int kind_zigOut = 3;
    Vec3d dir = p1-p0;
    double length = dir.normalize();
    up.makeOrthoU(dir);
    Vec3d side; side.set_cross(dir,up);
	double dl = length/(2*n + 2);
	//int dnb = 2+4+4+4;
	int dnp = 4;
	int i00 = points.size();

	blocks.push_back( {i00,edges.size()} );

	for (int i=0; i<n; i++){
		int i01=i00+1; int i10=i00+2; int i11=i00+3;
		points.push_back( p0 + side*-width + dir*(dl*(1+2*i  )) );
		points.push_back( p0 + side*+width + dir*(dl*(1+2*i  )) );
		points.push_back( p0 + up  *-width + dir*(dl*(1+2*i+1)) );
		points.push_back( p0 + up  *+width + dir*(dl*(1+2*i+1)) );
		edges .push_back( (TrussEdge){i00,i01,kind_perp}  );
		edges .push_back( (TrussEdge){i10,i11,kind_perp}  );
		edges .push_back( (TrussEdge){i00,i10,kind_zigIn} );
		edges .push_back( (TrussEdge){i00,i11,kind_zigIn} );
		edges .push_back( (TrussEdge){i01,i10,kind_zigIn} );
		edges .push_back( (TrussEdge){i01,i11,kind_zigIn} );
		if( i<(n-1) ){
            edges.push_back( (TrussEdge){i10,i00+dnp,kind_zigOut} );
            edges.push_back( (TrussEdge){i10,i01+dnp,kind_zigOut} );
            edges.push_back( (TrussEdge){i11,i00+dnp,kind_zigOut} );
            edges.push_back( (TrussEdge){i11,i01+dnp,kind_zigOut} );
            edges.push_back( (TrussEdge){i00,i00+dnp,kind_long} );
            edges.push_back( (TrussEdge){i01,i01+dnp,kind_long} );
            edges.push_back( (TrussEdge){i10,i10+dnp,kind_long} );
            edges.push_back( (TrussEdge){i11,i11+dnp,kind_long} );
		}
		i00+=dnp;
	}
}

void makeGriders( int nEdges, TrussEdge* edges, Vec3d* points, GirderParams* params, Vec3d * ups ){
    for( int i=0; i<nEdges; i++ ){
        TrussEdge& ed = edges[i];
        girder1( points[ed.a], points[ed.b], ups[i], params[i].n, params[i].widthX );
    }
};

void makeGriders( Truss plan, GirderParams* params, Vec3d * ups ){
    int np0 = points.size();
    blocks.push_back( {np0,edges.size()} );
    for( int i=0; i<plan.points.size(); i++ ){
        points.push_back( plan.points[i] );
    }
    int kind_end = 0;
    for( int i=0; i<plan.edges.size(); i++ ){
        TrussEdge& ed = plan.edges[i];
        int np0i = points.size();
        girder1( plan.points[ed.a], plan.points[ed.b], ups[i], params[i].n, params[i].widthX );
        int np0j = points.size();
        edges.push_back( (TrussEdge){np0+ed.a,np0i+0,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.a,np0i+1,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.a,np0i+2,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.a,np0i+3,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.b,np0j-1,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.b,np0j-2,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.b,np0j-3,kind_end} );
        edges.push_back( (TrussEdge){np0+ed.b,np0j-4,kind_end} );
    }
};

};

#endif



