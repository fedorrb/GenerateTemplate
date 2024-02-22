#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <set>
#include <vector>
#include <regex>
#include <sstream>
#include <unordered_set>

using namespace std;

std::string GetStrRegex(int index);
std::vector<std::vector<std::string>> GetMatches(std::string str_regex, std::vector<string>& input_file_lines);
std::vector<string> ReadLinesFromFile(std::string input_file_str);
void WriteOutTxt(std::vector<std::vector<std::string>>& matches);
std::string GetStrParam(int index);
std::string ReplaceMatch(std::string input_str, const std::vector<std::string>& replacements);
std::vector<std::string> GenerateByTemplate(std::vector<std::vector<std::string>>& matches, std::string str_param);
void WriteOutputTemplates(std::vector<std::string>& output);
void MakeUniq(vector<string>& unique_values);

int main(int argc, char** argv)
{
	// input file name
	std::string input_file = "d:\\in.txt";

	// read content of input file
	std::vector<string> input_file_lines = ReadLinesFromFile(input_file);

	// get string with regex pattern
	std::string str_regex = GetStrRegex(0);

	// get vector of mathes
	std::vector<std::vector<std::string>> matches = GetMatches(str_regex, input_file_lines);

	// write matches to out.txt 
	WriteOutTxt(matches);

	// sorting
	if (0) {
		std::sort(matches.begin(), matches.end(), [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
			return a[1] < b[1]; // compare second elements every vector
			});
	}

	// get string with params
	std::string str_param = GetStrParam(2);

	// convert to output
	std::vector<std::string> output = GenerateByTemplate(matches, str_param);

	// write output_templates.txt
	WriteOutputTemplates(output);

    return 0;
}

std::string GetStrRegex(int index) {
	std::vector<string> list_regex_string;
	// 0
	// in: "123._1HP8M_UIN1_L1_USTNGDEN.AND" LN=19 "UINT" RW 0 Houz=1 nb=15 kb=1
	// out: 123 _1HP8M_UIN1_L1_USTNGDEN AND UINT
	list_regex_string.push_back("\"(\\d+)\\.(.*?)\\.(\\w+)\".*?\"(\\w+)\".*");

	// check index
	if (list_regex_string.size() > index)
		return list_regex_string[index];
	else
		return "";
}

std::string GetStrParam(int index) {
	std::vector<string> list_param_string;
	// 0
	list_param_string.push_back("#ТЕХ ПРЕФИКС=K_{:match_1}, ПАРАМ={:match_2}, АТР={:match_3}, ТИП={:match_4}");
	// 1
	list_param_string.push_back("{:match_2}.{:match_3}");
	// 2
	std::string str2 = "#CALL ALS_KONTR_MPRC\n"
		"{MPRC={:match_2},       /* имя модуля МПРЦ-1\n"
		" SINHR16=_01_MPC.SINHR_M16, /* синроимпульс 1,6s\n"
		" SINHR19=_01_MPC.SINHR_M19, /* синроимпульс 1,9s\n"
		" ALS_SOST=_01_MPC.ALS_SOST {:match_3}  /* обобщенный параметр работоспособности\n"
		"}";
	list_param_string.push_back(str2);
	// 3 перед викликом додавати до вектору з параметрами номер, перевірити
	std::string str3 = " #CALL MPC_REG_NASTR {GRP=_01_MVR_GRP, OKU=_01_MVR_OKU, KEY=_01_MVR_KEY},{OKU_REGIM=_01_PS{:match_1}_REGIM, REGIM=_01_MRC_REGIM},{NASTR_GRP=1, NASTR_OKU={:match_2}, NASTR_KEY=1}\n"
		" #CALL MPC_REG_NASTR {GRP=_01_MVR_GRP, OKU=_01_MVR_OKU, KEY=_01_MVR_KEY},{OKU_REGIM=_01_PS{:match_1}_REGIM, REGIM=_01_MRC_REGIM},{NASTR_GRP=1, NASTR_OKU=682, NASTR_KEY=1}";
	list_param_string.push_back(str3);


	// check index
	if (list_param_string.size() > index)
		return list_param_string[index];
	else
		return "";
}

std::vector<std::vector<std::string>> GetMatches(std::string str_regex, std::vector<string>& input_file_lines) {
	std::vector<std::vector<std::string> > vv;
	std::smatch match;
	std::regex pattern(str_regex);
	std::vector<string> oneLine;

	for (std::string input : input_file_lines) {
		if (std::regex_search(input, match, pattern)) {
			int size = match.size();
			oneLine.clear();
			for (int i = 0; i < size; i++) {
				if(i) // match[0] не записувати, дублюється вхідна строка
					oneLine.push_back(match[i]);
			}
			if (!oneLine.empty())
				vv.push_back(oneLine);
		}
	}
	return vv;
}

std::vector<string> ReadLinesFromFile(std::string input_file_str) {
	std::vector<string> lines;
	wifstream input_file(input_file_str);
	if (!input_file.is_open()) {
		cerr << "Error: Unable to open input file." << endl;
		return lines;
	}
	//input_file.imbue(locale(locale(), new codecvt_byname<wchar_t, char, mbstate_t>("ru_RU.CP866")));
	wstring line;
	while (getline(input_file, line)) {
		string utf8_line(line.begin(), line.end());
		lines.push_back(utf8_line);
	}
	input_file.close();
	return lines;
}

void WriteOutTxt(std::vector<std::vector<std::string>>& matches) {
	ofstream output_file("d:\\out.txt");
	if (!output_file) {
		cerr << "Error: Unable to open output file." << endl;
	}
	for (const std::vector<string>& value : matches) {
		int size = value.size();
		for (int i = 0; i < size; ++i) {
			output_file << value[i] << endl;
		}
	}
	output_file.close();
}

void WriteOutputTemplates(std::vector<std::string>& output) {
	ofstream output_file("d:\\output_templates.txt");
	if (!output_file) {
		cerr << "Error: Unable to open output file." << endl;
	}
	for (const std::string& str : output) {
		output_file << str << endl;
	}
	output_file.close();
}

std::string ReplaceMatch(std::string input_str, const std::vector<std::string>& replacements) {
	std::stringstream result;
	std::regex placeholderRegex(R"(\{:(match_\d+)\})");
	std::sregex_iterator next(input_str.begin(), input_str.end(), placeholderRegex);
	std::sregex_iterator end;
	size_t last_pos = 0;
	while (next != end) {
		std::smatch match = *next;
		std::string match_index_str = match[1].str();
		size_t match_index = std::stoi(match_index_str.substr(6)) - 1; // Вираховуємо індекс вектора replacements
		result << input_str.substr(last_pos, match.position() - last_pos); // Додаємо частину строки до заміни
		if (match_index < replacements.size()) {
			result << replacements[match_index]; // Додаємо відповідне значення з вектора replacements
		}
		else {
			result << match.str(); // Якщо індекс виходить за межі вектора, просто додаємо посилання назад
		}
		last_pos = match.position() + match.length(); // Оновлюємо позицію останньої знайденої заміни
		++next;
	}
	// Додаємо залишок строки після останньої заміни
	result << input_str.substr(last_pos);
	return result.str();
}

std::vector<std::string> GenerateByTemplate(std::vector<std::vector<std::string>>& matches, std::string str_param) {
	std::vector<std::string> output;
	for (std::vector<std::string> match : matches) {
		output.push_back(ReplaceMatch(str_param, match));
	}
	return output;
}

/// <summary>
/// function for make vector "unique_values" unique
/// and elements no change position 
/// </summary>
/// <param name="unique_values"></param>
void MakeUniq(vector<string>& unique_values)
{
    vector<string> unique_vec;
    unique_vec.clear();
    set<string> unique_set;
    unique_set.clear();
    std::pair<std::set<string>::iterator, bool> ret;
    for (const string& value : unique_values) {
        ret = unique_set.insert(value);
        if(ret.second == true)
            unique_vec.push_back(value);
    }
    unique_values.clear();
    for (const string& value : unique_vec) {
        unique_values.push_back(value);
    }
}