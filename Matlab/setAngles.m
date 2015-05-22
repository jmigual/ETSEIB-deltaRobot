function D = setAngles(x, y, z)
    

% Some definitions
a = 17.233;     % Arm length
b = 22.648;     % Forearm length
L1 = 6.374;     % Base center length
L2 = 6;         % Clamp support center length

sin60 = sqrt(3)/2;
cos60 = 1/2;
    
x1 = x + L2 - L1;
y1 = z;
z1 = y;
D(1) = singleAngle(x1,y1,z1, a, b);

x2 = y*sin60 - x*cos60 + L2 - L1;
y2 = z;
z2 = -y*cos60 - x*sin60;
D(2) = singleAngle(x2,y2,z2, a, b);

x3 = -y*sin60 - x*cos60 + L2 - L1;
y3 = z;
z3 = -y*cos60 + x*sin60;
D(3) = singleAngle(x3,y3,z3, a, b);

end