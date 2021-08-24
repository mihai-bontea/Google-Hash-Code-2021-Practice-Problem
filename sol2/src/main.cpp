#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <Windows.h>
#include <algorithm>
#include <chrono>
#include "Data.h"
#define NR_ITERATED_BACK 7

using namespace std;

void backtrack(Delivery &delivery, int current_step, int nr_steps, Delivery &best_delivery, Data &data, int true_range, int last_index)
{
	auto it = data.pizzas.begin();
	bool first_time = true;
	for (int i = 0; i < true_range; ++i)
	{
		++it;
		// Ignore pizzas with an index smaller than the current one(we want combinations, not permutations)
		if (i <= last_index)
			continue;

		delivery.replace_index(it, current_step - 1);
		if (current_step < nr_steps)
			backtrack(delivery, current_step + 1, nr_steps, best_delivery, data, true_range, i);
		else
		{
			if (delivery.score >= best_delivery.score || first_time)
				best_delivery = delivery;
			first_time = false;
		}
	}
}

Delivery get_best_delivery(Delivery delivery, int delivery_size, Data &data, int range)
{
	int true_range = min((data.pizzas.size() - 1), range);
	Delivery result;

	if (delivery_size == 2)
	{
		result = delivery;
		auto it = data.pizzas.begin();
		++it;
		result.add_pizza(it);

		for (int i = 1; i < true_range; ++i)
		{
			++it;
			if (result.replace_profit(*it, 1) > 0)
				result.replace_index(it, 1);
		}

	}
	else
		backtrack(delivery, 2, delivery_size, result, data, true_range, -1);
	return result;
}

vector<Delivery> simulate(Data &data)
{
	vector<Delivery> deliveries;

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
			cout << best_of_2.score << " " << best_of_3.score << " " << best_of_4.score << best_score << endl;
			//cout << teams_of_2_left << " " << teams_of_3_left << " " << teams_of_4_left << endl;
			cout << best_of_3.pizzas[0].nr_ingredients << "  " << best_of_3.pizzas[1].nr_ingredients << " " << best_of_3.pizzas[2].nr_ingredients << endl;
		}

		// Removing the selected pizzas from the list
		for (int i = 0; i < final_delivery.nr_pizzas; ++i)
			data.pizzas.erase(final_delivery.pos_in_list[i]);

		// Adding best delivery to list
		if (final_delivery.score != 0)
			deliveries.push_back(final_delivery);
	}

	cout << "We have " << data.pizzas.size() << " leftover pizzas..." << endl;
	return deliveries;
}

void post_processing(vector<Delivery>::iterator it_beg, vector<Delivery>::iterator it_end, int &nr_improvements)
{
	using chrono::high_resolution_clock;
	using chrono::duration_cast;
	using chrono::minutes;
	const int max_minutes = 30;
	
	bool time_up = false;
	auto t1 = high_resolution_clock::now();
	for (auto it1 = it_beg; it1 != it_end; ++it1)
	{
		for (auto it2 = it1 + 1; it2 != it_end; ++it2)
		{
			bool ok = (*it1).swap_if_profit(*it2);
			if (ok)
			{
				++nr_improvements;
				break;
			}

			auto t2 = high_resolution_clock::now();
			auto s_int = duration_cast<minutes>(t2 - t1);
			if (s_int.count() >= max_minutes)
			{
				time_up = true;
				break;
			}
		}
		if (time_up)
			break;
	}
}

void post_processing_parallel(Data &data, vector<Delivery> &partial_result)
{
	auto vect_middle = partial_result.begin() + (partial_result.size() / 2);
	int nr_improvements1 = 0, nr_improvements2 = 0;

	thread t1(post_processing, partial_result.begin(), vect_middle, ref(nr_improvements1));
	thread t2(post_processing, vect_middle, partial_result.end(), ref(nr_improvements2));

	t1.join();
	t2.join();

	cout << "We have " << nr_improvements1 << " + " << nr_improvements2 << " improvements." << endl;
}

int main()
{
	const string in_prefix = "../../input_files/";
	const string out_prefix = "../../output_files/sol2/";
	vector<string> input_files = { "a_example.in", "b_little_bit_of_everything.in", "c_many_ingredients.in", "d_many_pizzas.in", "e_many_teams.in" };

	for (auto in_file_it = input_files.begin(); in_file_it != input_files.end(); ++in_file_it)
	{
		cout << "Now working on " << (*in_file_it);
		Data data(in_prefix + (*in_file_it));
		cout << ". Input processed." << endl;

		vector<Delivery> result = simulate(data);
		cout << "Starting post-processing..." << endl;
		post_processing_parallel(data, result);

		string out_filename = out_prefix + (*in_file_it).substr(0, ((*in_file_it).find('.'))) + ".out";
		data.write_to_file(out_filename, result);
	}

	system("pause");
	return 0;
}