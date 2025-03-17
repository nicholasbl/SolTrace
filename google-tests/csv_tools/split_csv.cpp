#include "split_csv.h"

//Make this more specific to raydata
vector<vector<string>> split_csv(string path)
{
	ifstream file(path);
	if (!file.is_open())
	{
		cerr << "Error opening file" << endl;
	}

	string line;
	vector<vector<string>> csv_data_columns;

	while (getline(file, line))
	{
		stringstream ss(line);
		string cell;
		int col_index = 0;

		while (getline(ss, cell, ','))
		{
			if (csv_data_columns.size() <= col_index)
			{
				csv_data_columns.push_back(std::vector<std::string>());
			}
			csv_data_columns[col_index].push_back(cell);
			col_index++;
		}

	}

	file.close();

	return csv_data_columns;
}