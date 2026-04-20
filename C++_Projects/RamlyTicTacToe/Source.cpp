//
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <vector>
//#include <iomanip>
//using namespace std;
//int num = 0;
//class Label_Generator {
//protected:
//    string prefix;
//    int index;
//
//public:
//    Label_Generator(const string& prefix, int start_index) : prefix(prefix), index(start_index) {}
//
//    virtual string nextLabel() {
//        return prefix + to_string(index++);
//    }
//};
//
//class File_Label_Generator : public Label_Generator {
//private:
//    vector<string> captions;
//    size_t index = 0;
//
//public:
//    File_Label_Generator(const string& prefix, int startIndex, const string& filename) : Label_Generator(prefix, startIndex) {
//        ifstream file(filename);
//        string line;
//        while (getline(file, line)) {
//            captions.push_back(line);
//        }
//        num = captions.size();
//    }
//
//    string nextLabel() override {
//        string label = Label_Generator::nextLabel();
//        if (index < captions.size()) {
//            label += " - " + captions[index++];
//        }
//        return label;
//    }
//};
//int main() {
//    bool b = true;
//
//    while (b) {
//        cout << "Choose if you want to :" << endl;
//        cout << "1-> view program ." << endl;
//        cout << "2-> Exit ." << endl;
//        cout << "Enter your chocie of 1 or 2 here please -> ";
//
//        int choice;
//        cin >> choice;
//        cout << setfill('-') << setw(30) << "" << endl;
//
//        if (choice == 1) {
//            Label_Generator figure("Figure ", 1);
//            Label_Generator point("P", 0);
//            cout << "Figure numbers: ";
//            for (int i = 0; i < 3; i++) {
//                cout << figure.nextLabel() << ", ";
//            }
//
//            cout << "\nPoint numbers: ";
//            for (int i = 0; i < 5; i++) {
//                cout << point.nextLabel() << ", ";
//            }
//
//            cout << "\nMore figures: ";
//            for (int i = 0; i < 3; i++) {
//                cout << figure.nextLabel() << ", ";
//            }
//
//            cout << "\nFigure labels:\n";
//            File_Label_Generator figureLabels("Figure ", 1, "labels.txt");
//            for (int i = 0; i < num; i++) {
//                cout << figureLabels.nextLabel() << endl;
//            }
//        }
//        else if (choice == 2) {
//            cout << "Okay goodbye then (*_*) ";
//            b = false;
//        }
//        else {
//            cout << "Invalid input please choose only number 1 or 2 (>_<)";
//        }
//
//    }
//
//
//    return 0;
//}




#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
using namespace std;

template <class T>
class StatisticalCalculation {
private:
	int counter;
	int arr_size;
	T* list;
public:
	StatisticalCalculation(const string& filename) : counter(0) {
		ifstream file(filename);
		if (!file) {
			cerr << "Error: Could not open file " << filename << endl;
			exit(1);
		}

		file >> arr_size;
		list = new T[arr_size];

		T value;
		while (file >> value) {
			inputData(value);
		}

		file.close();
	}

	void sort() {
		for (int i = 1; i < counter; i++) {
			T key = list[i];
			int j = i - 1;

			while (j >= 0 && list[j] > key) {
				list[j + 1] = list[j];
				j--;
			}
			list[j + 1] = key;
		}
	}

	void inputData(T data) {
		if (counter < arr_size) {
			list[counter++] = data;
		}
		else {
			cout << "Error -> array is already full. " << endl;
		}
	}

	void displayArray() {
		cout << "[ ";
		for (int i = 0; i < arr_size; i++) {
			cout << list[i] << ", ";
		}
		cout << "]" << endl;
	}

	int size() {
		return arr_size;
	}

	T findMin() {
		sort();
		return list[0];
	}

	T findMax() {
		sort();
		return list[--arr_size];
	}

	T findMedian() {
		sort();
		int m = counter / 2;
		if (counter % 2 == 0) {
			return (list[m - 1] + list[m]) / 2;
		}
		else {
			return list[m];
		}
	}

	T findSummation() {
		T sum = 0;
		for (int i = 0; i < counter; i++) {
			sum += list[i];
		}
		return sum;
	}

	double findMean() {
		if (counter == 0) return 0;
		return static_cast<double>(findSummation()) / counter;
	}


	void statisticsMenu() {
		cout << "Your file elements is ";
		displayArray();
		bool b = true;
		int choice;

		while (b) {
			cout << "Select a statistical calculation:\n";
			cout << "1. Find Median\n2. Find Minimum\n3. Find Maximum\n4. Find Mean\n5. Find Summation\n6. Exit\n ";
			cin >> choice;

			switch (choice) {
			case 1:
				cout << "Median: " << findMedian() << endl;
				break;
			case 2:
				cout << "Minimum: " << findMin() << endl;
				break;
			case 3:
				cout << "Maximum: " << findMax() << endl;
				break;
			case 4:
				cout << "Mean: " << findMean() << endl;
				break;
			case 5:
				cout << "Summation: " << findSummation() << endl;
				break;
			case 6:
				b = false;
				break;
			default:
				cout << "Invalid choice!\n";
				break;
			}
		}

	}

	~StatisticalCalculation() {
		delete[] list;
		cout << "Good Bye (>_<)";
	}
};
int main() {
	string filename;
	cout << "Enter file name: ";
	cin >> filename;

	StatisticalCalculation<int> arr(filename);
	arr.statisticsMenu();

	return 0;
}