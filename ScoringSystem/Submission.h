#pragma once
#include <iostream>
#include <string>
//Used for creating date and time object
#include "boost\date_time\posix_time\posix_time.hpp"
//The two library below is used for reading and parsing XML file
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
//Used for create, delete...directories
#include <boost/filesystem.hpp>
#include <Windows.h>
#include <vector>
//used for calculating time and multithreading
#include <chrono>
#include <mutex>
#include <thread>

#define DEFAULT_MAX_SUBMISSION 10000

//through PropertyTree:
//Insert and create new directories
//delete if necessary
//Modify and update XML file (depend on Students' submit)
class PropertyTree {
	friend class Submission;
	//Main tree
	boost::property_tree::ptree pt;
	//Directory of XML file
	std::string Directory;
	friend int NoOfChild(boost::property_tree::ptree&);
	void _CreateDir(std::string, boost::property_tree::ptree const&, bool);
public:
	void setDir(std::string); //Xác định vị trí file XML chính của folder ScoringSystem
	std::string getDir(); //Trả về vị trí file XML chính của folder ScoringSystem
	void CreateDirFromXML(); //Tạo folder Submission và Testcase từ file XML
	void InsertDir(std::string, std::string);//Insert directory and update XML file, bao gồm MSSV và date time dưới dạng string
	void DeleteDir(std::string, std::string);//delete directory and update XML file, bao gồm MSSV và date time dưới dạng string
	/**/std::string* ReadDir(); //Read new directory from current "Submission" folder, insert into XML file , create directories, and return list of new submission
};

//struct 1 lần nộp bài của 1 sinh viên bất kỳ
struct StudentSubmission {
	std::string Location;// current address of file group in computer (date time format)
	boost::posix_time::ptime time; //get from Location
	float score; //students' score -> xuất ra folder output format tên: "tentest.mssv.score.txt"
	bool isChecked; //check whether student's file has been processed
	std::string MSSV; //Student's ID
};

class Submission {
	//an array of students, each contain a list of file group student submitted
	StudentSubmission *Submit;
	//current property tree
	PropertyTree pTree;
	//current size of array above
	int size;
	//maximum size of heap
	int MaxSize;
	//private methods
	void reHeapUp();
	void reHeapDown();
public:
	//constructor
	Submission(std::string Address) {
		pTree.setDir(Address);
		//Check existence of XML file
		if (!exists(boost::filesystem::path(pTree.getDir())))
			throw("XML File does not exist!");
		pTree.CreateDirFromXML();
		Submit = new StudentSubmission[DEFAULT_MAX_SUBMISSION];
		MaxSize = DEFAULT_MAX_SUBMISSION;
		size = 0;
	}
	Submission(std::string Address, int MAX_ELEMENT) {
		pTree.setDir(Address);
		//Check existence of XML file
		if (!exists(boost::filesystem::path(pTree.getDir())))
			throw("XML File does not exist!");
		pTree.CreateDirFromXML();
		Submit = new StudentSubmission[MAX_ELEMENT];
		MaxSize = MAX_ELEMENT;
		size = 0;
	}
	//public methods
	PropertyTree GetFolderStructure();
	void InsertData(StudentSubmission*); //Insert (Heap-style) new submission information from the directory given by PropertyTree
	StudentSubmission* Scan(); //scan new submissions from student and update property tree
};

std::string PosixTimeToFolderName(boost::posix_time::ptime);

