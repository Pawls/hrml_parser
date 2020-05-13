#include <iostream>
#include "HrmlTree.h"
using namespace std;

int main() {
	HrmlTree scope;

	string str, temp;
	int n, q;
	cin >> n >> q;
	str += to_string(n) + " " + to_string(q);
	for (int i = 0; i <= (n+q); i++) {
		getline(cin, temp);
		str += temp.append("\n");
	}
	scope.parseInput(str);

	system("Pause");
	return 0;
}