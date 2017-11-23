#include "Submission.h"

void PropertyTree::CreateDirFromXML() {
	std::string File = Directory;
	//read XML file
	read_xml(File, pt);
	//obtain location for the program
	bool format = false; //format of address ('/' false,'\' true)
	int idx;
	idx = File.find_last_of("/");
	if (idx == -1) {
		idx = File.find_last_of("\\");
		format = true;
	}
	std::string Directory = File.substr(0, idx); //Directory of the data (removing the name of XML file from File)
												 /******************/
												 //create directories from the property tree
	_CreateDir(Directory, pt, format);
}

void PropertyTree::InsertDir(std::string MSSV, std::string DateAndTime) {
	//A.update property tree
	//S1: Convert date and time to proper Windows folder name.
	//index based on date and time format
	DateAndTime[10] = '_';
	DateAndTime[13] = '~';
	DateAndTime[16] = '~';
	DateAndTime[19] = ',';
	//S2: Update to property tree
	pt.get_child("Submission").put(MSSV + "." + DateAndTime, " ");
	//S3: Update to XML file
	write_xml(Directory, pt);
	//B.add directory
	//S1:Create directory
	int idx;
	idx = Directory.find_last_of("/");
	if (idx == -1)
		idx = Directory.find_last_of("\\");
	std::string dir = Directory.substr(0, idx) + "\\Submission\\" + MSSV + "\\" + DateAndTime;
	boost::filesystem::create_directories(dir);
}

void PropertyTree::DeleteDir(std::string MSSV, std::string DateAndTime) {
	//A.update property tree
	//S1: Convert date and time to proper Windows folder name.
	//index based on date and time format
	DateAndTime[10] = '_';
	DateAndTime[13] = '~';
	DateAndTime[16] = '~';
	DateAndTime[19] = ',';
	//S2: delete path in property tree
	if (NoOfChild(pt.get_child("Submission").get_child(MSSV)) > 1)
		pt.get_child("Submission").get_child(MSSV).erase(DateAndTime);
	else if (NoOfChild(pt.get_child("Submission")) > 1) {
		pt.get_child("Submission").erase(MSSV);
		pt.get_child("Submission").add(MSSV, " ");
	}
	else {
		boost::property_tree::ptree newTree;
		newTree.add("Submission", " ");
		pt = newTree;
	}
	//S3: Update to XML file
	write_xml(Directory, pt);
	//B.delete directory
	//S1:Create directory
	int idx;
	idx = Directory.find_last_of("/");
	if (idx == -1)
		idx = Directory.find_last_of("\\");
	std::string dir = Directory.substr(0, idx) + "\\Submission\\" + MSSV + "\\" + DateAndTime;
	boost::filesystem::remove(dir);
}

std::string * PropertyTree::ReadDir()
{
	return nullptr;
}

int NoOfChild(boost::property_tree::ptree &pt) {
	using boost::property_tree::ptree;
	int count = 0;
	for (ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
		++count;
	}
	return count;
}

std::string PosixTimeToFolderName(boost::posix_time::ptime t)
{
	std::string DateAndTime = to_iso_extended_string(t);
	DateAndTime[10] = '_';
	DateAndTime[13] = '~';
	DateAndTime[16] = '~';
	DateAndTime[19] = ',';
	return DateAndTime;
}

void PropertyTree::_CreateDir(std::string Dir, boost::property_tree::ptree const& pt, bool format) {
	using boost::property_tree::ptree;
	for (ptree::const_iterator it = pt.begin(); it != pt.end(); ++it) {
		std::string newDir = (format) ? (Dir + "\\" + it->first) : (Dir + "/" + it->first);
		boost::filesystem::create_directories(newDir);
		_CreateDir(newDir, it->second, format);
	}
}

void PropertyTree::setDir(std::string Address) {
	Directory = Address;
}

std::string PropertyTree::getDir() {
	return Directory;
}

void Submission::reHeapUp() {
	int idx = size - 1;
	while ((idx - 1) / 2) {
		if (Submit[idx].time < Submit[(idx - 1) / 2].time)
			std::swap(Submit[idx], Submit[(idx - 1) / 2]);
		else
			break;
		idx = (idx - 1) / 2;
	}
}

void Submission::reHeapDown() {
	int idx = 0;
	while (2 * idx + 1 < size && 2 * idx + 2 < size) {
		if (Submit[idx].time > Submit[2 * idx + 1].time) {
			std::swap(Submit[idx].time, Submit[2 * idx + 1].time);
			idx = 2 * idx + 1;
		}
		else if (Submit[idx].time > Submit[2 * idx + 2].time) {
			std::swap(Submit[idx].time, Submit[2 * idx + 2].time);
			idx = 2 * idx + 2;
		}
		else
			break;
	}
}

PropertyTree Submission::GetFolderStructure()
{
	return pTree;
}

StudentSubmission * Submission::Scan() {
	using boost::property_tree::ptree;
	//quét folders
	WIN32_FIND_DATA StudentList;
	HANDLE hfind;
	//tạo string dẫn đến folder Submission
	std::string spath = pTree.getDir().substr(0, pTree.getDir().size() - 11);
	spath += "Submission/*";
	int i = 0;
	//Loop đọc các folder mã số sinh viên
	hfind = FindFirstFile(spath.data(), &StudentList);
	do {
		if (StudentList.dwFileAttributes == 16) {
			if (i > 1) {
				boost::property_tree::ptree list = pTree.pt.begin()->second.get_child(StudentList.cFileName); //cây chứa lần nộp CŨ của 1 sinh viên
				std::string submissionTurns = spath.substr(0, spath.size() - 1) + StudentList.cFileName + "/*";
				WIN32_FIND_DATA SubmissionList;
				HANDLE hfind2 = FindFirstFile(submissionTurns.data(), &SubmissionList);
				//đọc các lần nộp của sinh viên
				int g = 0;
				do {
					if (SubmissionList.dwFileAttributes == 16) {
						if (g > 1) {
							ptree::assoc_iterator idt = list.find(SubmissionList.cFileName);
							if ((idt == list.not_found())) { //không có ->update cây và file XMl
								//1.Update file XML và cây property_tree
								pTree.InsertDir(StudentList.cFileName, SubmissionList.cFileName);
								//2.Tạo thêm lần nộp mới vào array của Submission: multithread, do bị ảnh hưởng khi xóa phần tử
								
							}
						}
					}
					++g;
				} while (FindNextFile(hfind2, &SubmissionList));
			}
		}
		++i;
	} while (FindNextFile(hfind, &StudentList));
	FindClose(hfind);
	system("pause");
	return nullptr;
}