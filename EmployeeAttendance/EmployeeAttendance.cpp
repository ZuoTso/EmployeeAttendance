#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <map>
#include <sstream>

using namespace std;

struct AttendanceRecord {
	string id;
	string sign;
	string timestamp;
};

struct AttendanceStats {
	int consecutiveDays = 0;
	string startDate;
	string endDate;
};

vector<AttendanceRecord> parseAttendanceRecords(string filename) {
	vector<AttendanceRecord> records;

	//open file
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Error opening file " << filename << endl;
		exit(1);
	}

	//read file and store in records
	string line;
	while (getline(file, line)) {
		stringstream ss(line);
		string id, sign, timestamp;
		getline(ss, id, ',');
		getline(ss, sign, ',');
		getline(ss, timestamp, ',');
		if (timestamp < "20120101000000" || timestamp > "20230426000000") {
			continue;
		}
		records.push_back({ id, sign, timestamp });
	}
	file.close();
	return records;
}

bool compareByConsecutiveDays(const pair<string, AttendanceStats>& a, const pair<string, AttendanceStats>& b) {
	return a.second.consecutiveDays > b.second.consecutiveDays;
}

void printAttendanceStats(const vector<pair<string, AttendanceStats>>& attendanceStats) {
	for (auto& stats : attendanceStats) {
		cout << stats.first << "," << stats.second.consecutiveDays << "," << stats.second.startDate << "," << stats.second.endDate << endl;
		//cout << ID			<< "," << Consecutive Days			   << "," << Start Date				<< "," << End Date			   << endl;
	}
}

/*
int main() {
	std::string date1_str = "20220101"; // 第一個日期，格式為YYYYMMDD
	std::string date2_str = "20220503"; // 第二個日期，格式為YYYYMMDD

	// 將日期字符串轉換為tm結構體
	std::tm date1 = {};
	date1.tm_year = std::stoi(date1_str.substr(0, 4)) - 1900;
	date1.tm_mon = std::stoi(date1_str.substr(4, 2)) - 1;
	date1.tm_mday = std::stoi(date1_str.substr(6, 2));

	std::tm date2 = {};
	date2.tm_year = std::stoi(date2_str.substr(0, 4)) - 1900;
	date2.tm_mon = std::stoi(date2_str.substr(4, 2)) - 1;
	date2.tm_mday = std::stoi(date2_str.substr(6, 2));

	// 將tm結構體轉換為time_t類型
	std::time_t time1 = std::mktime(&date1);
	std::time_t time2 = std::mktime(&date2);

	// 計算差值（單位：秒）
	std::time_t diff_seconds = std::difftime(time2, time1);

	// 將差值轉換為天數
	int diff_days = diff_seconds / (60 * 60 * 24);

	std::cout << "Days between " << date1_str << " and " << date2_str << ": " << diff_days << std::endl;

	return 0;
}
*/

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <input_file>" << endl;
		return 1;
	}
	string filename = argv[1];
	vector<AttendanceRecord> records = parseAttendanceRecords(filename);

	//read data from records
	map<string, AttendanceStats> stats;
	for (size_t i = 0; i < records.size(); i++) {
		string id = records[i].id;
		string sign = records[i].sign;
		string timestamp = records[i].timestamp;
		if (i == 0 || id != records[i - 1].id) {
			stats[id] = {};
		}
		if (sign == "sign-in") {
			if (i == 0 || timestamp > records[i - 1].timestamp) {
				stats[id].consecutiveDays++;
				stats[id].startDate = timestamp.substr(0, 8);
			}
		}
		else if (sign == "sign-out") {
			if (i == records.size() - 1 || id != records[i + 1].id || records[i + 1].sign == "sign-in" || timestamp > records[i + 1].timestamp) {
				stats[id].endDate = timestamp.substr(0, 8);
			}
		}
	}

	vector<pair<string, AttendanceStats>> attendanceStats(stats.begin(), stats.end());
	sort(attendanceStats.begin(), attendanceStats.end(), compareByConsecutiveDays);

	printAttendanceStats(attendanceStats);

	return 0;
}
