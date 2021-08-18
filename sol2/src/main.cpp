#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <Windows.h>
#include <algorithm>
#include "Data.h"
#define NR_ITERATED_BACK 10

using namespace std;

void backtrack(Delivery &delivery, int current_step, int steps, Delivery &best_delivery, Data &data, int true_range, int last_index)
{
	auto it = data.pizzas.end();
	for (int i = 0; i < true_range; ++i)
	{
		--it;
		if (i <= last_index)
			continue;

		//new_delivery.add_pizza(it, data.ingr_rarity, data.total_ingr_count, data.total_unique_ingr);
		delivery.replace_index(it, current_step - 1, data.ingr_rarity, data.total_ingr_count, data.total_unique_ingr);
		if (current_step < steps)
			backtrack(delivery, current_step + 1, steps, best_delivery, data, true_range, i);
		else
		{
			if (delivery.score >= best_delivery.score)
				best_delivery = delivery;
		}
	}
}

Delivery get_best_delivery(Delivery delivery, int delivery_size, Data &data, int range)
{
	int true_range = min((data.pizzas.size() - 1), range);
	Delivery result = delivery;

	if (delivery_size == 2)
	{
		auto it = data.pizzas.end();
		--it;
		result.add_pizza(it, data.ingr_rarity, data.total_ingr_count, data.total_unique_ingr);

		
		for (int i = 1; i < true_range; ++i)
		{
			--it;
			if (result.replace_profit(it, 1, data.ingr_rarity, data.total_ingr_count, data.total_unique_ingr) > 0)
				result.replace_index(it, 1, data.ingr_rarity, data.total_ingr_count, data.total_unique_ingr);
		}
	}
	else
		backtrack(delivery, 2, delivery_size, result, data, true_range, -1);
	return result;
}

vector<OutputForm> simulate(Data &data)
{
	vector<OutputForm> deliveries;

	int teams_of_4_left = data.teams_of_4;
	int teams_of_3_left = data.teams_of_3;
	int teams_of_2_left = data.teams_of_2;

	while (data.pizzas.size() >= 2 && (teams_of_2_left || teams_of_3_left || teams_of_4_left))
	{
		// Starting a delivery with the first element in the list
		Delivery delivery(data.pizzas.begin(), data.total_unique_ingr);
		
		Delivery best_of_2, best_of_3, best_of_4;

		if (teams_of_2_left)
			best_of_2 = get_best_delivery(delivery, 2, data, NR_ITERATED_BACK);

		if (teams_of_3_left && data.pizzas.size() >= 3)
			best_of_3 = get_best_delivery(delivery, 3, data, NR_ITERATED_BACK);

		if (teams_of_4_left && data.pizzas.size() >= 4)
			best_of_4 = get_best_delivery(delivery, 4, data, NR_ITERATED_BACK);

		assert(best_of_2.nr_pizzas == 0 || best_of_2.nr_pizzas == 2);
		assert(best_of_3.nr_pizzas == 0 || best_of_3.nr_pizzas == 3);
		assert(best_of_4.nr_pizzas == 0 || best_of_4.nr_pizzas == 4);

		/*
		cout << best_of_2.score << " " << best_of_3.score << " " << best_of_4.score << endl;
		if (best_of_2.score == best_of_3.score && best_of_2.score != 0)
		{
			for (int i = 0; i < data.total_unique_ingr; ++i)
				cout << (int)best_of_2.ingr_is_present[i] << " ";
			cout << endl;

			for (int i = 0; i < data.total_unique_ingr; ++i)
				cout << (int)best_of_3.ingr_is_present[i] << " ";
			cout << endl;

			for (int i = 0; i < data.total_unique_ingr; ++i)
				cout << (int)best_of_4.ingr_is_present[i] << " ";
			cout << endl;
		}
		*/
		

		double best_score = max(max(best_of_2.score, best_of_3.score), best_of_4.score);
		Delivery final_delivery;
		if (best_of_2.score == best_score && teams_of_2_left)
		{
			--teams_of_2_left;
			final_delivery = best_of_2;
		}
		else if (best_of_3.score == best_score && teams_of_3_left)
		{
			--teams_of_3_left;
			final_delivery = best_of_3;
		}
		else if (best_of_4.score == best_score && teams_of_4_left)
		{
			--teams_of_4_left;
			final_delivery = best_of_4;
		}
		else
		{
			cout << "ERROR" << endl;
		}

		// Removing the selected pizzas from the list
		for (int i = 0; i < final_delivery.nr_pizzas; ++i)
			data.pizzas.erase(final_delivery.pos_in_list[i]);		
	
		// Adding best delivery to list
		if (final_delivery.score != 0)
			deliveries.push_back(final_delivery);

		//final_delivery.release_memory();
	}

	cout << "We have " << data.pizzas.size() << " leftover pizzas...\n";
	return deliveries;
}

int main()
{
	const string in_prefix = "../../input_files/";
	const string out_prefix = "../../output_files/sol2/";
	vector<string> input_files = { "a_example.in", "b_little_bit_of_everything.in", "c_many_ingredients.in", "d_many_pizzas.in", "e_many_teams.in" };

	for (auto in_file_it = input_files.begin(); in_file_it != input_files.end(); ++in_file_it)
	{
		//if ((*in_file_it) != "e_many_teams.in")
			//continue;

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