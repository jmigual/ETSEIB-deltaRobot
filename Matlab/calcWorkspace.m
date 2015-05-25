function M = calcWorkspace(x, y, z, steps)
i = 1;
for dz = 0:z/(2*steps):z
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

tri = delaunay(X, Y, Z);
trisurf(tri,X, Y, Z, 'EdgeColor', 'black', 'LineWidth', 0.05);
axis('equal')


M(:, 1) = X;
M(:, 2) = Y;
M(:, 3) = Z;
end