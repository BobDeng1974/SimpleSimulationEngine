﻿--- https://www.lua.org/pil/5.3.html

print("BEGIN spaceship");

-- http://www.mse.mtu.edu/~drjohn/my4150/props.html
Material{ name="Kevlar", density=1.44e+3, Spull=3.6e+9, Spush=0.0, Kpull=154.0e+9, Kpush=0.0, reflectivity=0.6,  Tmelt=350 }
Material{ name="Steel" , density=7.89e+3, Spull=1.2e+9, Spush=0.0, Kpull=200.0e+9, Kpush=0.0, reflectivity=0.85, Tmelt=800 }
--Material{ name="Titanium" , density=7.89e+3, Spull=3.6e+9, Spush=0.0, Kpull=154.0e+9, Kpush=0.0, reflectivity=0.7, Tmelt=450 }

--- newSpaceShip( "Ship1" )

origin = {0.0,0.0,0.0}
xvec   = {1.0,0.0,0.0}
yvec   = {0.0,1.0,0.0}
zvec   = {0.0,0.0,1.0}

n0 = Node( origin );
n1 = Node( {-100.0,   0.0,    0.0} )
n2 = Node( { 100.0,   0.0,    0.0} )
n3 = Node( {   0.0,-200.0,    0.0} )
n4 = Node( {   0.0, 200.0,    0.0} )
n5 = Node( {   0.0,   0.0, -300.0} )
n6 = Node( {   0.0,   0.0,  800.0} )

print( "Lua:Nodes:" , n0,n1,n2,n3,n4,n5,n6 )


defGirderWidth = 4.0


--         from to  Up nseg,mseg   width, hegith
g1 = Girder( n0, n1, zvec, 10, 2, {defGirderWidth,defGirderWidth}, "Steel" )
g2 = Girder( n0, n2, zvec, 10, 2, {defGirderWidth,defGirderWidth}, "Steel" )
g3 = Girder( n0, n3, xvec, 20, 2, {defGirderWidth,defGirderWidth}, "Steel" )
g4 = Girder( n0, n4, xvec, 20, 2, {defGirderWidth,defGirderWidth}, "Steel" )
g5 = Girder( n0, n5, xvec, 30, 2, {8.0,8.0}, "Steel" )
g6 = Girder( n0, n6, xvec, 80, 2, {8.0,8.0}, "Steel" )

g7 = Girder( n3, n6, xvec, 70, 2, {defGirderWidth,defGirderWidth}, "Steel" )
g8 = Girder( n4, n6, xvec, 70, 2, {defGirderWidth,defGirderWidth}, "Steel" )

--          type     thick[mm]
Rope(n5,n1, 25, "Kevlar");
Rope(n6,n1, 25, "Kevlar" )
Rope(n5,n2, 25, "Kevlar");
Rope(n6,n2, 25, "Kevlar" )
Rope(n5,n3, 25, "Kevlar");
--Rope(n6,n3, 25, "Kevlar" )
Rope(n5,n4, 25, "Kevlar");
--Rope(n6,n4, 25, "Kevlar" )

Rope(n1,n3, 25, "Kevlar");
Rope(n1,n4, 25, "Kevlar");
Rope(n2,n3, 25, "Kevlar");
Rope(n2,n4, 25, "Kevlar");

function tanks( n,aOff, R, r, L, z0 )
    for i=1,n do
        local a = (i/n + aOff)*2*math.pi 
        Tank( {math.cos(a)*R,math.sin(a)*R,z0}, zvec, {r,r,L}, "H2")
    end
end

function girderFan( n, aOff, w, h, z0,z1,   nseg, thick, shielded )
    local tipNode = Node({0.0,0.0,z1});
    local a = aOff*2*math.pi 
    local gd0  = Girder( tipNode, Node({math.cos(a)*w,math.sin(a)*h,z0}), xvec, nseg, 2, {thick,thick}, "steel")
    local ogd  = gd0;
    for i=1,n do
        local gd=gd0
        if i<n then
            local a = (i/n + aOff)*2*math.pi
            gd = Girder( tipNode, Node({math.cos(a)*w,math.sin(a)*h,z0}), xvec, nseg, 2, {thick,thick},  "steel" )
        end
        if shielded then Shield( ogd,0.0,1.0, gd,0.0,1.0 ) end
        ogd = gd;
    end
end

tanks( 6,0.0, 10.0, 5.0, -50.0, -20.0 )
girderFan( 4, 0.0, 20.0, 40.0,-50.0, -250, 10, 1.0, true )
girderFan( 4, 0.0, 20.0, 40.0, 20.0,  300, 10, 1.0, true )

girderFan( 8, 0.0, 80.0, 80.0, -340, -150, 10, 1.0, false )

-- =RadiatorType = {"LithiumHeatPipe", 1280.0 }

--Radiator( g5,0.2,0.8, g1,0.2,0.8, 1280.0 )
Radiator( g6,0.15,0.8, g7,0.02,0.8, 1280.0 )
Radiator( g6,0.15,0.8, g8,0.02,0.8, 1280.0 )

--      node1,2, up,  nseg    R     {width,height}
Ring( {0.0,0.0,4.0}, zvec, xvec, 16, 100.0, {4.0,4.0}, "Steel" )
Ring( {20.0,0.0,0.0}, xvec, yvec, 16, 108.0, {4.0,4.0}, "Steel" )
-- Ring( {0.0,0.0,0.0}, yvec, xvec, 16, 160.0, {8.0,5.0}, "Steel" )

--  There should be mechanism how to generate nodes on-top of ship components (anchor points)

Thruster( {0.0,0.0,-300.0}, zvec, {5.0,100.0,50.0}, "ICF_Ebeam_magNozzle" )
-- Thruster( {-16,-16,16}, {1.0,2.0,3.0}, {5.0,100.0,50.0}, "ICF_Ebeam_magNozzle" )


Gun( g6, 0.1, 0.8, "XFEL" )


print("END spaceship");

--[=====[
--                type          T[K]
M_radiator = {"LithiumHeatPipe", 1280.0 }

Radiator( g5, g1 )
Radiator( g5, g2 )
Radiator( g5, g3 )
Radiator( g5, g4 )
Radiator( g6, g1 )
Radiator( g6, g2 )
Radiator( g6, g3 )
Radiator( g6, g4 )

Ring  ( n0, n6, xvec, 80, 2, {10.0,8.0} )

--                   path    side offset
Gun ( "railgun", {n5,n0,n6}, { 1.0,0.0} )
Gun ( "XFEL",    {n5,n0,n6}, {-1.0,0.0} )

-- Tank  ( "", R=16 )
-- Tank  ( "water", R=16 )

Thruster{ n5, maxPower=7.0e+9, maxTrust=10.0e+3, maxVexh=50e+3, nodes={} }

--]=====]
