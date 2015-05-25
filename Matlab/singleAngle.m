% Rep com a paràmetres:
% - x, y, z de la posicio a calcular en la base adequada
% - a la llargada del brac
% - b la llargada de l'avantbrac
function angle = singleAngle(x0, y0, z0, a, b)

n = b^2 - a^2 - z0^2 - x0^2 - y0^2;
root = sqrt (n^2*y0^2 - 4*(x0^2 + y0^2)*(-x0^2*a^2 + n^2/4));

if (x0 < 0)
    root = root * -1;
end

y = (-n*y0 + root ) / (2*(x0^2 + y0^2));

sign = 1;
if ((b^2 - (y0 + a)*(y0 + a)) < (x0^2 + z0^2) && x0 < 0)
    sign = -1;
end

x = sqrt(a^2 - y^2)*sign;

if (isreal(x) && isreal(y))
    angle = atan2(y, x);
else
    angle = NaN;
end

end