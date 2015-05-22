function angle = singleAngle(x0, y0, z0, a, b)

n = b*b - a*a - z0*z0 - x0*x0 - y0*y0;
root = sqrt (n*n*y0*y0 - 4*(x0*x0 + y0*y0)*(-x0*x0*a*a + n*n/4));

if (x0 < 0)
    root = root * -1;
end

y = (-n*y0 + root ) / (2*(x0*x0 + y0*y0));
sign = 1;

if ((b*b - (y0 + a)*(y0 + a)) < (x0*x0 + z0*z0) && x0 < 0)
    sign = -1;
end

x = sqrt(a*a - y*y)*sign;

angle = atan2(y, x);

end