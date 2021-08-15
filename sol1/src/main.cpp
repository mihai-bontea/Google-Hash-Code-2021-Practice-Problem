#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <Windows.h>
#include <algorithm>
#include "Data.h"

using namespace std;

vector<OutputForm> simulate(Data &data)
{	
	vector<OutputForm> deliveries;

	int teams_of_4_left = data.teams_of_4;
	int teams_of_3_left = data.teams_of_3;
	int teams_of_2_left = data.teams_of_2;

	int count = 0;
	while (data.pizzas.size() >= 2)
	{
		//cout << count << " " << data.pizzas.size() << endl;
		count++;

		// Starting a delivery with the first element in the list
		Delivery delivery(data.pizzas.begin(), data.total_unique_ingr);
		data.pizzas.pop_front();

		if (teams_of_4_left && data.pizzas.size() >= 3)
		{
			for (int i = 0; i < 3; ++i)
			{
				delivery.add_pizza(data.pizzas.begin(), data.total_unique_ingr);
				data.pizzas.pop_front();
			}
			--teams_of_4_left;
		}

		else if (teams_of_3_left && data.pizzas.size() >= 2)
		{
			for (int i = 0; i < 2; ++i)
			{
				delivery.add_pizza(data.pizzas.begin(), data.total_unique_ingr);
				data.pizzas.pop_front();
			}
			--teams_of_3_left;
		}

		else if (teams_of_2_left)
		{
			delivery.add_pizza(data.pizzas.begin(), data.total_unique_ingr);
			data.pizzas.pop_front();

			--teams_of_2_left;
		}

		else
			break;

		deliveries.push_back(delivery);
	}

	return deliveries;
}

int main()
{
	const string in_prefix = "../../input_files/";
	const string out_prefix = "../../output_files/sol1/";
	vector<string> input_files = { "a_example.in", "b_little_bit_of_everything.in", "c_many_ingredients.in", "d_many_pizzas.in", "e_many_teams.in" };

	for (auto in_file_it = input_files.begin(); in_file_it != input_files.end(); ++in_file_it)
	{
		cout << "Now working on " << (*in_file_it);
		Data data(in_prefix + (*in_file_it));
		cout << ". Input processed.\n";

		vector<OutputForm> result = simulate(data);

		string out_filename = out_prefix + (*in_file_it).substr(0, ((*in_file_it).find('.'))) + ".out";
		data.write_to_file(out_filename, result);
	}

	system("pause");
	return 0;
}