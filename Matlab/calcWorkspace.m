function [X, Y, Z] = calcWorkspace(x, y, z, steps)
i = 1;
for dz = 0:z/steps:z
   for dx = -x:x/steps:x
      for dy = -y:y/steps:y
         D = setAngles(dx, dy, dz);
         if (~isnan(D))
            X(i) = dx;
            Y(i) = dy;
            Z(i) = dz;
            i = i + 1;
         end
      end
   end
end

delaunay(X, Y, Z);
tri = delaunay(X, Y, Z);
trisurf(tri,X, Y, Z)
hold on
trisurf(tri,X, Y, Z)
axis('equal')

end