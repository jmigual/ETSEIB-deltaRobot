% Es van provant tots els punts a l'espai definit per [-x, x], [-y, y] i
% [0, z] i si no retorna NaN (Not a Number) es considera com a valid i es
% guarda als vectors, tot seguit es genera una superficie en 3D junt amb el
% grafic.
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