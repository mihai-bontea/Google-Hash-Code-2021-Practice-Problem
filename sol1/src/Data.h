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
	bool *ingr_is_present;
	int initial_index;
	int total_unique_ingredients;
	double score;

	Pizza(): nr_ingredients{0}, ingr_is_present{NULL}, initial_index{-1}, score{-1}, total_unique_ingredients{0}
	{
	}

	Pizza(const PrePizza &base, map<string, int> &ingr_to_num, int total_unique_ingredients, int index, const int *ingr_rarity, int total_ingr_count):
		nr_ingredients{base.nr_ingredients}, initial_index{index}, total_unique_ingredients{total_unique_ingredients} 
	{
		//nr_ingredients = base.nr_ingredients;
		//initial_index = index;
		//this->total_unique_ingredients = total_unique_ingredients;
		ingr_is_present = new bool[total_unique_ingredients];
		memset(ingr_is_present, 0, total_unique_ingredients);

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

	Pizza(const Pizza &other):
		nr_ingredients{other.nr_ingredients}, initial_index{other.initial_index}, total_unique_ingredients{other.total_unique_ingredients}
	{
		//this->nr_ingredients = other.nr_ingredients;
		//this->initial_index = other.initial_index;
		//this->total_unique_ingredients = other.total_unique_ingredients;

		this->ingr_is_present = new bool[total_unique_ingredients];
		for (int index = 0; index < total_unique_ingredients; ++index)
			this->ingr_is_present[index] = other.ingr_is_present[index];
	}

	Pizza &operator=(const Pizza &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->initial_index = other.initial_index;
		this->total_unique_ingredients = other.total_unique_ingredients;

		this->ingr_is_present = new bool[total_unique_ingredients];
		for (int index = 0; index < total_unique_ingredients; ++index)
			this->ingr_is_present[index] = other.ingr_is_present[index];
		return *this;
	}

	~Pizza()
	{
		if (ingr_is_present != NULL)
			delete[] ingr_is_present;
	}
};

class Delivery
{
public:
	int nr_pizzas, nr_ingredients, score;
	int *ingr_is_present;
	list<Pizza>::iterator pos_in_list[4];
	Pizza pizzas[4];

	Delivery(list<Pizza>::iterator first_pizza_it, int total_unique_ingredients)
	{
		pizzas[0] = *(first_pizza_it);
		pos_in_list[0] = first_pizza_it;

		nr_ingredients = (*first_pizza_it).nr_ingredients;
		nr_pizzas = 1;
		ingr_is_present = new int[total_unique_ingredients];

		// Initializing the ingr freq array with the ingredients of the starting pizza
		for (int ingr_index = 0; ingr_index < total_unique_ingredients; ++ingr_index)
			ingr_is_present[ingr_index] = pizzas[0].ingr_is_present[ingr_index];
	}

	void add_pizza(list<Pizza>::iterator pizza_it, int total_unique_ingredients)
	{
		pizzas[nr_pizzas] = *(pizza_it);
		pos_in_list[nr_pizzas++] = pizza_it;

		for (int ingr_index = 0; ingr_index < total_unique_ingredients; ++ingr_index)
		{
			nr_ingredients += (!ingr_is_present[ingr_index] && (*pizza_it).ingr_is_present[ingr_index]) ? 1 : 0;

			ingr_is_present[ingr_index] += (*pizza_it).ingr_is_present[ingr_index];
		}
	}

	Delivery(const Delivery &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->nr_pizzas = other.nr_pizzas;
		this->score = score;

		for (int i = 0; i < nr_pizzas; ++i)
		{
			this->pizzas[i] = other.pizzas[i];
			this->pos_in_list[i] = other.pos_in_list[i];
		}

		this->ingr_is_present = new int[other.pizzas[0].total_unique_ingredients];
		for (int ingr_index = 0; ingr_index < other.pizzas[0].total_unique_ingredients; ++ingr_index)
			this->ingr_is_present[ingr_index] = other.ingr_is_present[ingr_index];
	}

	Delivery &operator=(const Delivery &other)
	{
		this->nr_ingredients = other.nr_ingredients;
		this->nr_pizzas = other.nr_pizzas;
		this->score = score;

		for (int i = 0; i < nr_pizzas; ++i)
		{
			this->pizzas[i] = other.pizzas[i];
			this->pos_in_list[i] = other.pos_in_list[i];
		}

		this->ingr_is_present = new int[other.pizzas[0].total_unique_ingredients];
		for (int ingr_index = 0; ingr_index < other.pizzas[0].total_unique_ingredients; ++ingr_index)
			this->ingr_is_present[ingr_index] = other.ingr_is_present[ingr_index];

		return *this;
	}

	~Delivery()
	{
		delete[] ingr_is_present;
	}
};

class OutputForm
{public:
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

	~Data()
	{
		delete[] ingr_rarity;
	}
};