#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <cassert>
#include <cmath>
#include <bitset>
#define MAX_UNIQUE_INGR 10001

using namespace std;

class PrePizza
{
public:
	int nr_ingredients;
	string *ingredients;

	PrePizza(istream &is)
	{
		is >> this->nr_ingredients;
		this->ingredients = new string[this->nr_ingredients];
		for (int index = 0; index < this->nr_ingredients; ++index)
			is >> this->ingredients[index];
	}

	PrePizza(const PrePizza &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->ingredients = new string[this->nr_ingredients];
		for (int index = 0; index < this->nr_ingredients; ++index)
			this->ingredients[index] = other.ingredients[index];
	}

	~PrePizza()
	{
		delete[] ingredients;
	}
};

class Pizza
{
public:

	int nr_ingredients;
	bitset<MAX_UNIQUE_INGR> ingr_is_present;
	int initial_index;
	int total_unique_ingredients;
	double score;

	Pizza() : nr_ingredients{ 0 }, initial_index{ -1 }, score{ -1 }, total_unique_ingredients{ 0 }
	{
	}

	Pizza(const PrePizza &base, map<string, int> &ingr_to_num, int total_unique_ingredients, int index, const int *ingr_rarity, int total_ingr_count) :
		nr_ingredients{ base.nr_ingredients }, initial_index{ index }, total_unique_ingredients{ total_unique_ingredients }
	{
		int rarity_sum = 0;
		for (int index = 0; index < nr_ingredients; ++index)
		{
			auto it = ingr_to_num.find(base.ingredients[index]);
			ingr_is_present[(*it).second] = true;

			rarity_sum += ingr_rarity[(*it).second];
		}

		score = ((double)rarity_sum / total_ingr_count * total_unique_ingredients) * 0.25 + nr_ingredients * 0.75;
		//cout << "score " << score << endl;
	}

	Pizza(const Pizza &other) :
		nr_ingredients{ other.nr_ingredients }, initial_index{ other.initial_index }, total_unique_ingredients{ other.total_unique_ingredients }
	{
		this->ingr_is_present = other.ingr_is_present;
		this->score = other.score;
	}

	Pizza &operator=(const Pizza &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->initial_index = other.initial_index;
		this->total_unique_ingredients = other.total_unique_ingredients;

		this->ingr_is_present = other.ingr_is_present;
		this->score = other.score;

		return *this;
	}

	bool operator== (Pizza &rhs)
	{
		return (this->initial_index == rhs.initial_index);
	}

	bool operator !=(Pizza &rhs)
	{
		return (this->initial_index != rhs.initial_index);
	}
};

class Delivery
{
public:
	int nr_pizzas, nr_ingredients;
	char *ingr_is_present;
	list<Pizza>::iterator pos_in_list[4];
	Pizza pizzas[4];
	double score;

	Delivery()
	{
		nr_pizzas = nr_ingredients = 0;
		ingr_is_present = NULL;
		score = 0;
	}

	Delivery(list<Pizza>::iterator first_pizza_it, int total_unique_ingredients)
	{
		pizzas[0] = *(first_pizza_it);
		pos_in_list[0] = first_pizza_it;
		this->score = (*first_pizza_it).score;

		nr_ingredients = (*first_pizza_it).nr_ingredients;
		nr_pizzas = 1;
		ingr_is_present = new char[total_unique_ingredients];

		// Initializing the ingr freq array with the ingredients of the starting pizza
		for (int ingr_index = 0; ingr_index < total_unique_ingredients; ++ingr_index)
			ingr_is_present[ingr_index] = pizzas[0].ingr_is_present[ingr_index];
	}

	void add_pizza(list<Pizza>::iterator pizza_it, const int *ingr_rarity, int total_ingr_count, int total_unique_ingr)
	{
		pizzas[nr_pizzas] = *(pizza_it);
		pos_in_list[nr_pizzas] = pizza_it;
		this->nr_pizzas++;

		int rarity_sum = 0;
		nr_ingredients = 0;

		for (int ingr_index = 0; ingr_index < total_unique_ingr; ++ingr_index)
		{
			ingr_is_present[ingr_index] += (*pizza_it).ingr_is_present[ingr_index];

			if (ingr_is_present[ingr_index])
			{
				++nr_ingredients;
				rarity_sum += ingr_rarity[ingr_index];
			}
		}

		score = ((double)rarity_sum / total_ingr_count * total_unique_ingr) * 0.25 + nr_ingredients * 0.75;
	}

	double replace_profit(list<Pizza>::iterator pizza_it, int replace_index, const int *ingr_rarity, int total_ingr_count, int total_unique_ingr)
	{
		int new_nr_ingredients = 0;
		double new_score = 0;
		int rarity_sum = 0;

		for (int ingr_index = 0; ingr_index < pizzas[0].total_unique_ingredients; ++ingr_index)
		{
			bool ingr_exists = ((ingr_is_present[ingr_index] - pizzas[replace_index].ingr_is_present[ingr_index] +
				(*pizza_it).ingr_is_present[ingr_index]) > 0);

			if (ingr_exists)
			{
				++new_nr_ingredients;
				rarity_sum += ingr_rarity[ingr_index];
			}
		}

		new_score = ((double)rarity_sum / total_ingr_count * total_unique_ingr) * 0.25 + nr_ingredients * 0.75;

		return new_score - score;
	}

	void replace_index(list<Pizza>::iterator pizza_it, int replace_index, const int *ingr_rarity, int total_ingr_count, int total_unique_ingr)
	{
		if (replace_index == nr_pizzas)
		{
			add_pizza(pizza_it, ingr_rarity, total_ingr_count, total_unique_ingr);
			return;
		}

		nr_ingredients = 0;
		int rarity_sum = 0;

		for (int ingr_index = 0; ingr_index < total_unique_ingr; ++ingr_index)
		{
			ingr_is_present[ingr_index] -= (pizzas[replace_index].ingr_is_present[ingr_index] -
				(*pizza_it).ingr_is_present[ingr_index]);
			assert(ingr_is_present[ingr_index] >= 0);

			if (ingr_is_present[ingr_index])
			{
				++nr_ingredients;
				rarity_sum += ingr_rarity[ingr_index];
			}
		}

		score = ((double)rarity_sum / total_ingr_count * total_unique_ingr) * 0.25 + nr_ingredients * 0.75;

		pizzas[replace_index] = (*pizza_it);
		pos_in_list[replace_index] = pizza_it;
	}

	bool in_delivery(const Pizza &pizza)
	{
		for (int index = 0; index < nr_pizzas; ++index)
			if (pizzas[index].initial_index == pizza.initial_index)
				return true;
		return false;
	}

	Delivery(const Delivery &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->nr_pizzas = other.nr_pizzas;
		this->score = other.score;

		for (int i = 0; i < nr_pizzas; ++i)
		{
			this->pizzas[i] = other.pizzas[i];
			this->pos_in_list[i] = other.pos_in_list[i];
		}

		this->ingr_is_present = new char[other.pizzas[0].total_unique_ingredients];
		for (int ingr_index = 0; ingr_index < other.pizzas[0].total_unique_ingredients; ++ingr_index)
			this->ingr_is_present[ingr_index] = other.ingr_is_present[ingr_index];
	}

	Delivery &operator=(const Delivery &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->nr_pizzas = other.nr_pizzas;
		this->score = other.score;

		for (int i = 0; i < nr_pizzas; ++i)
		{
			this->pizzas[i] = other.pizzas[i];
			this->pos_in_list[i] = other.pos_in_list[i];
		}

		this->ingr_is_present = new char[other.pizzas[0].total_unique_ingredients];
		for (int ingr_index = 0; ingr_index < other.pizzas[0].total_unique_ingredients; ++ingr_index)
			this->ingr_is_present[ingr_index] = other.ingr_is_present[ingr_index];

		return *this;
	}

	void release_memory()
	{
		delete[] ingr_is_present;
		ingr_is_present = NULL;
	}

	~Delivery()
	{
		if (ingr_is_present != NULL)
			delete[] ingr_is_present;
	}
};

class OutputForm
{
public:
	int nr_pizzas, nr_ingredients;
	int pizza_indices[4];

	OutputForm(Delivery &delivery)
	{
		this->nr_pizzas = delivery.nr_pizzas;
		//cout << this->nr_pizzas << endl;
		this->nr_ingredients = delivery.nr_ingredients;
		for (int i = 0; i < nr_pizzas; ++i)
			this->pizza_indices[i] = delivery.pizzas[i].initial_index;
	}
};

class Data
{
public:
	int nr_pizzas, teams_of_2, teams_of_3, teams_of_4, total_unique_ingr, total_ingr_count;
	list<Pizza> pizzas;
	int *ingr_rarity;

	Data(string filename)
	{
		ifstream fin(filename);
		// Reading the nr of pizzas and teams
		fin >> nr_pizzas >> teams_of_2 >> teams_of_3 >> teams_of_4;

		vector<PrePizza> pre_pizzas;

		// Reading all the pizzas
		for (int index = 0; index < nr_pizzas; ++index)
			pre_pizzas.push_back(PrePizza(fin));

		fin.close();

		map<string, int> ingr_to_num;
		int ingr_index = 0;

		// Assigning an index to each ingredient
		for (auto pizza_it = pre_pizzas.begin(); pizza_it != pre_pizzas.end(); ++pizza_it)
			for (int index = 0; index < (*pizza_it).nr_ingredients; ++index)
			{
				auto ingr_it = ingr_to_num.find((*pizza_it).ingredients[index]);

				if (ingr_it == ingr_to_num.end())
					ingr_to_num.insert(pair<string, int>((*pizza_it).ingredients[index], ingr_index++));
			}
		total_unique_ingr = ingr_index;

		// Determining the rarity of each ingredient
		ingr_rarity = new int[total_unique_ingr];
		memset(ingr_rarity, 0, total_unique_ingr * sizeof(int));

		total_ingr_count = 0;
		for (auto pizza_it = pre_pizzas.begin(); pizza_it != pre_pizzas.end(); ++pizza_it)
			for (int index = 0; index < (*pizza_it).nr_ingredients; ++index)
			{
				++total_ingr_count;
				auto ingr_it = ingr_to_num.find((*pizza_it).ingredients[index]);
				assert(ingr_it != ingr_to_num.end());
				++ingr_rarity[(*ingr_it).second];
			}

		vector<Pizza> temp;
		// Creating the pizzas
		for (int index = 0; index < nr_pizzas; ++index) {
			temp.push_back(Pizza(pre_pizzas[index], ingr_to_num, ingr_index, index, ingr_rarity, total_ingr_count));
		}

		// Sort the array descendingly based on the number of ingredients
		sort(temp.begin(), temp.end(),
			[](const Pizza &a, const Pizza &b) -> bool
		{
			//return a.score > b.score;
			return a.score < b.score;
		});

		// Create a list with the sorted elements
		list<Pizza> pizzas_unused(temp.begin(), temp.end());

		pizzas = move(pizzas_unused);
	}

	void write_to_file(string filename, vector<OutputForm> &deliveries)
	{
		ofstream fout(filename);
		unsigned long long total_score_external = 0;

		fout << deliveries.size() << '\n';
		for (unsigned index = 0; index < deliveries.size(); ++index)
		{
			total_score_external += (unsigned long long)pow(deliveries[index].nr_ingredients, 2);

			fout << deliveries[index].nr_pizzas << " ";
			for (int p_index = 0; p_index < deliveries[index].nr_pizzas; ++p_index)
				fout << deliveries[index].pizza_indices[p_index] << " ";
			fout << '\n';
		}
		cout << "The final delivery score is: " << total_score_external << "\n\n";
	}

	~Data()
	{
		delete[] ingr_rarity;
	}
};
