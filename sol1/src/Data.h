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
#define MAGIC_NUMBER 0.67
#define MIN_DECENT_DELIVERY 0.01

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

	Pizza() : nr_ingredients{ 0 }, initial_index{ -1 }
	{
	}

	Pizza(const PrePizza &base, map<string, int> &ingr_to_num, int total_unique_ingredients, int index) :
		nr_ingredients{ base.nr_ingredients }, initial_index{ index }
	{
		for (int index = 0; index < nr_ingredients; ++index)
		{
			auto it = ingr_to_num.find(base.ingredients[index]);
			ingr_is_present[(*it).second] = true;
		}
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
	int nr_pizzas, nr_ingredients, total_unique_ingr;
	list<Pizza>::iterator pos_in_list[4];
	Pizza pizzas[4];
	double score;

	Delivery() : nr_pizzas{ 0 }, nr_ingredients{ 0 }, score{ 0 }, total_unique_ingr{ 0 }
	{
	}

	Delivery(list<Pizza>::iterator first_pizza_it, int total_unique_ingr) : nr_pizzas{ 1 }, nr_ingredients{ (*first_pizza_it).nr_ingredients },
		score{ (double)(*first_pizza_it).nr_ingredients }, total_unique_ingr{ total_unique_ingr }
	{
		pizzas[0] = *(first_pizza_it);
		pos_in_list[0] = first_pizza_it;
	}

	void add_pizza(list<Pizza>::iterator pizza_it)
	{
		pizzas[nr_pizzas] = *(pizza_it);
		pos_in_list[nr_pizzas] = pizza_it;
		++(this->nr_pizzas);

		nr_ingredients = 0;

		int nr_ingredients_wasted = 0;
		for (int ingr_index = 0; ingr_index < total_unique_ingr; ++ingr_index)
		{
			int ingr_count = 0;
			for (int pizza_index = 0; pizza_index < nr_pizzas; ++pizza_index)
				ingr_count += pizzas[pizza_index].ingr_is_present[ingr_index];

			if (ingr_count)
				++nr_ingredients;

			nr_ingredients_wasted += ((ingr_count <= 1) ? 0 : ingr_count - 1);
		}

		score = (double)nr_ingredients - (nr_ingredients_wasted * MAGIC_NUMBER);
		if (score < MIN_DECENT_DELIVERY)
			score = MIN_DECENT_DELIVERY;
	}

	double replace_profit(list<Pizza>::iterator pizza_it, int replace_index)
	{
		int new_nr_ingredients = 0;
		double new_score = 0;

		int nr_ingredients_wasted = 0;
		for (int ingr_index = 0; ingr_index < total_unique_ingr; ++ingr_index)
		{
			int ingr_count = 0;
			for (int pizza_index = 0; pizza_index < nr_pizzas; ++pizza_index)
				ingr_count += pizzas[pizza_index].ingr_is_present[ingr_index];
			ingr_count -= pizzas[replace_index].ingr_is_present[ingr_index];
			ingr_count += (*pizza_it).ingr_is_present[ingr_index];

			if (ingr_count)
				++new_nr_ingredients;

			nr_ingredients_wasted += ((ingr_count <= 1) ? 0 : ingr_count - 1);
		}

		new_score = (double)new_nr_ingredients - (nr_ingredients_wasted * MAGIC_NUMBER);
		if (new_score < MIN_DECENT_DELIVERY)
			new_score = MIN_DECENT_DELIVERY;
		return new_score - score;
	}

	void replace_index(list<Pizza>::iterator pizza_it, int replace_index)
	{
		if (replace_index == nr_pizzas)
		{
			add_pizza(pizza_it);
			return;
		}

		pizzas[replace_index] = (*pizza_it);
		pos_in_list[replace_index] = pizza_it;

		nr_ingredients = 0;
		int nr_ingredients_wasted = 0;
		for (int ingr_index = 0; ingr_index < total_unique_ingr; ++ingr_index)
		{
			int ingr_count = 0;
			for (int pizza_index = 0; pizza_index < nr_pizzas; ++pizza_index)
				ingr_count += pizzas[pizza_index].ingr_is_present[ingr_index];

			if (ingr_count)
				++nr_ingredients;
			
			nr_ingredients_wasted += ((ingr_count <= 1) ? 0 : ingr_count - 1);
		}
		score = (double)nr_ingredients - (nr_ingredients_wasted * MAGIC_NUMBER);
		if (score < MIN_DECENT_DELIVERY)
			score = MIN_DECENT_DELIVERY;
	}

	bool in_delivery(const Pizza &pizza)
	{
		for (int index = 0; index < nr_pizzas; ++index)
			if (pizzas[index].initial_index == pizza.initial_index)
				return true;
		return false;
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

		vector<Pizza> temp;
		// Creating the pizzas
		for (int index = 0; index < nr_pizzas; ++index) {
			temp.push_back(Pizza(pre_pizzas[index], ingr_to_num, ingr_index, index));
		}

		// Sort the array descendingly based on the number of ingredients
		sort(temp.begin(), temp.end(),
			[](const Pizza &a, const Pizza &b) -> bool
		{
			return a.nr_ingredients > b.nr_ingredients;
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
};