% Rep com a parametre la posicio x, y, z on es vol saber els tres angles
% del robot que es el que retorna la funcio.
function D = setAngles(x, y, z)

% Mides del robot
%a = 17.233;     % Llargada brac
a = 12;
b = 22.648;     % Llargada avantbrac
L1 = 6.374;     % Distancia al centre del triangle de la base
L2 = 6;         % Distancia al centre del triangle de la pinca

% Definicions de constants
sin60 = sqrt(3)/2;
cos60 = 1/2;
    
% Calcul primer angle
x1 = x + L2 - L1;
y1 = -z;
z1 = y;
D(1) = singleAngle(x1,y1,z1, a, b);

% Calcul segon angle
x2 = y*sin60 - x*cos60 + L2 - L1;
y2 = -z;
z2 = -y*cos60 - x*sin60;
D(2) = singleAngle(x2,y2,z2, a, b);

% Calcul tercer angle
x3 = -y*sin60 - x*cos60 + L2 - L1;
y3 = -z;
z3 = -y*cos60 + x*sin60;
D(3) = singleAngle(x3,y3,z3, a, b);

for  i = 1:3
    D(i) = 150 - D(i)*180/pi;
end

end