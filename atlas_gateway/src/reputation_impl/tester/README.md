# atlas_gateway_reputation_test
ATLAS IoT Reputation tester

Usage: 
- for Scenarios 1 to 4:
```sh
./atlas_gateway_reputation_test -n 1 -c 2 -f 2 -p 60 -e 3 -t 0.85 -i 10000
```
- for Scenario 5:
```sh
./atlas_gateway_reputation_test -n 5 -c 2 -p 30 -t 0.85 -i 10000
```
- for Scenarios 6
```sh
./atlas_gateway_reputation_test -n 6 -c 2 -r 0.4 -k 0.35 -v 0.25 -p 30 -t 0.85 -i 10000
```
- for Scenarios 7:
```sh
./atlas_gateway_reputation_test -n 7 -c 2 -f 2 -s 0.7 -r 0.4 -k 0.35 -v 0.25 -p 30 -t 0.85 -i 10000
```

Atlas gateway - reputation tester:

- --help ---> Display help message
- -n [ --scenario_number ] --> Scenario number: 
                                 - 1 ('c' clients with 'f' feature); 
                                 - 2 ('c' clients with 'f' features each: normal vs bad feedback); 
                                 - 3 ('c' clients with 'f' features each: switching feedback score at half of simulation); 
                                 - 4 ('c' clients with 'f' features for data plane and 3 system features and normal feedback)
                                 - 5 ('c' clients with 3 system features (fixed weight: 0.3 for REGISTER_TIME, 0,3 for KEEPALIVE_PACKETS and 0.4 for VALID_PACKETS) and normal feedback)
                                 - 6 ('c' clients with 3 system features with variable weight (parameters 'r', 'k' and 'v') and normal feedback)
                                 - 7 ('c' client with 'f' data feature and 3 system features with variable weight (parameters 'r', 'k' and 'v') and normal feedback. Through parameter 's', the weight of system features in the device reputation calculation can be modified.)
- -c [ --no_of_clients ] --> Number of clients that will be simulated (between 1 and 10)
- -f [ --no_of_features ] --> Number of dataplane features simulated for each client (between 1 and 5)
- -p [ --ref_data_value ] --> Bad feedback probability when generating random feedback values for dataplane (between 1 and 99)
- -t [ --threshold_value ] --> Feedback threshold value (between 0.1 and 0.95)
- -i [ --no_of_iterations ] --> Number of iterations the simulation should be run (between 1 and 50000)
- -s [ --sys_feat_weight ] --> Weight of all System features in the computation of device reputation (between 0.01 and 0.99)
- -r [ --reg_time_weight ] --> Weight of ATLAS_FEATURE_REGISTER_TIME feature (between 0.01 and 0.99). Sum of all three system features must be equal to 1.
- -k [ --ka_pkts_weight ] --> Weight of ATLAS_FEATURE_KEEPALIVE_PACKETS feature (between 0.01 and 0.99). Sum of all three system features must be equal to 1.
- -v [ --valid_pkts_weight ] --> Weight of ATLAS_FEATURE_VALID_PACKETS feature (between 0.01 and 0.99). Sum of all three system features must be equal to 1.


Depending on the number of selected 'c' clients, each scenario will output a number of 'c' files, named "scenario_#_client_#_.dat".

E.g.: for 2 clients in scenario 1, the resulsts will be written in files "scenario_1_client_1.dat" and "scenario_1_client_2.dat"

For viewing the results as a graph, we recommend using Gnuplot.
Usage:
- from a Terminal and run:
```sh
gnuplot
```
- from gnuplot application, execute the plot command. E.g: for the files obtained in the example above, the command wil be: 
```sh
plot "scenario_1_client_1.dat", "scenario_1_client_2.dat"
```
