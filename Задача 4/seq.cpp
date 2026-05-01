#include <iostream>
#include <fstream>
using namespace std;

int main() {
    int n;
    cout << "Введите n: ";
    cin >> n;
    
    ofstream file("numbers.txt");
    if (!file.is_open()) {
        cout << "Ошибка открытия файла!" << endl;
        return 1;
    }
    
    for (int i = 1; i <= n; i++) {
        file << i << endl;
    }
    
    file.close();
    return 0;
}
