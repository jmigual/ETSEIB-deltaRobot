% X, Y i Z son arrays que contenen els punts vàlids
delaunay(X, Y, Z);
tri = delaunay(X, Y, Z);
trisurf(tri,X, Y, Z)
hold on
trisurf(tri,X, Y, Z)
axis('equal')