#include<iostream>
#include"vec3f.h"

using namespace std;

int main(int argc, char *argv []) {
	cout << (abs(-0) < 10e-6 ? "True" : "False") << endl;
	Vec3f a, b(1,0,0);

	cout << "Non-const:" << endl;
	cout << a << " [0,0,0]" << endl;
	cout << b << " [1,0,0]" << endl;
	
	const Vec3f c, d(1,0,0);

	cout << "Const:" << endl;
	cout << c << " [0,0,0]" << endl;
	cout << d << " [1,0,0]" << endl;

	cout << "Addition testing" << endl;
	cout << "   Homogeneous access:" << endl;
	cout << a+b << " [1,0,0]" << endl;
	cout << c+d << " [1,0,0]" << endl;
	cout << "   Const + Non-const:" << endl;
	cout << a+d << " [1,0,0]" << endl;
	cout << b+c << " [1,0,0]" << endl;
	cout << "   Addition with Assignment:" << endl;
	a += b;
	cout << a << " [1,0,0]" << endl;
	a += c;
	cout << a << " [1,0,0]" << endl;

	cout << endl << "Cross-product" << endl;
	cout << a << " " << b << " " << a*b << " [0,0,0]" << endl;
	cout << a << " " << c << " " << a*c << " [0,0,0]" << endl;
	a.y = 1; a.x = 0;
	cout << d << " " << a << " " << d*a << " [0,0,1]" << endl;
	a.z = 1; a.y = 0;
	cout << a << " " << d << " " << a*d << " [0,1,0]" << endl;
	b.x = 0; b.y = 1;
	cout << b << " " << a << " " << b*a << " [1,0,0]" << endl;

	cout << endl << "Subtraction testing" << endl;
	cout << a << " " << d << " " << a-d << " [-1,0,1]" << endl;
	cout << c << " " << a << " " << c-a << " [0,0,-1]" << endl;
	cout << a << " " << a << " " << a-a << " [0,0,0]" << endl;

	cout << endl << "Scalar multiplication" << endl;
	cout << 5 << " * " << a << " = " << 5*a << " [0,0,5]" << endl;
	cout << a << " * " << 5 << " = " << a*5 << " [0,0,5]" << endl;
	cout << 2e-10 << " * " << a << " = " << (2e-10)*a << " [0,0,0]" << endl;

	cout << endl << "Scalar Division" << endl;
	cout << a << "/" << 2 << " = " << a/2 << " [0,0,0.5]" << endl;

	cout << endl << "Negation" << endl;
	cout << "-" << a << " = " << -a << " [0,0,-1]" << endl;

	cout << endl << "Magnitude" << endl;
	cout << "length" << a << " = " << a.length() << " [1]" << endl;

	cout << endl << "Dot Product" << endl;
	cout << a << " . " << a.dot(a) << " [1]" << endl;
	cout << a << " . " << a.dot(b) << " [0]" << endl;

	cout << endl << "Element access" << endl;
	cout << a << " = ( " << a[0] << " , " << a[1] << " , " << a[2] << " )" << endl;
	cout << c << " = ( " << c[0] << " , " << c[1] << " , " << c[2] << " )" << endl;
	a[0] = 1;
	cout << a << " = ( " << a[0] << " , " << a[1] << " , " << a[2] << " )" << endl;

	cout << endl << "Miscellaneous" << endl;
	cout << Vec3f(1-1,-1+1,-0) << " [0,0,0]" << endl;
	cout << "a       = " << a << endl;
	cout << "b       = " << b << endl;
	cout << "d       = " << d << endl;
	cout << "a*b     = " << a*b << endl;
	cout << "a*b*d   = " << a*b*d << endl;
	cout << "a*b*d*5 = " << a*b*d*5 << endl;
	cout << "b/2     = " << b/2 << endl;
	cout << "a*b*d*5 - b/2 = " << a*b*d*5-b/2 << endl;
	cout << endl;
	cout << "2*d = " << 2*d << endl;
	cout << "a - 2*d = " << a-2*d << endl;

	cout << endl << "Equality" << endl;
	cout << "a-2*d == a*b: " << ((a-2*d) == (a*b*d)) << " [1]" << endl;
	cout << "a == b        " << (a == b) << " [0]" << endl;

	cout << endl << "Copying and Assignment" << endl;
	a = b;
	cout << "a = b        => " << a << endl;
	a = Vec3f(d);
	cout << "a = Vec3f(d) => " << a << endl;

	cout << endl << "Normalization" << endl;
	a += (d+b);
	cout << "a                = " << a << endl;
	cout << "a.norm()         = " << a.norm() << endl;
	a.normalizeMe();
	cout << "a.normalizeMe() => " << a << endl;

	return 0;
}
