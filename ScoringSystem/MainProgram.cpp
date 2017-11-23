#include "Submission.h"
#include <ctime>

int main() {
	Submission test("C:/Users/Kevin/Documents/Visual Studio 2015/Projects/ScoringSystem/ScoringSystem/ScoringSystem/Student.XML", 10000);
	//std::cout << PosixTimeToFolderName(boost::posix_time::microsec_clock::local_time());  current time
	for (int i = 1000000; i < 1030000; i++) {
			boost::filesystem::create_directories("C:/Users/Kevin/Documents/Visual Studio 2015/Projects/ScoringSystem/ScoringSystem/ScoringSystem/Submission/" + std::to_string(i));
			std::cout << "C:/Users/Kevin/Documents/Visual Studio 2015/Projects/ScoringSystem/ScoringSystem/ScoringSystem/Submission/" + std::to_string(i) << std::endl;
			for (int x = 0; x < 100; x++) {
				boost::filesystem::create_directories("C:/Users/Kevin/Documents/Visual Studio 2015/Projects/ScoringSystem/ScoringSystem/ScoringSystem/" + std::to_string(i) + "/" + PosixTimeToFolderName(boost::posix_time::microsec_clock::local_time()));
			}
	}
	system("pause");
}