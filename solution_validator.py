
def check_out_file(prefix, filename, nr_pizzas, teams_of_2, teams_of_3, teams_of_4):
        with open(prefix + filename, "r") as fin:
            nr_deliveries = int(fin.readline().strip())

            # Number of deliveries should be a positive integer
            assert(nr_deliveries > 0)

            pizzas_used = set()
            for _ in range(nr_deliveries):
                delivery = fin.readline().strip().split(" ")
                nr_pizzas_in_del = int(delivery[0])
                # There should be 2, 3, or 4 pizzas in a delivery
                assert(nr_pizzas_in_del >= 2 and nr_pizzas_in_del <= 4)

                # Making sure that a pizza never appears twice
                for pizza in delivery[1:]:
                    appears = False
                    if int(pizza) in pizzas_used:
                        appears = True
                    assert(appears == False)
                    pizzas_used.add(int(pizza))    


input_files = ["a_example.in", "b_little_bit_of_everything.in", "c_many_ingredients.in", "d_many_pizzas.in", "e_many_teams.in"]

for input_file in input_files:
    with open("input_files/" + input_file, "r") as fin:
        first_line = fin.readline()
        info = first_line.strip().split(" ")

        # Read the general information
        nr_pizzas = int(info[0])
        teams_of_2 = int(info[1])
        teams_of_3 = int(info[2])
        teams_of_4 = int(info[3])

        print("Now checking for dataset " + input_file)

        output_file = input_file.split(".")[0] + ".out"
        print("\tChecking for sol1")
        check_out_file("output_files/sol1/", output_file, nr_pizzas, teams_of_2, teams_of_3, teams_of_4)
        print("\tAll is good")

        print("\tChecking for sol2")
        check_out_file("output_files/sol2/", output_file, nr_pizzas, teams_of_2, teams_of_3, teams_of_4)
        print("\tAll is good")
        
        
                
        
        

        

        
